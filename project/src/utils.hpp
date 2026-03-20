#include <cstdio>
#include <cstdarg>

void fp_log(const char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("\033[32m[fp]\033[0m ");
  vprintf(format, args);
  va_end(args);
}
