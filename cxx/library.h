#ifndef CXX_LIBRARY_H
#define CXX_LIBRARY_H

#include <memory>

class LibraryFile {
public:
  virtual ~LibraryFile() = default;
  virtual int Read(void* buf, int len, int size) = 0;
  virtual int Seek(long, int) = 0;
  virtual char* GetString(char* buffer, int size) = 0;
  virtual int GetChar() = 0;
  virtual long Tell() = 0;
  virtual int Close() = 0;
};

class Library {
public:
  static std::unique_ptr<Library> OfFileSystem(char const* path);

  virtual ~Library() = default;
  virtual std::unique_ptr<LibraryFile> Open(
      char const* filename, char const* mode) const = 0;
};

#endif
