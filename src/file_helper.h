#ifndef FILE_HELPER_H
#define FILE_HELPER_H
#include <functional>
#include <list>
#include "string_type.h"
namespace wav2mp3 {

typedef StringType FilePath;

std::list<FilePath> GetFilesList(
    const FilePath& path,
    std::function<bool(const FilePath&)> is_appropriate_file_name);

}  // namespace wav2mp3
#endif  // FILE_HELPER_H