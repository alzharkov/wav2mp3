#ifdef __unix
#include "file_helper.h"

#include "dirent.h"

namespace wav2mp3 {

std::list<FilePath> GetFilesList(
    const FilePath& path,
    std::function<bool(const FilePath&)> is_appropriate_file_name) {
  
  DIR* directory = opendir(path.c_str());

  if (directory == nullptr)
    return std::list<FilePath>();

  dirent* entry = nullptr;
  std::list<FilePath> files_list;

  while ((entry = readdir(directory)) != nullptr) {
    FilePath file_path(path);
    if (file_path[file_path.size()-1] != _T('/'))
      file_path += _T("/");
    file_path += entry->d_name;
    if (is_appropriate_file_name(file_path))
      files_list.push_back(file_path);
  }
  closedir(directory);

  return files_list;
}

}  // namespace wav2mp3
#endif  // __unix