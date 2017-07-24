#include "encoder.h"

#include "lame.h"


#if !defined(_UNICODE)  // not _UNICODE
#define EXT_MP3 "mp3"
#else  // _UNICODE
#define EXT_MP3 L"mp3"
#endif  // _UNICODE

namespace {
using wav2mp3::FilePath;
class Mp3Encoder {
 public:
  Mp3Encoder(const wav2mp3::FilePath& file_path);
  ~Mp3Encoder() {
    if (global_flags_)
      delete global_flags_;
  }
 private:
  lame_global_flags* global_flags_;
  FilePath file_path_;
  FilePath output_file_;
};

Mp3Encoder::Mp3Encoder(const wav2mp3::FilePath& file_path)
  : file_path_(file_path) {
  output_file_ = file_path.substr(0, file_path.size() - 3) + EXT_MP3;


  global_flags_ = lame_init();
  lame_set_brate(global_flags_, 192); // increase bitrate
  lame_set_quality(global_flags_, 3); // increase quality level
  lame_set_bWriteVbrTag(global_flags_, 0);
  
}
}
namespace wav2mp3 {
namespace encoder {

void Encode(std::shared_ptr<EncodingItem> encoding_item) {

}

}  // namespace encoder
}  // namespace wav2mp3