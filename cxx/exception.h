/*
 * exception.h
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <memory>
#include <string>

#include "stacktrace.h"

class Exception : public std::exception {
public:
  Exception(std::string const& message);

  char const* what() const noexcept override;

private:
  std::string message_;
  StackTrace stack_trace_;
  mutable std::unique_ptr<std::string> what_message_;
};

#endif /* EXCEPTION_H_ */
