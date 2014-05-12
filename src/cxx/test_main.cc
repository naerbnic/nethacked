
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "cxx/exception.h"
#include "cxx/stacktrace.h"
#include "cxx/str_format.h"
#include "cxx/json_helpers.h"
#include "cxx/process.h"
#include "json_spirit.h"

using std::cout;
using std::endl;

int main(int argc, char const* argv[]) {
  cout << StrFormat("%s, %s!\n", "Hello", "World");
  //cout << StrFormat("%s, %s!\n %s\n", "Hello", "World");

  std::string json_with_comments = R"START(
[
  "foo//", // This
  "bar/*", // Is
  "baz", /*/ A */ "quux" // Test
]
)START";

  std::string result;
  if (!JSONStripComments(json_with_comments, &result)) {
    cout << "ERROR: Cannot parse JSON file!";
  }

  cout << result << endl;

  json_spirit::Value parse_result;
  json_spirit::read(result, parse_result);

  cout << parse_result.get_array()[0].get_str() << std::endl;
}
