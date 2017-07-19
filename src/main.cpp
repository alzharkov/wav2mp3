#include <iostream>
#include "encoding_queue.h"
#include "encoding_threads_pool.h"
namespace {
std::shared_ptr<wav2mp3::EncodingQueue> GetEncodingQueue(std::string path) {
  auto queue = std::make_shared<wav2mp3::EncodingQueue>();
  return queue;
}

void PrintHelp() {
  std::cout << "Press [Enter] to continue..." << std::endl;
  std::cin.get();
}

}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Please provide path to wav files." << std::endl;
    PrintHelp();
    return 1;
  }
  std::string path = argv[1];
  auto encoding_queue = GetEncodingQueue(path);
  wav2mp3::EncodingThreadsPool(GetEncodingQueue(path))
    .Wait();
}