#include <algorithm>
#include <iostream>
#include "encoding_item.h"
#include "encoding_queue.h"
#include "encoding_threads_pool.h"

#if !defined(_UNICODE)  // not _UNICODE
#define _tmain main

#define WAV_EXT_W 'w'
#define WAV_EXT_A 'a'
#define WAV_EXT_V 'v'
#else  // _UNICODE
#define _tmain wmain

#define WAV_EXT_W L'w'
#define WAV_EXT_A L'a'
#define WAV_EXT_V L'v'
#endif  // _UNICODE

namespace {
bool is_wav_file(const wav2mp3::FilePath& file_path) {
  auto ext = file_path.substr(file_path.size() - 3, 3);
  return ext[0] == WAV_EXT_W && ext[1] == WAV_EXT_A && ext[2] == WAV_EXT_V;
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
  wav2mp3::EncodingThreadsPool(GetEncodingQueue(path))
    .Wait();

  PrintContinue();
}