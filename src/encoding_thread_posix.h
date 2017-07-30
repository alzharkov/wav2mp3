#ifndef ENCODING_THREAD_POSIX_H
#define ENCODING_THREAD_POSIX_H

#include <mutex>
#include "pthread.h"

#include "encoding_thread_base.h"
namespace wav2mp3 {

// Implements encoding thread. Receives queue of items to be encoded.
// To start execution call Start method.
// Uses POSIX threads
class EncodingThreadPosix : public EncodingThreadBase {
 public:
  // Creates new thread, processing starts immediately.
  EncodingThreadPosix(std::shared_ptr<EncodingQueue> encoding_queue,
                 std::function<void(void)> on_stopped);

  // Starts thread execution.
  virtual void Start() override;
  // Sends stop signal to the main loop. Waits until thread is stopped.
  virtual void Stop() override;

  // Sends stop signal.
  virtual ~EncodingThreadPosix();

  EncodingThreadPosix(const EncodingThreadPosix&);
 protected:
  static void* PosixThreadFunction(void* object);
 private:
  pthread_t thread_;
  std::mutex mutex_;
  bool is_started_;
};

}  // namespace wav2mp3
#endif //ENCODING_THREAD_POSIX_H
