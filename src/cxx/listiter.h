#ifndef LISTITER_H_
#define LISTITER_H_

template <typename T, typename NextFunc>
class LinkedList {
  LinkedList(T **head, NextFunc next_func = NextFunc())
      : head_(head), next_func_(next_func) {}

  class iterator {
   public:
    iterator(T **curr) : curr_(curr) {}

    T *operator*() { return *curr_; }

    bool operator!=(iterator const &other) {
      if (curr_ == other.curr_) {
        return false;
      }

      return *curr_ != *other.curr_;
    }

    iterator &operator++() {
      curr_ = next_func_(*curr_);
      return *this;
    }

   private:
    T **curr_;
  };

 private:
  T **head_;
  NextFunc next_func_;
};

#endif  // LISTITER_H_
