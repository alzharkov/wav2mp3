#ifndef ENCODING_QUEUE_H
#define ENCODING_QUEUE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "encoding_item.h"

namespace wav2mp3 {

// Implements thread-safe queue of items which describes file to be encoded.
class EncodingQueue {
 public:
  
  EncodingQueue();  
  virtual ~EncodingQueue() {}
  
  // Thread-safe adding.
  virtual void Add(const FilePath& file_name);

  // Retrives next unhadled item. Thread-safe.
  virtual std::shared_ptr<EncodingItem> GetNextItem();

  EncodingQueue(const EncodingQueue&) = delete;
  EncodingQueue& operator=(const EncodingQueue&) = delete;

 private:
  std::vector<std::shared_ptr<EncodingItem>> items_;
  std::mutex mutex_;
  std::atomic<std::size_t> next_retrieved_item_;
};

}  // namespace wav2mp3
#endif // ENCODING_QUEUE_H
