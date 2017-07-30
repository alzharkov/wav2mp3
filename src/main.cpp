#include <algorithm>
#include <iostream>
#include "encoding_item.h"
#include "encoding_queue.h"
#include "encoding_threads_pool.h"

#if !defined(_UNICODE)  // not _UNICODE

#define _tmain main

#else  // _UNICODE

#define _tmain wmain

#endif  // _UNICODE

namespace {
bool is_wav_file(const wav2mp3::FilePath& file_path) {
  if (file_path.size() < 5)
    return false;
  auto ext = file_path.substr(file_path.size() - 3, 3);
  return ext[0] == _T('w') && ext[1] == _T('a') && ext[2] == _T('v');
}

std::shared_ptr<wav2mp3::EncodingQueue> GetEncodingQueue(
    const wav2mp3::FilePath& path) {
  auto queue = std::make_shared<wav2mp3::EncodingQueue>();
  auto files_list = wav2mp3::GetFilesList(path, is_wav_file);
  for (auto file : files_list)
    queue->Add(file);
  return queue;
}

void PrintContinue() {
  std::cout << "Press [Enter] to continue..." << std::endl;
  std::cin.get();
}

void PrintHelp() {
  PrintContinue();
}

}

int _tmain(int argc, CharType **argv) {
  if (argc < 2) {
    std::cout << "Please provide path to wav files." << std::endl;
    PrintHelp();
    return 1;
  }
  wav2mp3::FilePath path = argv[1];
  auto encoding_queue = GetEncodingQueue(path);
  wav2mp3::EncodingThreadsPool(encoding_queue)
    .Wait();

  PrintContinue();
}