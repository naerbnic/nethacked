/*
 * library.cc
 *
 *  Created on: Apr 30, 2014
 *      Author: brianchin
 */

#include <memory>
#include <string>
#include <stdio.h>

#include "cxxutil.h"
#include "library.h"

using std::string;
using std::unique_ptr;

namespace {

class FileSystemLibraryFile : public LibraryFile {
public:
  FileSystemLibraryFile(FILE* file) : file_(file), closed_(false) {}
  ~FileSystemLibraryFile() {
    if (!closed_) {
      Close();
    }
  }

  int Read(char* buf, int len, int size) override {
    return fread(buf, len, size, file_);
  }

  int Seek(long offset, int whence) override {
    return fseek(file_, offset, whence);
  }

  char* GetString(char* buffer, int size) override {
    return fgets(buffer, size, file_);
  }

  int GetChar() override {
    return fgetc(file_);
  }

  long Tell() override {
    return ftell(file_);
  }

  int Close() override {
    closed_ = true;
    return fclose(file_);
  }

private:
  FILE* file_;
  bool closed_;
};

class FileSystemLibrary : public Library {
public:
  FileSystemLibrary(string const& path) : path_(path) {
    if (path_.back() != '/') {
      path_.push_back('/');
    }
  }

  unique_ptr<LibraryFile> Open(
      char const* filename, char const* mode) const override {
    // TODO: As a general rule, this is a very unsafe way to handle filenames,
    // but as it's supposed to be from a trusted source...
    if (filename[0] == '/') {
      return unique_ptr<LibraryFile>(nullptr);
    }

    return unique_ptr<LibraryFile>(
        new FileSystemLibraryFile(fopen(filename, mode)));
  }

private:
  string path_;

  NO_COPY_OR_ASSIGN(FileSystemLibrary);
};

}

unique_ptr<Library> Library::OfFileSystem(const char* path) {
  return unique_ptr<Library>(new FileSystemLibrary(path));
}



