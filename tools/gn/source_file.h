// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_GN_SOURCE_FILE_H_
#define TOOLS_GN_SOURCE_FILE_H_

#include <stddef.h>

#include <algorithm>
#include <string>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"

class SourceDir;

// Represents a file within the source tree. Always begins in a slash, never
// ends in one.
class SourceFile {
 public:
  // This should be sequential integers starting from 0 so they can be used as
  // array indices.
  enum Type {
    SOURCE_UNKNOWN = 0,
    SOURCE_ASM,
    SOURCE_C,
    SOURCE_CPP,
    SOURCE_H,
    SOURCE_M,
    SOURCE_MM,
    SOURCE_S,
    SOURCE_RC,
    SOURCE_O,  // Object files can be inputs, too. Also counts .obj.
    SOURCE_DEF,

    SOURCE_RS,
    SOURCE_GO,

    // Must be last.
    SOURCE_NUMTYPES,
  };

  enum SwapIn { SWAP_IN };

  SourceFile();

  // Takes a known absolute source file. Always begins in a slash.
  explicit SourceFile(const base::StringPiece& p);
  SourceFile(const SourceFile& other) = default;

  // Constructs from the given string by swapping in the contents of the given
  // value. The value will be the empty string after this call.
  SourceFile(SwapIn, std::string* value);

  ~SourceFile();

  bool is_null() const { return value_.empty(); }
  const std::string& value() const { return value_; }
  Type type() const { return type_; }

  // Returns everything after the last slash.
  std::string GetName() const;
  SourceDir GetDir() const;

  // Resolves this source file relative to some given source root. Returns
  // an empty file path on error.
  base::FilePath Resolve(const base::FilePath& source_root) const;

  // Returns true if this file starts with a "//" which indicates a path
  // from the source root.
  bool is_source_absolute() const {
    return value_.size() >= 2 && value_[0] == '/' && value_[1] == '/';
  }

  // Returns true if this file starts with a single slash which indicates a
  // system-absolute path.
  bool is_system_absolute() const { return !is_source_absolute(); }

  // Returns a source-absolute path starting with only one slash at the
  // beginning (normally source-absolute paths start with two slashes to mark
  // them as such). This is normally used when concatenating names together.
  //
  // This function asserts that the file is actually source-absolute. The
  // return value points into our buffer.
  base::StringPiece SourceAbsoluteWithOneSlash() const {
    CHECK(is_source_absolute());
    return base::StringPiece(&value_[1], value_.size() - 1);
  }

  bool operator==(const SourceFile& other) const {
    return value_ == other.value_;
  }
  bool operator!=(const SourceFile& other) const { return !operator==(other); }
  bool operator<(const SourceFile& other) const {
    return value_ < other.value_;
  }

  void swap(SourceFile& other) {
    value_.swap(other.value_);
    std::swap(type_, other.type_);
  }

 private:
  friend class SourceDir;

  void SetValue(const std::string& value);

  std::string value_;
  Type type_;

  // Copy & assign supported.
};

namespace std {

template <>
struct hash<SourceFile> {
  std::size_t operator()(const SourceFile& v) const {
    hash<std::string> h;
    return h(v.value());
  }
};

}  // namespace std

inline void swap(SourceFile& lhs, SourceFile& rhs) {
  lhs.swap(rhs);
}

#endif  // TOOLS_GN_SOURCE_FILE_H_
