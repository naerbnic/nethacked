/*
 * stl_util.h
 *
 *  Created on: May 7, 2014
 *      Author: brianchin
 */

#ifndef STL_UTIL_H_
#define STL_UTIL_H_

#include <algorithm>
#include <type_traits>
#include <vector>

template <typename From, typename Predicate>
std::vector<typename std::result_of<Predicate(From)>::type> STLTransformVector(
    std::vector<From> const& input, Predicate const& pred) {
  std::vector<typename std::result_of<Predicate(From)>::type> result;

  std::transform(input.begin(), input.end(), std::back_inserter(result), pred);

  return result;
}



#endif /* STL_UTIL_H_ */
