#include "encoding_thread_base.h"

#include "encoder.h"

namespace wav2mp3 {

EncodingThreadBase::EncodingThreadBase(const EncodingThreadBase& source)
    : EncodingThreadBase(source.encoding_queue_, source.on_stopped_) {
}

EncodingThreadBase::EncodingThreadBase(std::shared_ptr<EncodingQueue> encoding_queue,
                               std::function<void(void)> on_stopped)
    : encoding_queue_(encoding_queue),
      enforce_stop_(false),
      on_stopped_(on_stopped) {
}

EncodingThreadBase::~EncodingThreadBase() {

}

void EncodingThreadBase::MainLoop() {
  if (encoding_queue_ == nullptr) {
    return;
  }
  std::shared_ptr<EncodingItem> item = encoding_queue_->GetNextItem();
  while (!enforce_stop_ && item != nullptr) {
    encoder::Encode(item);
    item = encoding_queue_->GetNextItem();
  }
  if (on_stopped_ != nullptr)
    on_stopped_();
}

}  // namespace wav2mp3
