#include <execinfo.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <vector>

#include "stacktrace.h"
#include "process.h"

using std::string;

namespace {

constexpr int kAddressListSize = 256;
thread_local void* address_space[kAddressListSize] = {};

string PointerToHex(void* ptr) {
  std::stringstream stream;
  stream << ptr;
  return stream.str();
}

}

StackTrace StackTrace::GetCurrent(int num_removed_frames) {
  int size = backtrace(address_space, kAddressListSize);

  return StackTrace(address_space + num_removed_frames,
      std::max(0, size - num_removed_frames));
}

string StackTrace::ToString() const {
  pid_t pid = getpid();

  std::vector<string> pointer_strings;
  pointer_strings.reserve(addresses_.size());

  for (void* address : addresses_) {
    pointer_strings.push_back(PointerToHex(address));
  }

  std::vector<string> arguments = {
      "/usr/bin/xcrun", "atos", "-p", std::to_string(pid),
  };

  arguments.insert(
      arguments.end(),
      pointer_strings.begin(), pointer_strings.end());

  return CallExecutable("/usr/bin/xcrun", arguments);
}

StackTrace::StackTrace(void** address_ptr, int length)
    : addresses_(address_ptr, address_ptr + length) {}







