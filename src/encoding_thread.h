#ifndef ENCODING_THREAD_H
#define ENCODING_THREAD_H
#include <thread>

#include "encoding_thread_base.h"

namespace wav2mp3 {

// Implements encoding thread. Receives queue of items to be encoded.
// Starts execution immediately after creation.
class EncodingThread : public EncodingThreadBase {
 public:
  // Creates new thread, processing starts immediately.
  EncodingThread(std::shared_ptr<EncodingQueue> encoding_queue,
                  std::function<void(void)> on_stopped);

  // Starts thread execution.
  virtual void Start() override;
  // Sends stop signal to the main loop. Waits until thread is stopped.
  virtual void Stop() override;

  // Sends stop signal.
  virtual ~EncodingThread();

  EncodingThread(const EncodingThread&);

 private:
  std::thread thread_;
};

}  // namespace wav2mp3
#endif // ENCODING_THREAD_H
