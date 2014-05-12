/*
 * json_helpers.h
 *
 *  Created on: May 7, 2014
 *      Author: brianchin
 */

#ifndef JSON_HELPERS_H_
#define JSON_HELPERS_H_

#include <string>

// Strips out comments from otherwise well-formed JSON files. Returns false if
// it was unable to parse out the comment. Passing in a non-well-formed JSON
// string as input results in an undefined (but valid) string as output, that
// should fail on JSON parsing.
bool JSONStripComments(std::string const& json_text, std::string* output);

#endif /* JSON_HELPERS_H_ */
