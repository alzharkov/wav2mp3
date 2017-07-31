#include "encoding_thread.h"

namespace wav2mp3 {


EncodingThread::EncodingThread(const EncodingThread& source)
    : EncodingThread(source.encoding_queue_,source.on_stopped_) {
}

EncodingThread::EncodingThread(std::shared_ptr<EncodingQueue> encoding_queue,
                               std::function<void(void)> on_stopped)
  : EncodingThreadBase(encoding_queue, on_stopped) {
}

EncodingThread::~EncodingThread() {
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

}  // namespace wav2mp3