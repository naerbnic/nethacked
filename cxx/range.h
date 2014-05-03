// Class to manage integer ranges more conveniently.

#ifndef RANGE_H_
#define RANGE_H_

#include <type_traits>

template <
    typename T,
    typename = typename std::enable_if<std::is_integral<T>::value, void>::type>
class Range {
public:
  Range(T start, T end) : start_(start), end_(end) {}

  class iterator {
  public:
    iterator(T i) : i_(i) {}

    T operator*() {
      return i_;
    }

    bool operator!=(iterator other) {
      return i_ != other.i_;
    }

    iterator& operator++() {
      ++i_;
      return *this;
    }

  private:
    T i_;
  };

  iterator begin() {
    return iterator(start_);
  }

  iterator end() {
    return iterator(end_);
  }

  bool contains(T val) {
    return start_ <= val && val < end_;
  }

private:
  T start_;
  T end_;
};

#endif
