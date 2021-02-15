/* btclocview
 * Tool to show blocks file-offset content.
 */

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

static string  help_txt = "Usage: btclocview <locs-file> <bk no>\n";

int main(int argc, char *argv[])
{
    int         bkno;
    ifstream    infile;
    uint64_t    offset;
    uint32_t    fileno, fileoff;

    // 1. cli
    // 1.1. opt qty
    if (argc != 3) {
        cerr << "Bad cli args." << endl << help_txt << endl;
        exit(EXIT_FAILURE);
    }
    // 1.2. file chk #1: exists
    if (!filesystem::exists(argv[1])) {
        cerr << argv[1] << " does not exists." << endl;
        exit(EXIT_FAILURE);
    }
    // 1.3.  file chk #2: is file
    if (!filesystem::is_regular_file(argv[1])) {
        cerr << argv[1] << " is not file." << endl;
        exit(EXIT_FAILURE);
    }
    // 1.4. 2nd arg test
    bkno = atoi(argv[2]);
    if (!bkno) {
        cerr << "Bad bk no: " << argv[2] << endl;
        exit(EXIT_FAILURE);
    }
    offset = bkno << 3;    // x8
    // 1.5. file chk #3: len
    if (filesystem::file_size(argv[1]) < (offset + 8)) {
        cerr << argv[1] << " is too short." << endl;
        exit(EXIT_FAILURE);
    }
    // 1.6. file chk #4: can be opened
    infile.open(argv[1], ios::in|ios::binary);
    if (!infile.is_open()) {
        cerr << "Can't open file: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    // 2. go
    infile.seekg(offset, infile.beg);
    infile.read(reinterpret_cast<char *>(&fileno), sizeof (fileno));
    infile.read(reinterpret_cast<char *>(&fileoff), sizeof (fileoff));
    // 3. that's all
    infile.close();
    // x. that's all
    printf("Bk %d: bk%05u.%u\n", bkno, fileno, fileoff);
    exit(EXIT_SUCCESS);
}
