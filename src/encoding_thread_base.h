#ifndef ENCODING_THREAD_BASE_H
#define ENCODING_THREAD_BASE_H

#include <atomic>
#include <memory>

#include "encoding_queue.h"

namespace wav2mp3 {

// Implements encoding thread. Receives queue of items to be encoded.
// Starts execution immediately after creation.
class EncodingThreadBase {
 public:
  // Creates new thread, processing starts immediately.
  EncodingThreadBase(std::shared_ptr<EncodingQueue> encoding_queue,
                 std::function<void(void)> on_stopped);

  // Starts thread execution.
  virtual void Start() = 0;
  // Sends stop signal to the main loop. Waits until thread is stopped.
  virtual void Stop() = 0;

  // Sends stop signal.
  virtual ~EncodingThreadBase();

  EncodingThreadBase(const EncodingThreadBase&);

 protected:
  // Implements main thread loop: gets item from queue and encodes it.
  // Stops when there are no unprocessed items or stop signal received.
  // Before finish calls on_stopped_ callback.
  virtual void MainLoop();

 protected:
  std::shared_ptr<EncodingQueue> encoding_queue_;
  std::atomic<bool> enforce_stop_;
  std::function<void(void)> on_stopped_;
};

}  // namespace wav2mp3
#endif //ENCODING_THREAD_BASE_H
