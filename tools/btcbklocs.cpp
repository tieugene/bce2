/* btcbklocs
 * Tool to create blocks file-offset file from blockchain leveldb
 * Using leveldb-devel (cmake and pkgconfig includable
 * LevelDB doc: https://github.com/google/leveldb)
 * BTC LDB structure: https://en.bitcoin.it/wiki/Bitcoin_Core_0.11_(ch_2):_Data_Storage#Block_index_.28leveldb.29
 * Out format: { uint32_t fileno, uint32_t offset } (8 bytes per block)
 * Bk:
 * - Header
 * - height
 * - txs
 * - To what extent this block is validated (?)
 * - In which file, and where in that file, the block data is stored
 * - In which file, and where in that file, the undo data is stored
 */

#include <iostream>
#include <fstream>  // files
#include <filesystem>
#include <sstream>  // stringstream
#include <unistd.h> // getopt
#include <leveldb/db.h>

using namespace std;

const uint64_t MAX32 = 0xFFFFFFFF;
const int MAXFILENO = 2999;
const int MAXOFFSET = 256 << 20; // 256MB > any blk*.dat
const int BLOCK_HAVE_DATA = 8;
const int BLOCK_HAVE_UNDO = 16;
static string  help_txt = "\
Usage: [options]\n\
Options:\n\
-i <path>   - indices folder\n\
-o <path>   - output file\n\
-x <path>   - hashes file (option)\n\
-b <path>   - blk*.dat folder (option)\n\
";

struct LDBREC_T {
    uint64_t    v,  // version
                h,  // height
                s,  // status
                t,  // nTx
                f,  // file no
                d,  // data offset
                u;  // undo offset
};

struct {     ///< program CLI options
    string      idir;
    string      ofile;
    string      xfile;
    string      bdir;
    bool        use_dat = false;
} OPTS;

string      __ptr2hex(const void *vptr, const size_t size)
{
    static string hex_chars = "0123456789abcdef";
    string s;
    auto cptr = reinterpret_cast<char const *>(vptr);
    for (size_t i = 0; i < size; i++, cptr++) {
        s.push_back(hex_chars[(*cptr & 0xF0) >> 4]);
        s.push_back(hex_chars[(*cptr & 0x0F)]);
    }
    return s;
}

bool        cli(int argc, char *argv[])
{
    int opt;
    bool retvalue = false;

    while ((opt = getopt(argc, argv, "i:o:x:n:b:")) != -1) {
        switch (opt) {
        case 'i':
            OPTS.idir = optarg;
            break;
        case 'o':
            OPTS.ofile = optarg;
            break;
        case 'x':
            OPTS.xfile = optarg;
            break;
        case 'b':
            OPTS.bdir = optarg;
            OPTS.use_dat = !OPTS.bdir.empty();
            break;
        default:   // can handle optopt
            cerr << help_txt << endl;
            return false;
        }
    }
    if (OPTS.idir.empty())
        cerr << "Set -i option" << endl;
    else if (OPTS.ofile.empty())
        cerr << "Set -o option" << endl;
    else if (!filesystem::exists(OPTS.idir))
        cerr << OPTS.idir << " not exists" << endl;
    else if (!OPTS.xfile.empty() and !filesystem::exists(OPTS.xfile))
        cerr << OPTS.xfile << " not exists" << endl;
    else if (!OPTS.bdir.empty() and !filesystem::exists(OPTS.bdir))
        cerr << OPTS.bdir << " not exists" << endl;
    else
        retvalue = true;
    return retvalue;
}

uint64_t    get_varint(const string& rec, uint32_t& pos)
{
    uint64_t result = 0;
    auto l = rec.size();
    while (pos <= l) {
        auto limb = int(rec[pos]);
        pos++;
        result <<= 7;
        result |= (limb & 0x7f);
        if (limb & 0x80)
            result++;
        else
            break;
    }
    return result;
}

