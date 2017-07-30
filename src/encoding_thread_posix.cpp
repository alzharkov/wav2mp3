#include "encoding_thread_posix.h"

namespace wav2mp3 {
EncodingThreadPosix::EncodingThreadPosix(
    std::shared_ptr<EncodingQueue> encoding_queue,
    std::function<void(void)> on_stopped)
  : EncodingThreadBase (encoding_queue, on_stopped){

}

EncodingThreadPosix::EncodingThreadPosix(const EncodingThreadPosix& source)
  : EncodingThreadPosix(source.encoding_queue_, source.on_stopped_){

}

void EncodingThreadPosix::Start() {
  pthread_create(
      &thread_,
      NULL,
      &EncodingThreadPosix::PosixThreadFunction,
      this);
}

void EncodingThreadPosix::Stop() {
  pthread_join(thread_, NULL);
}

EncodingThreadPosix::~EncodingThreadPosix() {
  Stop();
}

void* EncodingThreadPosix::PosixThreadFunction(void* object) {
  auto encoding_thread_posix = reinterpret_cast<EncodingThreadPosix*>(object);
  if (encoding_thread_posix != nullptr)
    encoding_thread_posix->MainLoop();
  pthread_exit(nullptr);
}

}  // namespace wav2mp3
