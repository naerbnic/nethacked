/*
 * library.cc
 *
 *  Created on: Apr 30, 2014
 *      Author: brianchin
 */

#include <memory>
#include <string>

#include "cxxutil.h"
#include "library.h"

using std::string;
using std::unique_ptr;

namespace {

class FileSystemLibrary : public Library {
public:
  FileSystemLibrary(string const& path) : path_(path) {}

  unique_ptr<LibraryFile> Open(char const* filename, char const* mode) {
    return unique_ptr<LibraryFile>(nullptr);
  }

private:
  string path_;

  NO_COPY_OR_ASSIGN(FileSystemLibrary);
};

}

unique_ptr<Library> Library::OfFileSystem(const char* path) {
  return unique_ptr<Library>(new FileSystemLibrary(path));
}



