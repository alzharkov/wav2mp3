#include "encoding_thread.h"

#include <thread>
#include "encoder.h"

namespace wav2mp3 {

EncodingThread::
EncodingThread(std::shared_ptr<EncodingQueue> encoding_queue,
               std::function<void(void)> on_stopped)
  : encoding_queue_(encoding_queue),
  enforce_stop_(false),
  on_stopped_(on_stopped) {
  thread_ = std::thread(std::bind(&EncodingThread::MainLoop, this));
}

EncodingThread::~EncodingThread() {
  Stop();
}

void EncodingThread::Stop() {
  enforce_stop_ = true;
  thread_.join();
}

void EncodingThread::MainLoop() {
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