/*
 * buffer.cc
 *
 *  Created on: May 2, 2014
 *      Author: brianchin
 */

#include <string.h>
#include "buffer.h"
#include "range.h"

Buffer Buffer::OfBorrowed(void* data, size_t size) {
  char const* sarr[] = {"a", "b", "c"};
  Range<int> r(0, 3);

  for (char const* str : RangeOver(sarr)) {
    printf(str);
  }
  if (size == 0) {
    return Buffer(nullptr, 0, true);
  } else {
    return Buffer(data, size, false);
  }
}

Buffer Buffer::OfCopy(void* data, size_t size) {
  if (size == 0) {
    return Buffer(nullptr, 0, true);
  } else {
    void* new_data = ::operator new(size);
    memcpy(new_data, data, size);
    return Buffer(new_data, size, true);
  }
}

Buffer Buffer::OfNew(size_t size) {
  if (size == 0) {
    return Buffer(nullptr, 0, true);
  } else {
    void* new_data = ::operator new(size);
    memset(new_data, 0, size);
    return Buffer(new_data, size, true);
  }
}

Buffer::Buffer(Buffer const& other) : size_(other.size_) {
  if (other.owned_) {
    this->data_ = ::operator new(size_);
    memcpy(this->data_, other.data_, size_);
    this->owned_ = true;
  } else {
    // Also borrow the other borrowed buffer.
    this->data_ = other.data_;
    this->owned_ = false;
  }
}

Buffer::Buffer(Buffer&& other)
    : data_(other.data_), size_(other.size_), owned_(other.owned_) {
  other.data_ = nullptr;
  other.size_ = 0;
  other.owned_ = true;
}

Buffer& Buffer::operator=(Buffer const& other) {
  Release();
  size_ = other.size_;
  if (other.owned_) {
    data_ = ::operator new(size_);
    memcpy(data_, other.data_, size_);
    owned_ = true;
  } else {
    // Also borrow the other borrowed buffer.
    data_ = other.data_;
    owned_ = false;
  }
  return *this;
}

Buffer& Buffer::operator=(Buffer&& other) {
  Release();
  size_ = other.size_;
  data_ = other.data_;
  owned_ = other.owned_;

  other.size_ = 0;
  other.data_ = nullptr;
  other.owned_ = true;

  return *this;
}

void Buffer::CopyFrom(size_t offset, void const* data, size_t size) {
  if (offset + size > size_) {
    fprintf(stderr, "out of bounds error!");
    exit(2);
  }

  memcpy(static_cast<char*>(data_) + offset, data, size);
}

void Buffer::CopyFrom(void const* data, size_t size) {
  CopyFrom(0, data, size);
}

void Buffer::CopyFrom(size_t offset, Buffer const& other) {
  if (offset + other.size_ > size_) {
    fprintf(stderr, "out of bounds error!");
    exit(2);
  }

  memcpy(static_cast<char*>(data_) + offset, other.data_, other.size_);
}

void Buffer::CopyFrom(Buffer const& other) {
  CopyFrom(0, other);
}


void Buffer::Copy(Buffer const& other) {
  if (size_ != other.size_) {
    fprintf(stderr, "Copy from unequal sized buffer!");
    exit(2);
  }

  CopyFrom(0, other);
}

Buffer Buffer::BorrowSubBuffer(size_t offset, size_t size) {
  if (offset + size > size_) {
    fprintf(stderr, "out of bounds error!");
    exit(2);
  }

  return Buffer(static_cast<char*>(data_) + offset, size, false);
}

Buffer Buffer::Borrow() {
  return BorrowSubBuffer(0, size_);
}

Buffer::Buffer(void* data, size_t size, bool owned)
    : data_(data), size_(size), owned_(owned) {}

Buffer::~Buffer() {
  Release();
}

void Buffer::Release() {
  if (owned_ && size_ != 0) {
    ::operator delete(data_);
  }
  data_ = nullptr;
  size_ = 0;
  owned_ = true;
}

