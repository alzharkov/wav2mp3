#include "encoding_queue.h"

namespace wav2mp3 {

EncodingQueue::EncodingQueue()
  :last_retrieved_item_(-1) {
}

void EncodingQueue::Add(std::string file_name) {
  items_.push_back(std::make_shared<EncodingItem>(file_name));
}

std::shared_ptr<EncodingItem> EncodingQueue::GetNextItem() {
  if (last_retrieved_item_ >= items_.size())
    return nullptr;

  last_retrieved_item_++;
  return items_[last_retrieved_item_];
}

}  // namespace wav2mp3