#ifndef __unix

#include "file_helper.h"

#include "windows.h"

namespace wav2mp3 {

std::list<FilePath> GetFilesList(
    const FilePath& path,
    std::function<bool(const FilePath&)> is_appropriate_file_name) {
  std::list<FilePath> files_list;
  FilePath base_path, path_pattern;
  if (path[path.size() - 1] == L'\\') {
    base_path = path;
    path_pattern = path + L"*";
  }
  else {
    base_path = path + L"\\";
    path_pattern = path + L"\\*";
  }
  
  if (path_pattern.size() > MAX_PATH)
    return files_list;

  WIN32_FIND_DATA ffd;
  HANDLE hFind = INVALID_HANDLE_VALUE;

  hFind = FindFirstFile(path_pattern.data(), &ffd);

  if (INVALID_HANDLE_VALUE == hFind)
    return files_list;

  do {
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      continue;
    FilePath file_name(ffd.cFileName);
    if (is_appropriate_file_name(file_name))
      files_list.push_back(base_path + file_name);
  } while (FindNextFile(hFind, &ffd));
  
  FindClose(hFind);
  return files_list;
}

}  // namespace wav2mp3

#endif