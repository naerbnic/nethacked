
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "stacktrace.h"
#include "str_format.h"
#include "json_helpers.h"
#include "json_spirit.h"
#include "process.h"

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

  StackTrace trace = StackTrace::GetCurrent(1);
  cout << trace.ToString();
}
