#ifndef ENCODING_ITEM_H
#define ENCODING_ITEM_H

#include <atomic>
#include <string>

#include "encoder.h"
#include "string_type.h"
#include "file_helper.h"

namespace wav2mp3 {

// Stores information about file to be encoded.
class EncodingItem {
 public:
  EncodingItem(const FilePath& file_name);
  virtual ~EncodingItem() {}
  // Marks item as encoded.
  EncodingErrors EncodingCompleted(const EncodingErrors error_code);
  const FilePath& FileName() const;
  bool IsEncoded() const;
  bool HasErrors() const;
  EncodingErrors GetErrorCode() const;

  EncodingItem(const EncodingItem&) = delete;
  EncodingItem& operator=(const EncodingItem&) = delete;
 private:
  FilePath file_name_;
  std::atomic<bool> is_encoded_;
  std::atomic<EncodingErrors> encoding_error_code_;
};

}
#endif // ENCODING_ITEM_H
