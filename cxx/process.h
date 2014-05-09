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

std::string CallExecutable(
    std::string const& path,
    std::vector<std::string> const& arguments);

#endif /* PROCESS_H_ */
