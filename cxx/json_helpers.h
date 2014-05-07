/*
 * json_helpers.h
 *
 *  Created on: May 7, 2014
 *      Author: brianchin
 */

#ifndef JSON_HELPERS_H_
#define JSON_HELPERS_H_

#include <string>

// Strips out comments from otherwise well-formed JSON files. If an error
bool JSONStripComments(std::string const& json_text, std::string* output);

#endif /* JSON_HELPERS_H_ */
