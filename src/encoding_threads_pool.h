#ifndef ENCODING_THREADS_POOL_H
#define ENCODING_THREADS_POOL_H
#include <memory>
#include <vector>
#include "encoding_queue.h"

#ifdef USE_POSIX_THREADS
#include "encoding_thread_posix.h"
#define ENCODING_THREAD EncodingThreadPosix
#else
#include "encoding_thread.h"
#define ENCODING_THREAD EncodingThread
#endif
namespace wav2mp3 {

// Creates threads pool with automatically started threads.
class EncodingThreadsPool {
 public:
  // Creates specified number of threads.
  // Each thread starts processing right after creation.
  EncodingThreadsPool(
      std::shared_ptr<EncodingQueue> encoding_queue,
      uint32_t threads_count = 0);
  virtual ~EncodingThreadsPool();

  // Use this method to wait until processing will be finished.
  void Wait();
 protected:
  void Start();
  void Stop();
 private:
  std::vector<ENCODING_THREAD> threads_;
  uint32_t threads_completed_;
};

}
#endif  // ENCODING_THREADS_POOL_H