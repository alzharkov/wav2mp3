#include "encoding_thread_posix.h"

namespace wav2mp3 {
EncodingThreadPosix::EncodingThreadPosix(
    std::shared_ptr<EncodingQueue> encoding_queue,
    std::function<void(void)> on_stopped)
  : is_started_(false), EncodingThreadBase (encoding_queue, on_stopped){
  
}

EncodingThreadPosix::EncodingThreadPosix(const EncodingThreadPosix& source)
  : EncodingThreadPosix(source.encoding_queue_, source.on_stopped_) {

}

void EncodingThreadPosix::Start() {
  std::lock_guard<std::mutex> lock(mutex_);
  pthread_create(
      &thread_,
      nullptr,
      &EncodingThreadPosix::PosixThreadFunction,
      this);
  is_started_ = true;
}

void EncodingThreadPosix::Stop() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!is_started_)
    return;

  void* return_value;
  pthread_join(thread_, &return_value);
}

EncodingThreadPosix::~EncodingThreadPosix() {
  Stop();
}

void* EncodingThreadPosix::PosixThreadFunction(void* object) {
  auto encoding_thread_posix = reinterpret_cast<EncodingThreadPosix*>(object);
  if (encoding_thread_posix != nullptr)
    encoding_thread_posix->MainLoop();
  pthread_exit(nullptr);
  return nullptr;
}

}  // namespace wav2mp3
