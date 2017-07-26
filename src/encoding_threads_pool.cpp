#include "encoding_threads_pool.h"

namespace {

static const uint32_t MAX_THREADS_COUNT = 256;

}

namespace wav2mp3 {

EncodingThreadsPool::EncodingThreadsPool(
    std::shared_ptr<EncodingQueue> encoding_queue,
    uint32_t threads_count)
  :threads_completed_(0) {
  
  uint32_t threads_to_create = 0;
  if (threads_count <= 0)
    threads_to_create = std::thread::hardware_concurrency();
  else if (threads_count < MAX_THREADS_COUNT)
    threads_to_create = threads_count;
  else
    threads_to_create = MAX_THREADS_COUNT;

  auto on_stopped_callback = [this]() {
    this->threads_completed_++;
  };
  for (uint32_t i = 0; i < threads_to_create; i++) {
    threads_.push_back(EncodingThread(encoding_queue, on_stopped_callback));
  }

  Start();
}

EncodingThreadsPool::~EncodingThreadsPool() {
  Stop();

  Wait(); 
}

void EncodingThreadsPool::Start() {
  for (EncodingThread& thread : threads_) {
    thread.Start();
  }
}

void EncodingThreadsPool::Stop() {
  for (EncodingThread& thread : threads_) {
    thread.Stop();
  }
}

void EncodingThreadsPool::Wait() {
  while (threads_.size() > threads_completed_);
}

}  // namespace wav2mp3