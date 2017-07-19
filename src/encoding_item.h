#ifndef ENCODING_ITEM_H
#define ENCODING_ITEM_H

#include <atomic>
#include <string>

namespace wav2mp3 {

// Stores information about file to be encoded.
class EncodingItem {
 public:
  EncodingItem(std::string file_name);
  virtual ~EncodingItem() {}
  // Marks item as encoded.
  void EncodingCompleted();

  EncodingItem(const EncodingItem&) = delete;
  EncodingItem& operator=(const EncodingItem&) = delete;
 private:
  std::string file_name_;
  std::atomic<bool> is_encoded_;
};

}
#endif // ENCODING_ITEM_H
