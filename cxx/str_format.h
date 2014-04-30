/*
 * str_format.h
 *
 *  Created on: Apr 29, 2014
 *      Author: brianchin
 */

#ifndef STR_FORMAT_H_
#define STR_FORMAT_H_

#include <string>
#include "tradstdc.h"

void panic(const char *,...) PRINTF_F(1,2);

namespace str_format_internal {
  using std::string;

  char const* FindNextFmtStart(char const* curr, char const* end) {
    return nullptr;
  }

  char const* ParseFmt(char const* curr, char const* end) {
    return nullptr;
  }

  template <typename Type, typename = void>
  struct TypeFormatter {
  };

  template <>
  struct TypeFormatter<string> {
    static void WriteFormat(string* output, string const& arg, string const& fmt) {
      if (fmt != "%s") {
        panic("Bad string format directive: %s", fmt.c_str());
      }

      output->append(arg);
    }
  };

  template <typename... Args>
  struct StrFormatInt {
  };

  template <typename Head, typename... Rest>
  struct StrFormatInt<Head, Rest...> {
    static void Format(
        string* output,
        char const* curr_ptr, char const* end_ptr,
        Head head, Rest... rest) {
      char const* start_fmt_ptr = FindNextFmtStart(curr_ptr, end_ptr);
      if (start_fmt_ptr == end_ptr) {
        panic("Too many arguments to StrFormat");
      }

      while (curr_ptr < start_fmt_ptr) {
        output->push_back(*curr_ptr);
        ++curr_ptr;
      }

      char const* end_fmt_ptr = ParseFmt(curr_ptr, end_ptr);

      if (end_fmt_ptr == nullptr) {
        panic("Badly formatted directive");
      }

      string fmt(start_fmt_ptr, end_fmt_ptr);
      TypeFormatter<Head>::WriteFormat(output, head, fmt);

      StrFormatInt<Rest...>::Format(output, end_fmt_ptr, end_ptr, rest...);
    }
  };

  template <>
  struct StrFormatInt<> {
    static void Format(
        string* output,
        char const* curr_ptr, char const* end_ptr) {
      // We shouldn't have any more formatting terms
      char const* start_fmt_ptr = FindNextFmtStart(curr_ptr, end_ptr);
      if (start_fmt_ptr != end_ptr) {
        panic("Too many format directives");
      }

      while (curr_ptr < end_ptr) {
        output->push_back(*curr_ptr);
        ++curr_ptr;
      }
    }
  };
}

template <typename... Args>
std::string StrFormat(std::string fmt, Args... args) {
  std::string result;
  char const* start = fmt.data();
  char const* end = fmt.data() + fmt.size();
  str_format_internal::StrFormatInt<Args...>::Format(&result, start, end, args...);
  return result;
}


#endif /* STR_FORMAT_H_ */
