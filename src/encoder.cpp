#include "encoder.h"

#include <fstream>
#include <string.h>
#include <vector>

#include "lame.h"

#include "encoding_item.h"
#include "wav_headers.h"

#ifdef __unix
int fopen_s(FILE** f, const char* name, const char* mode) {
  *f = fopen(name, mode);
  if (!*f)
    return errno;
  return 0;
}
#endif

#if !defined(_UNICODE)  // not _UNICODE
#define FOPEN fopen_s
#else  // _UNICODE
#define FOPEN _wfopen_s
#endif  // _UNICODE

namespace {
using wav2mp3::FilePath;
using wav2mp3::EncodingErrors;
using wav2mp3::FmtHeader;
using wav2mp3::RiffHeader;
using wav2mp3::DataHeader;

typedef std::ifstream InputFileStream;

class Mp3Encoder {
 public:
  Mp3Encoder(const wav2mp3::FilePath& file_path);
  ~Mp3Encoder() {}

  EncodingErrors operator()();

  Mp3Encoder(const Mp3Encoder&) = delete;
  Mp3Encoder& operator=(const Mp3Encoder&) = delete;
 protected:
  EncodingErrors CheckFormat();
  EncodingErrors ReadHeaders(int &data_size, int &data_offset);
  EncodingErrors ReadPcm(const int data_size, const int data_offset);
  EncodingErrors ReadWaveFile(int &data_size);
  EncodingErrors EncodeToMp3(lame_global_flags* global_flags, const int data_size);

 private:
  FmtHeader fmt_header_;
  std::vector<uint16_t> left_pcm_;
  std::vector<uint16_t> right_pcm_;

