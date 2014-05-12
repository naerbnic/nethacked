/*
 * cmdline.h
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <string>
#include <vector>

namespace cmdline {

std::vector<std::string> ReadArgv(int argc, char const* argv[]);

class CommandLineParser {
public:
};

}  // namespace cmdline

#endif /* CMDLINE_H_ */
