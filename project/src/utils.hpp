#pragma once

#include <cstdarg>
#include <cstdio>

void fp_log(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printf("\033[32m[fp] [I]\033[0m ");
  vprintf(format, args);
  va_end(args);
}

void fp_err(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printf("\033[31m[fp] [E]\033[0m ");
  vprintf(format, args);
  va_end(args);
}

template <typename T, typename U> constexpr T limit(T x, U a)
{
  if (x < -a)
    return -a;
  if (x > a)
    return a;
  return x;
}
