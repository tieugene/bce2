#include <iostream>
#include <stdio.h>

size_t test_cin(void) {
  std::string line;
  size_t count = 0;

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  while (std::cin) {
    std::getline(std::cin, line);
    count++;
  }
  return count;
}

size_t test_old(void) {
  const int BUFSIZE = 3 + (1 << 24);
  char *inbuf = new char[BUFSIZE];
  size_t count = 0;
  while (std::fgets(inbuf, BUFSIZE - 1, stdin))
    count++;
  return count;
}

inline int htoi(int x) {
    return 9 * (x >> 6) + (x & 017);
}

int main(void)
{
  //std::cerr << test_old() << " lines read." << std::endl;
  for (char i = '0'; i < 127; i++) {
    std::cout << i << " = " << std::to_string(i) << " => " << std::to_string(htoi(i)) << std::endl;
  }
  return 0;
}
