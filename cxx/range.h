// Class to manage integer ranges more conveniently.

#ifndef RANGE_H_
#define RANGE_H_

#include <type_traits>

template <typename ArrayType, typename RangeType>
class ArrayRange;

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

  T range_start() const {
    return start_;
  }

  T range_end() const {
    return end_;
  }

  iterator begin() const {
    return iterator(start_);
  }

  iterator end() const {
    return iterator(end_);
  }

  bool contains(T val) const {
    return start_ <= val && val < end_;
  }

  template <typename ArrayType>
  ArrayRange<ArrayType, T> Over(ArrayType const& array) const {
    return ArrayRange<ArrayType, T>(array, *this);
  }

private:
  T start_;
  T end_;
};

template <typename ArrayType, typename RangeType>
class ArrayRange {
public:
  ArrayRange(ArrayType const& array, Range<RangeType> range)
      : array_(array), range_(range) {}

  class iterator {
  public:
    iterator(
        ArrayType const& array,
        typename Range<RangeType>::iterator iter)
        : array_(array), iter_(iter) {}

    bool operator!=(iterator other) {
      return iter_ != other.iter_;
    }

    iterator& operator++() {
      ++iter_;
      return *this;
    }

  private:
    ArrayType const& array_;
    typename Range<RangeType>::iterator iter_;

  public:
    // We're forced to go in order to get the typing to work right, so...
    typename std::remove_const<decltype(array_[*iter_])>::type& operator*() {
      return array_[*iter_];
    }
  };

  iterator begin() {
    return iterator(array_, range_.begin());
  }

  iterator end() {
    return iterator(array_, range_.end());
  }

private:
  ArrayType const& array_;
  Range<RangeType> range_;
};

template <typename T, int Len>
ArrayRange<T*, int> RangeOver(T(& array)[Len]) {
  return ArrayRange<T*, int>(array, Range<int>(0, Len));
}

#endif
