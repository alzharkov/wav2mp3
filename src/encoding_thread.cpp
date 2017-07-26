#include "encoding_thread.h"

#include <thread>
#include "encoder.h"

namespace wav2mp3 {


EncodingThread::EncodingThread(const EncodingThread& source)
    : encoding_queue_(source.encoding_queue_),
    enforce_stop_(static_cast<bool>(source.enforce_stop_)),
    on_stopped_(source.on_stopped_) {
}

EncodingThread::EncodingThread(std::shared_ptr<EncodingQueue> encoding_queue,
                               std::function<void(void)> on_stopped)
  : encoding_queue_(encoding_queue),
  enforce_stop_(false),
  on_stopped_(on_stopped) {  
}

EncodingThread::~EncodingThread() {
  Stop();
}

void EncodingThread::Start() {
  Stop();
  enforce_stop_ = false;
  thread_ = std::thread(std::bind(&EncodingThread::MainLoop, this));
}
void EncodingThread::Stop() {
  enforce_stop_ = true;
  if (thread_.joinable())
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