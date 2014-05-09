/*
 * exception.cc
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#include "exception.h"

#include <sstream>

using std::string;
using std::endl;

char const* Exception::what() const noexcept {
  if (!what_message_) {
    std::stringstream stream;
    stream << message_ << endl;
    try {
      string trace = stack_trace_.ToString();
      stream << "Stack Trace: " << endl;
      stream << trace;
    } catch (Exception const& e) {
      stream << "Unable to get stack trace: " << e.message_;
    } catch (...) {
      stream << "Unable to get stack trace";
    }
    what_message_.reset(new std::string(stream.str()));
  }

  return what_message_->c_str();
}

Exception::Exception(string const& message)
    : message_(message), stack_trace_(StackTrace::GetCurrent(2)){}

