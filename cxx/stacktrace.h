/*
 * stacktrace.h
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#ifndef STACKTRACE_H_
#define STACKTRACE_H_

#include <string>
#include <vector>

class StackTrace {
public:
  static StackTrace GetCurrent(int num_removed_frames);

  std::string ToString() const;

private:
  StackTrace(void** addrs, int length);

  std::vector<void*> addresses_;
};

#endif /* STACKTRACE_H_ */
