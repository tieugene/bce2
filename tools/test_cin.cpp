/*
 * test_cin.cpp - test hex input from bitcoind
 */
#include <iostream>

const int  MAX_BK_SIZE = 4 << 20;  // 4MB enough

using namespace std;

int hex2bytes(const string& s, char *dst)
{
    auto src_ptr = s.c_str();
    auto src_end = src_ptr + s.length();
    char *dst_ptr;

    for (dst_ptr = dst; src_ptr < src_end; src_ptr += 2, dst_ptr++) {
        if (sscanf(src_ptr, "%2hhx", dst_ptr) != 1)
          break;
    }
    return dst_ptr - dst;
}

string      bytes2hex(const string_view &src)
{
    static string hex_chars = "0123456789abcdef";
    string s;
    auto cptr = src.begin();
    for (size_t i = 0; i < src.length(); i++, cptr++) {
        s.push_back(hex_chars[(*cptr & 0xF0) >> 4]);
        s.push_back(hex_chars[(*cptr & 0x0F)]);
    }
    return s;
}

bool read_line(char *dst) {
    string line;
    bool retvalue = false;

    fseek(stdin, 0, SEEK_END);
    cerr << "Ftell:" << ftell(stdin) << endl;
    rewind(stdin);
    getline(cin, line);
    if (line.empty()) {     // nothing to read == EOF
      cerr << "Line empty" << endl;
    } else if (line.length() > (MAX_BK_SIZE << 1)) {
      cerr << "Line too big" << endl;
    } else {
        auto line_len = line.length();
        cout << "Line read (" << line_len << " bytes):" << endl << line << endl;
        auto done = hex2bytes(line, dst);
        if (done != (line_len >> 1)) {
            cerr << done << " bytes converted " << endl;
        } else {
          cerr << "Done:" << endl << bytes2hex(string_view(dst, done)) << endl;
          retvalue = true;
        }
    }
    return retvalue;
}

int main(void)
{
    int counter, num = 10;
    char *dst = new char[MAX_BK_SIZE];

    for (counter = 0; num > 0; counter++, num--) {
        if (!read_line(dst)) {
            cerr << "Cannot read line" << endl;
            break;
        }
    }
}
