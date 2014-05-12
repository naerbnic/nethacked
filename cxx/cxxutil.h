#ifndef CPPUTIL_H
#define CPPUTIL_H

#include <string>
#include "core/tradstdc.h"

template <typename T, T... args>
struct Sequence {};

std::string StrPrintf(char const *fmt, ...) PRINTF_F(1, 2);

#endif
