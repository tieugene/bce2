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

const std::map<std::string, KVNGIN_T> kvnames = {
  {"none", KVTYPE_NONE},
  {"kcf", KVTYPE_KCFILE},
  {"kcm", KVTYPE_KCMEM},
  {"tkf", KVTYPE_TKFILE},
  {"tkm", KVTYPE_TKMEM}
};
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
-e <name> - key-value engine (kcf/kcm/tkf/tkm)\n\
-t n      - k-v tuning (depends on engine)\n\
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
  string datdir, locsfile, kvdir, kvngin;
  int verbose = -1;
  ifstream f_in(filesystem::path(getenv("HOME")) / cfg_file_name);
  if(f_in) {
      CFG::ReadFile(
            f_in,
            vector<string>{"datdir", "locsfile", "kvdir", "kvtype", "tune", "verbose", "out"},
            datdir, locsfile, kvdir, kvngin, OPTS.kvtune, verbose, OPTS.out);
      f_in.close();
      if (verbose >= 0)
        OPTS.verbose = DBG_LVL_T(verbose);
      if (!datdir.empty())
        OPTS.datdir = datdir;
      if (!locsfile.empty())
        OPTS.locsfile = locsfile;
      if (!kvdir.empty())
        OPTS.cachedir = kvdir;
      if (!kvngin.empty()) {
        auto kt = kvnames.find(kvngin);
        if (kt != kvnames.end())
          OPTS.kvngin = kt->second;
        else
          if (OPTS.verbose)
            cerr << "Unknow k-v type: " << optarg << endl;
      }
  }
}

bool        cli(int argc, char *argv[]) {
    int opt, tmp;
    long tmp_l;
    auto kt = kvnames.begin();
    bool retvalue = false, direct = false;

    while ((opt = getopt(argc, argv, "hf:n:d:l:k:e:t:cov::")) != -1) {  // FIXME: v?
      switch (opt) {
        case 'f':   // FIXME: optarg < 0 | > 999999
          tmp = atoi(optarg);
          if (tmp < 0 or tmp > 700000)
            return b_error("Bad -f: " + string(optarg));
          OPTS.from = tmp;
          break;
        case 'n':   // FIXME: optarg < 1 | > 999999
          tmp = atoi(optarg);
          if (tmp < 0 or tmp > 700000)
            return b_error("Bad -i: " + string(optarg));
          OPTS.num = tmp;
          if (OPTS.num == 0)  // ?
              OPTS.num = 999999;
          break;
        case 'd':
          if (OPTS.fromcin)
            return b_error("-d conflicts with -c");
          if (!filesystem::exists(optarg))
            return b_error("-d: '" + string(optarg) + "' not exists");
          if (!filesystem::is_directory(optarg))
            return b_error("-d: '" + string(optarg) + "' is not dir");
          OPTS.datdir = optarg;
          direct = true;
          break;
        case 'l':
          if (OPTS.fromcin)
            return b_error("-l conflicts with -c");
          if (!filesystem::exists(optarg))
            return b_error("-l: '" + string(optarg) + "' not exists");
          OPTS.locsfile = optarg;
          direct = true;
          break;
        case 'k':
          if (!filesystem::exists(optarg))
            return b_error("-k: '" + string(optarg) + "' not exists");
          if (!filesystem::is_directory(optarg))
            return b_error("-k: '" + string(optarg) + "' is not dir");
          OPTS.cachedir = optarg;
          break;
        case 'e':
          kt = kvnames.find(optarg);
          if (kt == kvnames.end())
            return b_error("-e: unknown engine '" + string(optarg) + "'");
          OPTS.kvngin = kt->second;
          break;
        case 't':
          tmp_l = atol(optarg);
          if (tmp_l < 0)
            return b_error("Bad -t: " + string(optarg));
          OPTS.kvtune = tmp_l;
          break;
        case 'c':
          if (direct)
            return b_error("-c conflicts with -d/-l.");
          // check stdin
          fseek(stdin, 0, SEEK_END);
          if (ftell(stdin) >= 0)
            return b_error("Stdin is empty");
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
    //OPTS.cash = !OPTS.cachedir.empty();
    retvalue = true;
    if (OPTS.verbose > 1)   // TODO: up v-level
        __prn_opts();
    return retvalue;
}
