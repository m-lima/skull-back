#pragma once

#include "user.hpp"

struct UserIterator {
  static void forEach(const std::function<void(const User &)> & executor);
};

template <typename T>
struct FileHandle {
  const std::string path;
  T file;

  FileHandle(const std::string & user, const char * const fileName);
  virtual ~FileHandle();

  FileHandle(const FileHandle &) = delete;
  FileHandle & operator=(const FileHandle &) = delete;

  inline bool good() const {
    return file.good();
  }
};