bool        decode_rec(const string &value, LDBREC_T &rec)
{
    uint32_t vidx = 0;  // value string index
    rec.v = get_varint(value, vidx);
    rec.h = get_varint(value, vidx);
    rec.s = (get_varint(value, vidx));
    rec.t = (get_varint(value, vidx));
    // chk s
    rec.f = rec.d = rec.u = 0;
    if (rec.s & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO)) {
        rec.f = (get_varint(value, vidx));
        if (rec.f > MAXFILENO) {
            cerr << "File # " << rec.f << " too big." << endl;
            return false;
        }
        if (rec.s & BLOCK_HAVE_DATA) {
            rec.d = (get_varint(value, vidx));
            if (rec.d > MAXOFFSET) {
                cerr << "Data offset " << rec.d << " too big." << endl;
                return false;
            }
            if (rec.d < 8) {
                cerr << "Data offset " << rec.d << " too small." << endl;
                return false;
            }
        }
        if (rec.s & BLOCK_HAVE_UNDO) {
            rec.u = (get_varint(value, vidx));
            if (rec.u > MAXOFFSET) {
                cerr << "Undo offset " << rec.u << " too big." << endl;
                return false;
            }
            if (rec.u < 8) {
                cerr << "Undo offset " << rec.u << " too small." << endl;
                return false;
            }
        }
    }
    return true;
}

bool        chk_dat(uint64_t fileoffset)   ///< chk bk header
{
    const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE
    static uint32_t i = MAX32;
    static ifstream f;
    uint32_t sig, fno = fileoffset & MAX32, offset = fileoffset >> 32;

    if (fno != i) {
        if (f.is_open())
            f.close();
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << fno;
        string fname = OPTS.bdir + "/blk" + ss.str() + ".dat";
        f.open(fname, ios::in|ios::binary);
        if (!f.is_open()) {
            cerr << "Can't open to read: " << fname << endl;
            return false;
        }
        i = fno;
    }
    f.seekg(offset-8, f.beg);
    f.read(reinterpret_cast<char *>(&sig), sizeof(sig));
    return sig == BK_SIGN;
}

int main(int argc, char *argv[])
{
    leveldb::DB*        db;
    leveldb::Options    options;
    istream             *in;
    ifstream            xfile;
    ofstream            ofile;
    char                key[33];
    LDBREC_T            rec;

    // 0. cli
    if (!cli(argc, argv))
        exit(EXIT_FAILURE);
    // 1. Prepare
    // 1.1. open db
    options.create_if_missing = false;
    leveldb::Status status = leveldb::DB::Open(options, OPTS.idir, &db);
    if (!status.ok()) {
        cerr << "Can't open LevelDB: " << OPTS.idir << endl;
        exit(EXIT_FAILURE);
    }
    // 1.2. open input
    if (OPTS.xfile.empty())
        in = &cin;
    else {
        xfile.open(OPTS.xfile, ios::in);
        if (!xfile.is_open()) {
            cerr << "Can't open hash file: " << OPTS.xfile << endl;
            exit(EXIT_FAILURE);
        }
        in = &xfile;
    }
    // 1.3. open output
    ofile.open(OPTS.ofile, ios::out|ios::binary);
    if (!ofile.is_open()) {
        cerr << "Can't open output file: " << OPTS.ofile << endl;
        exit(EXIT_FAILURE);
    }
    // 1.4. misc
    key[0] = 'b';
    string s, value;
    size_t bk_no = 0;
    // 2. main loop
    while (!getline(xfile, s).eof()) {
        // if read ok
        if (s.length() != 64) {
            cerr << "Line " << bk_no << " is " << s.length() << " bytes != 64." << endl;
            exit(EXIT_FAILURE);
        }
        // 2.2. conver
        auto ptr = s.c_str()+62;
        for (auto j = 0; j < 32; j++, ptr -= 2) // reverse order
            sscanf(ptr, "%2hhx", key + j + 1);
        // 2.3. find
        if (!db->Get(leveldb::ReadOptions(), leveldb::Slice(key, sizeof(key)), &value).ok()) {
            cerr << bk_no << ": not found." << endl;
            exit(EXIT_FAILURE);
        }
        // 2.4. decode
        if (!decode_rec(value, rec)) {
            cerr << "Can' decode rec " << bk_no << endl;
            exit(EXIT_FAILURE);
        }
        if (rec.h != bk_no) {
            cerr << "Decode rec height " << rec.h << " <> " << bk_no << "waiting." << endl;
            exit(EXIT_FAILURE);
        }
        // 2.5. out
        uint64_t result = (rec.d << 32) | rec.f;
        ofile.write(reinterpret_cast<char *>(&result), sizeof(result));
        bk_no++;
    }
    // 3. that's all
    delete db;
    ofile.close();
    // x. that's all
    cerr << bk_no << " records wrote." << endl;
    exit(EXIT_SUCCESS);
}
