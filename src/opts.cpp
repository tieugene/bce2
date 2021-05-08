/*
 * opts.cpp - options processing (cfg, CLI)
 */
//#include <cstdlib>
//#include <cstring>
//#include <filesystem>
//#include <stdio.h>
#include <unistd.h> // getopt()
#include "bce.h"
#include "misc.h"
#include "config_file.h"  // https://github.com/fbarberm/SimpleConfigFile

using namespace std;

const string cfg_file_name = ".bce2.cfg";
const string  help_txt = "\
Usage: [options] (- | <dat_dir> <locs_file>)\n\
Options:\n\
-h        - this help\n\
-f n      - block starts from (default=0)\n\
-n n      - blocks to process (default=1, 0=all)\n\
-d <path> - *.dat folder\n\
-l <path> - locs-file\n\
-c        - hex input from stdin (conflicts w/ -d and -l)\n\
-k <path> - file-based key-value folder\n\
-o        - output results\n\
-v[n]     - verbosity (0..3, to stderr)\
";

void        __prn_opts(void) {
    cerr
        << "Options:" << endl
        << TAB << "From:" << TAB << OPTS.from << endl
        << TAB << "Num:" << TAB << OPTS.num << endl
        << TAB << "Dat dir:" << TAB << OPTS.datdir << endl
        << TAB << "Locs file:" << TAB << OPTS.locsfile << endl
        << TAB << "K-V dir:" << TAB << OPTS.cachedir << endl
        << TAB << "Cin:" << TAB << OPTS.fromcin << endl
        << TAB << "Debug:" << TAB << OPTS.verbose << endl
        << TAB << "Out:" << TAB << OPTS.out << endl
    ;
}

void load_cfg(void) {
  string datdir, locsfile, kvdir;
  ifstream f_in(filesystem::path(getenv("HOME")) / cfg_file_name);
  if(f_in) {
      CFG::ReadFile(f_in, vector<string>{"datdir", "locsfile", "kvdir", "verbose", "out"}, datdir, locsfile, kvdir, OPTS.verbose, OPTS.out);
      f_in.close();
      if (!datdir.empty())
        OPTS.datdir = datdir;
      if (!locsfile.empty())
        OPTS.locsfile = locsfile;
      if (!kvdir.empty())
        OPTS.cachedir = kvdir;
  }
}

bool        cli(int argc, char *argv[]) {
    int opt, tmp;
    bool retvalue = false, direct = false;

    while ((opt = getopt(argc, argv, "hf:n:d:l:k:cov::")) != -1) {  // FIXME: v?
      switch (opt) {
        case 'f':   // FIXME: optarg < 0 | > 999999
          tmp = atoi(optarg);
          if (tmp < 0 or tmp > 700000) {
            cerr << "Bad -f: " << optarg << endl;
            return false;
          }
          OPTS.from = tmp;
          break;
        case 'n':   // FIXME: optarg < 1 | > 999999
          tmp = atoi(optarg);
          if (tmp < 0 or tmp > 700000) {
            cerr << "Bad -i: " << optarg << endl;
            return false;
          }
          OPTS.num = tmp;
          if (OPTS.num == 0)  // ?
              OPTS.num = 999999;
          break;
        case 'd':
          if (OPTS.fromcin) {
            cerr << "-d conflicts with -c" << endl;
            return false;
          }
          if (!filesystem::exists(optarg)) {
            cerr << "-d: '" << optarg << "' not exists" << endl;
            return false;
          }
          if (!filesystem::is_directory(optarg)) {
            cerr << "-d: '" << optarg << "' id not dir" << endl;
            return false;
          }
          OPTS.datdir = optarg;
          direct = true;
          break;
        case 'l':
          if (OPTS.fromcin) {
            cerr << "-l conflicts with -c" << endl;
            return false;
          }
          if (!filesystem::exists(optarg)) {
            cerr << "-l: '" << optarg << "' not exists" << endl;
            return false;
          }
          OPTS.locsfile = optarg;
          direct = true;
          break;
        case 'k':
          if (!filesystem::exists(optarg)) {
            cerr << "-k: '" << optarg << "' not exists" << endl;
            return false;
          }
          if (!filesystem::is_directory(optarg)) {
            cerr << "-k: '" << optarg << "' is not dir" << endl;
            return false;
          }
          OPTS.cachedir = optarg;
          break;
        case 'c':
          if (direct) {
            cerr << "-c conflicts with -d/-l." << endl;
            return false;
          }
          // check stdin
          fseek(stdin, 0, SEEK_END);
          if (ftell(stdin) >= 0) {
            cerr << "Stdin is empty" << endl;
            return false;
          }
          rewind(stdin);
          OPTS.fromcin = true;
          break;
        case 'o':
          OPTS.out = true;
          break;
        case 'v':   // FIXME: optarg = 0..5
          OPTS.verbose = (optarg) ? (DBG_LVL_T)atoi(optarg) : DBG_MIN;
          break;
        case 'h':
        case '?':   // can handle optopt
          cerr << help_txt << endl;
          return false;
      }
    }
    // opterr - always 1; optind - 1-st unhandled is argv[optarg] (if argc > optind), so argc_last = argc - optind;
    OPTS.cash = !OPTS.cachedir.empty();
    retvalue = true;
    if (OPTS.verbose > 1)   // TODO: up v-level
        __prn_opts();
    return retvalue;
}
