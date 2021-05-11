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

int main(void)
{
  std::cerr << test_old() << " lines read." << std::endl;
  return 0;
}
