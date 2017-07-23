#include "encoding_threads_pool.h"

namespace {

static const uint32_t MAX_THREADS_COUNT = 256;

}

namespace wav2mp3 {

EncodingThreadsPool::EncodingThreadsPool(
  std::shared_ptr<EncodingQueue> encoding_queue,
  uint32_t threads_count) {
  uint32_t threads_to_create = threads_count > 0
    ? threads_count < MAX_THREADS_COUNT ? threads_count : MAX_THREADS_COUNT
    : std::thread::hardware_concurrency();
  auto on_stopped_callback = [this]() {
    this->threads_completed_++;
  };
  threads_.resize(threads_to_create);
  for (uint32_t i = 0; i < threads_to_create; i++) {
    auto thread = std::unique_ptr<EncodingThread>(
        new EncodingThread(encoding_queue, on_stopped_callback));
    threads_[i] = std::move(thread);
  }
}

EncodingThreadsPool::~EncodingThreadsPool() {
  for (uint32_t i = 0; i < threads_.size(); i++) {
    threads_[i]->Stop();
  }
  Wait(); 
}
void EncodingThreadsPool::Wait() {
  while (threads_.size() > threads_completed_);
}

}  // namespace wav2mp3