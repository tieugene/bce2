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
  {"none", KVTYPE_NONE}
#ifdef USE_KC
  ,{"kcf", KVTYPE_KCFILE}
  ,{"kcm", KVTYPE_KCMEM}
#endif
#ifdef USE_TK
  ,{"tkf", KVTYPE_TKFILE}
  ,{"tkm", KVTYPE_TKMEM}
#endif
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
-s n      - logging step (default 1)\n\
-v[n]     - verbosity (0..3, to stderr)\
";

void        __prn_opts(void) {
    cerr
        << "Options:" << endl
        << TAB << "From:" << TAB << OPTS.from << endl
        << TAB << "Num:" << TAB << OPTS.num << endl
        << TAB << "Dat dir:" << TAB << OPTS.datdir << endl
        << TAB << "Locs file:" << TAB << OPTS.locsfile << endl
        << TAB << "K-V dir:" << TAB << OPTS.kvdir << endl
        << TAB << "K-V type:" << TAB << OPTS.kvngin << endl
        << TAB << "K-V tune:" << TAB << OPTS.kvtune << endl
        << TAB << "Cin:" << TAB << OPTS.fromcin << endl
        << TAB << "Debug:" << TAB << OPTS.verbose << endl
        << TAB << "Out:" << TAB << OPTS.out << endl
        << TAB << "Log by:" << TAB << OPTS.logstep << endl
    ;
}

/// Load options from config
bool load_cfg(void) {
  ifstream f_in(filesystem::path(getenv("HOME")) / cfg_file_name);
  if(f_in) {
    string datdir, locsfile, kvdir, kvngin;
    int verbose = -1;
    unsigned long logstep = 0;
    CFG::ReadFile(
          f_in,
          vector<string>{"datdir", "locsfile", "kvdir", "kvtype", "tune", "verbose", "out", "stdin", "logby"},
          datdir, locsfile, kvdir, kvngin, OPTS.kvtune, verbose, OPTS.out, OPTS.fromcin, logstep);
    f_in.close();
    if (verbose >= 0)
      OPTS.verbose = DBG_LVL_T(verbose);
    // fileststem::path workaround
    if (!datdir.empty())
      OPTS.datdir = datdir;
    if (!locsfile.empty())
      OPTS.locsfile = locsfile;
    if (!kvdir.empty())
      OPTS.kvdir = kvdir;
    if (!kvngin.empty()) {
      auto kt = kvnames.find(kvngin);
      if (kt != kvnames.end())
        OPTS.kvngin = kt->second;
      else
        if (OPTS.verbose)
          return b_error("Unknow k-v type: " + kvngin);
    }
    if (logstep > 0)
      OPTS.logstep = logstep;
  }
  return true;
}

/// Handle CLI. Return 0 if error, argv's index of 1st filename on success.
bool        cli(int argc, char *argv[]) {
    int opt, tmp;
    long tmp_l;
    auto kt = kvnames.begin();
    bool direct = false;

    while ((opt = getopt(argc, argv, "hf:n:d:l:k:e:t:s:cov::")) != -1) {  // FIXME: v?
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
          OPTS.datdir = optarg;
          direct = true;
          break;
        case 'l':
          if (OPTS.fromcin)
            return b_error("-l conflicts with -c");
          OPTS.locsfile = optarg;
          direct = true;
          break;
        case 'k':
          OPTS.kvdir = optarg;
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
          OPTS.fromcin = true;
          break;
        case 'o':
          OPTS.out = true;
          break;
        case 'v':   // FIXME: optarg = 0..5
          tmp = atoi(optarg);
          if (tmp < 0 or tmp > DBG_MAX)
            return b_error("-v: Bad verbose level " + string(optarg));
          OPTS.verbose = DBG_LVL_T(tmp);
          break;
        case 's':   // FIXME: optarg < 0 | > 999999
          tmp = atoi(optarg);
          if (tmp < 1 or tmp > 700000)
            return b_error("Bad -s: " + string(optarg));
          OPTS.logstep = tmp;
          break;
        case 'h':
        case '?':   // can handle optopt
          return b_error(help_txt);
      }
    }
    // opterr - always 1; optind - 1-st unhandled is argv[optarg] (if argc > optind), so argc_last = argc - optind;
    if (OPTS.verbose > 1)   // TODO: up v-level
        __prn_opts();
    return true;
}

bool load_opts(int argc, char *argv[]) {
  if (load_cfg() and cli(argc, argv)) {
    // post-check
    if (OPTS.fromcin) {
      fseek(stdin, 0, SEEK_END);
      if (ftell(stdin) >= 0)
        return b_error("Stdin is empty");
      rewind(stdin);
    } else {
      if (!filesystem::exists(OPTS.datdir))
        return b_error("datdir: '" + string(OPTS.datdir) + "' not exists");
      if (!filesystem::is_directory(OPTS.datdir))
        return b_error("datdir: '" + string(OPTS.datdir) + "' is not dir");
      if (!filesystem::exists(OPTS.locsfile))
        return b_error("locsfile: '" + string(OPTS.locsfile) + "' not exists");
    }
    if (kv_mode()) {
      if (!filesystem::exists(OPTS.kvdir))
        return b_error("k-v: '" + string(OPTS.kvdir) + "' not exists");
      if (!filesystem::is_directory(OPTS.kvdir))
        return b_error("k-v: '" + string(OPTS.kvdir) + "' is not dir");
    }
    return true;
  }
  return false;
}
