#include "encoding_queue.h"

namespace wav2mp3 {

EncodingQueue::EncodingQueue()
  :next_retrieved_item_(0) {
}

void EncodingQueue::Add(const FilePath& file_name) {
  items_.push_back(std::make_shared<EncodingItem>(file_name));
}

std::shared_ptr<EncodingItem> EncodingQueue::GetNextItem() {
  if (next_retrieved_item_ >= items_.size())
    return nullptr;
  auto item = items_[next_retrieved_item_];
  ++next_retrieved_item_;
  return item;
}

}  // namespace wav2mp3