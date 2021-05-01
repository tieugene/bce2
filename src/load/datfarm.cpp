//#include <cstdint>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <queue>

#include "datfarm.h"

using namespace std;

const uint8_t WIN_SIZE = 16;
static queue<size_t> opened;

bool        DATFARM_T::open(const size_t no)
{
    if (no > qty) {
        cerr << "File # too big: " << no << endl;
        return false;
    }
    if (file[no].is_open())
        return true;
    if (opened.size() >= WIN_SIZE) {  // close unused file
        file[opened.front()].close();
        opened.pop();
    }
    ostringstream ss;
    ss << setw(5) << setfill('0') << no;
    string fn = folder + "blk" + ss.str() + ".dat";
    file[no].open(fn, ios::in|ios::binary);
    if (!file[no].is_open()) {
        cerr << "Can't open file " << fn << endl;
        return false;
    }
    opened.push(no);
    return true;
}

bool        DATFARM_T::read(const size_t no, const size_t offset, const int size, void *dst)
{
    if (!open(no))
        return false;
    file[no].seekg(offset, file[no].beg);
    file[no].read(static_cast<char *>(dst), size);
    if (file[no].gcount() != size) {
        cerr << "Can't read " << size << " bytes from " << no << "." << offset << endl;
        return false;
    }
    return true;
}
