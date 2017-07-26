#ifndef ENCODING_THREAD_H
#define ENCODING_THREAD_H
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "encoding_queue.h"

namespace wav2mp3 {

// Implements encoding thread. Receives queue of items to be encoded.
// Starts execution immideatedly after creation.
class EncodingThread {
 public:
  // Creates new thread, processing starts immideatly.
  EncodingThread(std::shared_ptr<EncodingQueue> encoding_queue,
                  std::function<void(void)> on_stopped);

  // Starts thread execution.
  virtual void Start();
  // Sends stop signal to the main loop. Waits until thread is stopped.
  virtual void Stop();

  // Sends stop signal.
  virtual ~EncodingThread();

  EncodingThread(const EncodingThread&);

 protected:
  // Implements main thread loop: gets item from queue and encodes it.
  // Stops when there are no unprocessed items or stop signal received.
  // Before finish calls on_stopped_ callback.
  virtual void MainLoop();

 private:
  std::shared_ptr<EncodingQueue> encoding_queue_;
  std::atomic<bool> enforce_stop_;
  std::thread thread_;
  std::function<void(void)> on_stopped_;
};

}  // namespace wav2mp3
#endif // ENCODING_THREAD_H
