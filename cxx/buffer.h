/*
 * buffer.h
 *
 *  Created on: May 2, 2014
 *      Author: brianchin
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>
#include <stdlib.h>

class Buffer {
public:
  // Returns a borrowed buffer. The passed in arguments for data and
  // size must outlive this value.
  Buffer OfBorrowed(void* data, size_t size);
  Buffer OfCopy(void* data, size_t size);
  Buffer OfNew(size_t size);

  Buffer(Buffer const& other);
  Buffer(Buffer&& other);

  Buffer& operator=(Buffer const& other);
  Buffer& operator=(Buffer&& other);

  void* raw_data() {
    return data_;
  }

  size_t size() { return size_; }

  template <typename T>
  T* As() {
    if (sizeof(T) != size_) {
      fprintf(stderr, "Trying to cast to differently-sized type!");
      exit(2);
    }
    return static_cast<T*>(data_);
  }

  void CopyFrom(size_t offset, void const* data, size_t size);
  void CopyFrom(void const* data, size_t size);
  void CopyFrom(size_t offset, Buffer const& other);
  void CopyFrom(Buffer const& other);

  void Copy(Buffer const& other);

  Buffer BorrowSubBuffer(size_t offset, size_t);
  Buffer Borrow();

private:
  Buffer(void* data, size_t size, bool owned);
  ~Buffer();

  void Release();

  void* data_;
  size_t size_;
  bool owned_;
};

#endif /* BUFFER_H_ */
