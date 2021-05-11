#ifndef FASTHEX_H
#define FASTHEX_H

// bitcoin-cli receives lower hex (0-9a-f == 0x30..39,0x61..66[,0x41..46])

#include <type_traits>

///< requres filter before
inline int htoi(int x) {
    return 9 * (x >> 6) + (x & 017);
}

struct Table {
  long long tab[128];
  constexpr Table() : tab {} {
    tab['1'] = 1;
    tab['2'] = 2;
    tab['3'] = 3;
    tab['4'] = 4;
    tab['5'] = 5;
    tab['6'] = 6;
    tab['7'] = 7;
    tab['8'] = 8;
    tab['9'] = 9;
    tab['a'] = 10;
    tab['A'] = 10;
    tab['b'] = 11;
    tab['B'] = 11;
    tab['c'] = 12;
    tab['C'] = 12;
    tab['d'] = 13;
    tab['D'] = 13;
    tab['e'] = 14;
    tab['E'] = 14;
    tab['f'] = 15;
    tab['F'] = 15;
  }
  constexpr long long operator[](char const idx) const { return tab[(std::size_t) idx]; }
} constexpr table;

constexpr int hextoint(char number) {
  return table[(std::size_t)number];
}

#endif // FASTHEX_H
