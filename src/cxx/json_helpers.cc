/*
 * json_helpers.cc
 *
 *  Created on: May 7, 2014
 *      Author: brianchin
 */

#include "cxx/json_helpers.h"

#include <iostream>

using std::string;

class JsonCommentStripper {
public:
  enum class State {
    OUTSIDE_STRING,
    INSIDE_STRING,
  };

  JsonCommentStripper(string const& str, string* output)
      : str_(str), output_(output), curr_index_(0),
        curr_state_(State::OUTSIDE_STRING) {}

  bool HandleNonStringCharacter() {
    switch (CurrChar()) {
      case '"':
        curr_state_ = State::INSIDE_STRING;
        Advance(1);
        break;

      case '/':
        // This is a possible comment. Check to see if either of our
        // comment strings work

        if (IsSubstringAtIndex("//")) {
          // End of line comment. Find the end of the line.
          Skip(2);
          SkipToNextNewlineOrEOF();
        } else if (IsSubstringAtIndex("/*")) {
          // Multiline comment. Find the end of the terminator, or an
          // error otherwise.

          Skip(2);
          if (!SkipToAfterClosingMultilineComment()) {
            return false;
          }
          // A multiline comment acts as whitespace, so insert at least one
          // space, so that "a/* foo */b" gets treated as two symbols.
          Emit(' ');
        } else {
          // This is a non-special slash. Not sure what it's doing here,
          // but we're not actually parsing the file, so...

          Advance(1);
        }
        break;

      default:
        Advance(1);
    }
    return true;
  }

  bool HandleStringCharacter() {
    switch (CurrChar()) {
      case '\\': {
        // Other than a unicode escape (which has exactly four hex digits
        // after it) all escapes are exactly one character. We can just
        // skip that character in terms of checking state to handle pretty
        // much all escapes. This will miss some invalid escapes, but
        // that means the original file is invalid. Thank you undefined
        // behavior!
        if (!Advance(2)) {
          return false;
        }
        break;
      }

      case '"': {
        // Time to close the string
        curr_state_ = State::OUTSIDE_STRING;
        Advance(1);
        break;
      }

      default: {
        // Random string value. Just copy it.
        Advance(1);
        break;
      }
    }

    return true;
  }

  bool Process() {
    // Even though technically the string may be in UTF-8 format, since all of
    // the symbols we're looking for are in the ASCII 7-bit subset, simple
    // string parsing should work even then. Just make sure we copy the bytes
    // along no matter what, and only skip over substrings we know about.

    output_->clear();

    while (curr_index_ < str_.size()) {
      switch (curr_state_) {
        case State::OUTSIDE_STRING:
          if (!HandleNonStringCharacter()) {
            return false;
          }
          break;

        case State::INSIDE_STRING:
          // We can't start a comment in here, but we need to make sure that we
          // leave the string at the right time.

          if (!HandleStringCharacter()) {
            return false;
          }
          break;
      }
    }
    return true;
  }

private:
  char CurrChar() {
    return str_[curr_index_];
  }

  int CharsLeft() {
    return str_.size() - curr_index_;
  }

  bool Skip(int n) {
    if (CharsLeft() < n) {
      return false;
    }

    curr_index_ += n;
    return true;
  }

  void Emit(char c) {
    output_->push_back(c);
  }

  bool Advance(int n) {
    if (CharsLeft() < n) {
      return false;
    }

    for (int i = 0; i < n; i++) {
      Emit(str_[curr_index_ + i]);
    }
    curr_index_ += n;
    return true;
  }

  bool IsSubstringAtIndex(string const& substring) {
    if (curr_index_ + substring.size() > str_.size()) {
      return false;
    }

    string test_string = str_.substr(curr_index_, substring.size());

    return substring == test_string;
  }

  void SkipToNextNewlineOrEOF() {
    while (curr_index_ < str_.size()) {
      // Not the most efficient, but that's not the point
      // Handle all three newline styles, just in case. The order is important
      // as it will find any newline in the stream of the three standard
      // types.
      if (IsSubstringAtIndex("\n")) {
        return;
      }

      if (IsSubstringAtIndex("\r\n")) {
        return;
      }

      if (IsSubstringAtIndex("\r")) {
        return;
      }

      curr_index_++;
    }
  }

  bool SkipToAfterClosingMultilineComment() {
    while (curr_index_ < str_.size()) {
      if (IsSubstringAtIndex("*/")) {
        curr_index_ += 2;
        return true;
      }
      curr_index_++;
    }

    return false;
  }

  string const& str_;
  string* output_;
  int curr_index_;
  State curr_state_;
};

bool JSONStripComments(string const& json_text, string* output) {
  JsonCommentStripper stripper(json_text, output);
  return stripper.Process();
}
