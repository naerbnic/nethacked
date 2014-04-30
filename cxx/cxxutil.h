#ifndef CPPUTIL_H
#define CPPUTIL_H

#include <string>
#include "tradstdc.h"

#define NO_COPY_OR_ASSIGN(ClassName) \
  ClassName& operator=(ClassName const& other) = delete; \
  ClassName(ClassName const& other) = delete

template <typename T, T... args>
struct Sequence {
};

std::string StrPrintf(char const* fmt, ...) PRINTF_F(1, 2);

#endif
