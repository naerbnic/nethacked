/*
 * json_helpers.cc
 *
 *  Created on: May 7, 2014
 *      Author: brianchin
 */

#include "json_helpers.h"

using std::string;

class JsonCommentStripper {
public:
  JsonCommentStripper(string const& str);

private:
  enum class State {
    OUTSIDE_STRING,
    INSIDE_STRING,
  };

  string const& str_;
  int curr_offset_;
  State curr_state_;
};

JsonCommentStripper::JsonCommentStripper(string const& str)
    : str_(str), curr_offset_(0), curr_state_(State::OUTSIDE_STRING) {}

bool CheckSubstringAtIndex(
    string const& checked_string, int index, string const& substring) {
  if (index + substring.size() > checked_string.size()) {
    return false;
  }

  return substring == checked_string.substr(index, index + substring.size());
}

int FindNextNewlineOrEOF(string const& json_text, int index) {
  for (int i = index; i < json_text.size(); ++i) {
    // Not the most efficient, but that's not the point
    if (CheckSubstringAtIndex(json_text, i, "\n")) {
      return i + 1;
    }

    if (CheckSubstringAtIndex(json_text, i, "\r\n")) {
      return i + 2;
    }

    if (CheckSubstringAtIndex(json_text, i, "\r")) {
      return i + 1;
    }
  }

  return json_text.size();
}

int FindClosingMultilineComment(string const& json_text, int index) {
  for (int i = index; i < json_text.size(); ++i) {
    // Not the most efficient, but that's not the point
    if (CheckSubstringAtIndex(json_text, i, "*/")) {
      return i + 2;
    }
  }

  return -1;
}

bool JSONStripComments(string const& json_text, string* output) {
  enum class State {
    OUTSIDE_STRING,
    INSIDE_STRING,
  };

  // Even though technically the string may be in UTF-8 format, since all of
  // the symbols we're looking for are in the ASCII 7-bit subset, simple string
  // parsing should work even then. Just make sure we copy the bytes along no
  // matter what.

  output->clear();

  State curr_state = State::OUTSIDE_STRING;
  int curr_index = 0;
  while (curr_index < json_text.size()) {
    char curr_char = json_text[curr_index];
    switch (curr_state) {
      case State::OUTSIDE_STRING: {
        switch (curr_char) {
          case '"': {
            curr_state = State::INSIDE_STRING;
            output->push_back(curr_char);
            curr_index++;
            break;
          }

          case '/': {
            // This is a possible comment. Check to see if either of our
            // comment strings work

            if (CheckSubstringAtIndex(json_text, curr_index, "//")) {
              // End of line comment. Find the end of the line.
              curr_index = FindNextNewlineOrEOF(json_text, curr_index + 2);
            } else if (CheckSubstringAtIndex(json_text, curr_index, "/*")) {
              // Multiline comment. Find the end of the terminator, or an
              // error otherwise.
              curr_index =
                  FindClosingMultilineComment(json_text, curr_index + 2);
              if (curr_index < 0) {
                // Ran off the end of the file. Shame.
                return false;
              }
            } else {
              // This is a non-special slash. Not sure what it's doing here, but
              // may as well emit it.

              output->push_back(curr_char);
              curr_index++;
            }
            break;
          }

          default: {
            output->push_back(curr_char);
            curr_index++;
          }
        }
        break;
      }

      case State::INSIDE_STRING: {
        // We can't start a comment in here, but we need to make sure that we
        // leave the string at the right time.

        switch (curr_char) {
          case '\'': {
            // Other than a unicode escape (which has exactly four hex digits
            // after it) all escapes are exactly one character. We can just
            // skip that character in terms of checking state to handle pretty
            // much all escapes.
            if (curr_index + 1 >= json_text.size()) {
              // This is the last character in the file. Huh. No idea what's
              // going on there.
              return false;
            }
            output->push_back(json_text[curr_index]);
            output->push_back(json_text[curr_index + 1]);
            curr_index += 2;
            break;
          }

          case '"': {
            // Time to close the string
            curr_state = State::OUTSIDE_STRING;
            output->push_back(curr_char);
            curr_index += 1;
            break;
          }

          default: {
            // Random string value. Just copy it.
            output->push_back(curr_char);
            curr_index += 1;
            break;
          }
        }

        break;
      }
    }

    return true;
  }


  return true;
}
