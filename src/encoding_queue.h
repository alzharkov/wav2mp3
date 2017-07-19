#ifndef ENCODING_QUEUE_H
#define ENCODING_QUEUE_H

#include <atomic>
#include <memory>
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
  virtual void Add(std::string file_name);

  // Retrives next unhadled item. Thread-safe.
  virtual std::shared_ptr<EncodingItem> GetNextItem();

  EncodingQueue(const EncodingQueue&) = delete;
  EncodingQueue& operator=(const EncodingQueue&) = delete;

 private:
  std::vector<std::shared_ptr<EncodingItem>> items_;
  std::atomic<int> last_retrieved_item_;
};

}  // namespace wav2mp3
#endif // ENCODING_QUEUE_H