  FilePath file_path_;
  FilePath output_file_;
  InputFileStream file_;
};

Mp3Encoder::Mp3Encoder(const wav2mp3::FilePath& file_path)
  : file_path_(file_path),
  fmt_header_({ 0 }) {
  output_file_ = file_path.substr(0, file_path.size() - 3) + _T("mp3");  
}

EncodingErrors Mp3Encoder::operator()() {
  lame_global_flags* global_flags = lame_init();
  lame_set_brate(global_flags, 128);  // Set bit rate.
  lame_set_quality(global_flags, 5);  // Set 'good' quality.
  lame_set_bWriteVbrTag(global_flags, 0);

  int data_size = -1;
  auto result = ReadWaveFile(data_size);

  if (result != EncodingErrors::kOk) {
    lame_close(global_flags);
    return result;
  }

  lame_set_num_channels(global_flags, fmt_header_.num_channels);
  lame_set_num_samples(global_flags, data_size / fmt_header_.block_align);
  // Check params by lame.
  auto lame_result = lame_init_params(global_flags);
  if (lame_result) {
    lame_close(global_flags);
    return result;
  }

  // Encode to mp3.
  result = EncodeToMp3(global_flags, data_size);
  
  lame_close(global_flags);

  return result;
}


EncodingErrors Mp3Encoder::EncodeToMp3(lame_global_flags* global_flags, int data_size) {
  int num_samples = data_size / fmt_header_.block_align;

  int mp3_buffer_size = num_samples * 5 / 4 + 7200;
  std::vector<uint8_t> mp3_buffer;
  mp3_buffer.resize(mp3_buffer_size);

  int16_t* left_pcm = left_pcm_.size() 
      ? reinterpret_cast<int16_t*>(left_pcm_.data()) : nullptr;
  int16_t* right_pcm = right_pcm_.size()
      ? reinterpret_cast<int16_t*>(right_pcm_.data()) : nullptr;

  int mp3_size = lame_encode_buffer(
      global_flags, 
      left_pcm,
      right_pcm,
      num_samples,
      mp3_buffer.data(),
      mp3_buffer_size);

  if (mp3_size <= 0)
    return EncodingErrors::kLameEncodingError;
  
  FILE *mp3_file_handler;
  auto error_code = FOPEN(&mp3_file_handler, output_file_.c_str(), _T("wb+"));
  if (error_code)
    return EncodingErrors::kOutputFileOpenError;

  fwrite(reinterpret_cast<void*>(mp3_buffer.data()),
         sizeof(uint8_t),
         mp3_size,
         mp3_file_handler);
  fflush(mp3_file_handler);
  int flushSize = lame_encode_flush(global_flags,
                                    mp3_buffer.data(),
                                    mp3_buffer_size);

  fwrite(reinterpret_cast<void*>(mp3_buffer.data()),
         sizeof(uint8_t),
         flushSize,
         mp3_file_handler);

  fflush(mp3_file_handler);
  lame_mp3_tags_fid(global_flags, mp3_file_handler);

  fflush(mp3_file_handler);
  fclose(mp3_file_handler);
  return EncodingErrors::kOk;
}

EncodingErrors Mp3Encoder::CheckFormat() {
  if (fmt_header_.audio_format != 0x01)
    return EncodingErrors::kFmtNonPCMFormat;
  
  if (fmt_header_.num_channels != 1 && fmt_header_.num_channels != 2)
    return EncodingErrors::kFmtWrongChannelsNumber;

  if (fmt_header_.sub_chunk1_size != 16)
    return EncodingErrors::kFmtWrongChunkSize;

  if (fmt_header_.block_align == 0)
    return EncodingErrors::kFmtWrongBlockAlign;

  if (fmt_header_.block_align !=
    fmt_header_.bits_per_sample * fmt_header_.num_channels / 8)
    return EncodingErrors::kFmtWrongBlockAlign;

  return EncodingErrors::kOk;
}

EncodingErrors Mp3Encoder::ReadHeaders(int &data_size, int &data_offset) {
  if (!file_.is_open())
    return EncodingErrors::kFileReadError;
  file_.seekg(0, std::ios::end);
  auto file_size = static_cast<int>(file_.tellg());
  if (file_size < sizeof(RiffHeader) + sizeof(FmtHeader))
    return EncodingErrors::kWrongRiffHeader;

  file_.seekg(0, std::ios::beg);
  
  // Read RIFF header.
  RiffHeader riff_header = { 0 };
  file_.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
  
  // Validate RIFF header.
  if (!(strncmp(riff_header.chunck_id, "RIFF", 4) == 0
      && strncmp(riff_header.format, "WAVE", 4) == 0
      && riff_header.chunck_sie + 8 == file_size))
    return EncodingErrors::kWrongRiffHeader;
  
  DataHeader chunk_header;
  // Find chunck with type 'fmt '.
  bool found_format_header = false;
  while (!found_format_header && !file_.eof()) {
    file_.read(reinterpret_cast<char*>(&chunk_header), sizeof(DataHeader));
    if (strncmp(chunk_header.sub_chunk2_id, "fmt ", 4) == 0) {
      // Move file pointer back and read fmt header.
      file_.seekg((int)file_.tellg() - sizeof(DataHeader));

      file_.read(reinterpret_cast<char*>(&fmt_header_), sizeof(FmtHeader));

      // Check fmt header.
      auto result = CheckFormat();
      if (result != EncodingErrors::kOk)
        return result;

      found_format_header = true;
      break;
    }
    // Skip current chunk.
    file_.seekg(chunk_header.sub_chunk2_size, std::ios::cur);
  }

  if (!found_format_header)
    return EncodingErrors::KFmtHeaderNotFound;
  
  bool found_data_header = false;
  while (!found_data_header && !file_.eof()) {    
    file_.read(reinterpret_cast<char*>(&chunk_header), sizeof(DataHeader));

    if (strncmp(chunk_header.sub_chunk2_id, "data", 4) == 0) {
      found_data_header = true;
      data_size = chunk_header.sub_chunk2_size;
      if (data_size > file_size)
        return EncodingErrors::kWrongDataSize;
      data_offset = static_cast<int>(file_.tellg());
      break;
    }
    else {
      file_.seekg(chunk_header.sub_chunk2_size, std::ios::cur);
    }
  }

  if (!found_data_header)
    return EncodingErrors::kDataHeaderNorFound;

  return EncodingErrors::kOk;
}

EncodingErrors Mp3Encoder::ReadPcm(const int data_size, const int data_offset) {
  int idx;
  int numSamples = data_size / fmt_header_.block_align;

  left_pcm_.resize(0);
  right_pcm_.resize(0);

  // allocate PCM arrays
  int chunks_count = data_size / fmt_header_.num_channels / sizeof(uint16_t);
  left_pcm_.resize(chunks_count);
  if (fmt_header_.num_channels > 1)
    right_pcm_.resize(chunks_count);

  // capture each sample
  file_.seekg(data_offset);// set file pointer to beginning of data array

  if (fmt_header_.num_channels == 1) {
    file_.read(reinterpret_cast<char*>(left_pcm_.data()),
               fmt_header_.block_align * numSamples);
  }
  else {
    for (idx = 0; idx < numSamples; idx++) {
      file_.read(reinterpret_cast<char*>(&left_pcm_[idx]),
                 fmt_header_.block_align / fmt_header_.num_channels);

      if (fmt_header_.num_channels > 1)
        file_.read(reinterpret_cast<char*>(&right_pcm_[idx]),
                   fmt_header_.block_align / fmt_header_.num_channels);
    }
  }
  return EncodingErrors::kOk;
}

EncodingErrors Mp3Encoder::ReadWaveFile(int &data_size) {
  file_ = InputFileStream(file_path_, std::ios::in | std::ios::binary);

  if (!file_.is_open())
    return EncodingErrors::kFileReadError;

  data_size = 0;
  int data_offset = 0;
  
  auto result = ReadHeaders(data_size, data_offset);
  
  if (result == EncodingErrors::kOk)
    result = ReadPcm(data_size, data_offset);
  
  file_.close();  
  return result;
}

}
namespace wav2mp3 {
namespace encoder {

EncodingErrors Encode(std::shared_ptr<EncodingItem> encoding_item) {
  if (encoding_item == nullptr)
    return EncodingErrors::kInvalidParameters;

  return encoding_item->EncodingCompleted(
      Mp3Encoder(encoding_item->FileName())());
}

}  // namespace encoder
}  // namespace wav2mp3