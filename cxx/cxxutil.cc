/*
 * cxxutil.cc
 *
 *  Created on: Apr 29, 2014
 *      Author: brianchin
 */

#include <cstdio>
#include <string>
#include <stdarg.h>

#include <memory>

#include "cxxutil.h"
#include "str_format.h"

using std::string;

constexpr int kInitialReserve = 128;

string StrPrintf(char const* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int curr_size = kInitialReserve;
  string result;
  do {
    va_list arg_copy;
    va_copy(va, arg_copy);
    std::unique_ptr<char[]> data(new char[curr_size]);
    int new_length = vsnprintf(data.get(), curr_size, fmt, arg_copy);
    if (new_length < curr_size) {
      result = data.get();
      break;
    }

    curr_size *= 2;
  } while (true);
  va_end(va);
  return result;
}



