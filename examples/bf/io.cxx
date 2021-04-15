#include "io.hxx"

#include <cstdio>

#if defined(_WIN32)

#include <conio.h>

#elif defined(__linux__) || defined(__APPLE__)

#include <termios.h>

#else
#error Unsupported operating system
#endif

namespace bf::io {
  void print_char(int c)
  {
    std::putchar(c);
  }

#if defined(_WIN32)

  int scan_char()
  {
    int const input = _getch();
    if (input=='\r') {
      return '\n';
    }

    return input;
  }

#elif defined(__linux__) || defined(__APPLE__)

  int scan_char()
  {
    termios old, cur;
    int result;

    tcgetattr(0, &old);
    cur = old;
    cur.c_lflag &= ~ICANON;
    cur.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &cur);

    result = std::getchar();

    tcsetattr(0, TCSANOW, &old);

    return result;
  }

#endif
}
