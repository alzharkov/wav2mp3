#include "encoding_threads_pool.h"

#ifdef USE_POSIX_THREADS
#ifdef __unix
#include <zconf.h>
#else
#include "windows.h"
#endif
#else
#include <thread>
#endif

namespace {

static const uint32_t MAX_THREADS_COUNT = 256;
uint32_t GetProcessorsCount() {
#ifdef USE_POSIX_THREADS
#ifndef __unix
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
#else
  return std::thread::hardware_concurrency();
#endif
}

}

namespace wav2mp3 {

EncodingThreadsPool::EncodingThreadsPool(
    std::shared_ptr<EncodingQueue> encoding_queue,
    uint32_t threads_count)
  :threads_completed_(0) {
  
  uint32_t threads_to_create = 0;
  if (threads_count <= 0)
    threads_to_create = GetProcessorsCount();
  else if (threads_count < MAX_THREADS_COUNT)
    threads_to_create = threads_count;
  else
    threads_to_create = MAX_THREADS_COUNT;

  auto on_stopped_callback = [this]() {
    this->threads_completed_++;
  };
  for (uint32_t i = 0; i < threads_to_create; i++) {
    threads_.push_back(ENCODING_THREAD(encoding_queue, on_stopped_callback));
  }
}

EncodingThreadsPool::~EncodingThreadsPool() {
}

void EncodingThreadsPool::Start() {
  for (ENCODING_THREAD& thread : threads_) {
    thread.Start();
  }
}

void EncodingThreadsPool::Stop() {
  for (ENCODING_THREAD& thread : threads_) {
    thread.Stop();
  }
}

void EncodingThreadsPool::StartAndWait() {
  Start();

  while (threads_.size() > threads_completed_);

  Stop();
}

}  // namespace wav2mp3