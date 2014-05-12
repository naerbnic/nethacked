/*
 * str_format.h
 *
 *  Created on: Apr 29, 2014
 *      Author: brianchin
 */

#ifndef STR_FORMAT_H_
#define STR_FORMAT_H_

#include <exception>
#include <string>
#include "core/tradstdc.h"

class FormattingErrorException : public std::exception {
public:
  // TODO(BNC): Add stack trace functionality
  FormattingErrorException(std::string const& what) : what_(what) {}
  char const* what() const throw() override {
    return what_.c_str();
  }

private:
  std::string what_;
};

namespace str_format_internal {
using std::string;

char const *FindNextFmtStart(char const *curr, char const *end) {
  // TODO: Add formatting for "%%"
  while (curr < end) {
    if (*curr == '%') {
      return curr;
    }
    curr++;
  }
  return end;
}

char const *ParseFmt(char const *curr, char const *end) {
  if (end - curr < 2) {
    return nullptr;
  }

  switch (curr[1]) {
    case 's':
      return curr + 2;

    default:
      return nullptr;
  }
}

template <typename Type, typename = void>
struct TypeFormatter {};

template <>
struct TypeFormatter<string> {
  static void WriteFormat(string *output, string const &arg,
                          string const &fmt) {
    if (fmt != "%s") {
      throw FormattingErrorException(
          "Bad string format directive: " + fmt);
    }

    output->append(arg);
  }
};

template <>
struct TypeFormatter<char const *> {
  static void WriteFormat(string *output, char const *arg, string const &fmt) {
    if (fmt != "%s") {
      throw FormattingErrorException(
          "Bad string format directive: " + fmt);
    }

    output->append(arg);
  }
};

template <typename... Args>
struct StrFormatInt {};

template <typename Head, typename... Rest>
struct StrFormatInt<Head, Rest...> {
  static void Format(string *output, char const *curr_ptr, char const *end_ptr,
                     Head head, Rest... rest) {
    char const *start_fmt_ptr = FindNextFmtStart(curr_ptr, end_ptr);
    if (start_fmt_ptr == end_ptr) {
      throw FormattingErrorException("Too many arguments");
    }

    output->append(curr_ptr, start_fmt_ptr);

    char const *end_fmt_ptr = ParseFmt(start_fmt_ptr, end_ptr);

    if (end_fmt_ptr == nullptr) {
      throw FormattingErrorException("Badly formatted directive");
    }

    string fmt(start_fmt_ptr, end_fmt_ptr);
    TypeFormatter<Head>::WriteFormat(output, head, fmt);

    StrFormatInt<Rest...>::Format(output, end_fmt_ptr, end_ptr, rest...);
  }
};

template <>
struct StrFormatInt<> {
  static void Format(string *output, char const *curr_ptr,
                     char const *end_ptr) {
    // We shouldn't have any more formatting terms
    char const *start_fmt_ptr = FindNextFmtStart(curr_ptr, end_ptr);
    if (start_fmt_ptr != end_ptr) {
      throw FormattingErrorException("Too few arguments");
    }

    output->append(curr_ptr, end_ptr);
  }
};
}

template <typename... Args>
std::string StrFormat(std::string fmt, Args... args) {
  std::string result;
  char const *start = fmt.data();
  char const *end = fmt.data() + fmt.size();
  str_format_internal::StrFormatInt<Args...>::Format(&result, start, end,
                                                     args...);
  return result;
}

#endif /* STR_FORMAT_H_ */
