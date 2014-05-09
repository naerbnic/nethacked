/*
 * Process.h
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <vector>

#include "exception.h"

class ProcessException : public Exception {
public:
  ProcessException(std::string const& message) : Exception(message) {}
};

std::string CallExecutable(
    std::string const& path,
    std::vector<std::string> const& arguments);

#endif /* PROCESS_H_ */
