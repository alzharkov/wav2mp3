#include "encoder.h"

#include <fstream>

#include "lame.h"
#include "wav_headers.h"

#if !defined(_UNICODE)  // not _UNICODE
#define EXT_MP3 "mp3"
#else  // _UNICODE
#define EXT_MP3 L"mp3"
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
  ~Mp3Encoder() {
    if (global_flags_)
      delete global_flags_;
  }
  EncodingErrors operator()();
 protected:
  EncodingErrors CheckFormat();
  EncodingErrors ReadHeaders(int &data_size, int &data_offset);
  EncodingErrors ReadPcm(const int data_size, const int data_offset);
  EncodingErrors ReadWaveFile(int &data_size);
  EncodingErrors EncodeToMp3(const int data_size);

 private:
  lame_global_flags* global_flags_;
  FmtHeader fmt_header_;
  uint16_t* left_pcm_;
  uint16_t* right_pcm_;

  FilePath file_path_;
  FilePath output_file_;
  InputFileStream file_;

  int encoding_status_;
};

Mp3Encoder::Mp3Encoder(const wav2mp3::FilePath& file_path)
  : file_path_(file_path),
  fmt_header_({ 0 }),
  left_pcm_(nullptr),
  right_pcm_(nullptr),
  global_flags_(nullptr) {
  output_file_ = file_path.substr(0, file_path.size() - 3) + EXT_MP3;
  
}

EncodingErrors Mp3Encoder::operator()() {
  global_flags_ = lame_init();
  lame_set_brate(global_flags_, 192); // increase bitrate
  lame_set_quality(global_flags_, 3); // increase quality level
  lame_set_bWriteVbrTag(global_flags_, 0);

  int data_size = -1;
  auto result = ReadWaveFile(data_size);

  if (result != EncodingErrors::kOk) {
    lame_close(global_flags_);
    return result;
  }

  lame_set_num_channels(global_flags_, fmt_header_.num_channels);
  lame_set_num_samples(global_flags_, data_size / fmt_header_.block_align);
  // check params
  auto lame_result = lame_init_params(global_flags_);
  if (lame_result != 0) {
    lame_close(global_flags_);
    return result;
  }

  // encode to mp3
  result = EncodeToMp3(data_size);
  
  lame_close(global_flags_);
  if (left_pcm_ != NULL) delete[] left_pcm_;
  if (right_pcm_ != NULL) delete[] right_pcm_;

  return result;
}


EncodingErrors Mp3Encoder::EncodeToMp3(const int data_size) {
  int numSamples = data_size / fmt_header_.block_align;

  int mp3BufferSize = numSamples * 5 / 4 + 7200; // worst case estimate
  unsigned char *mp3Buffer = new unsigned char[mp3BufferSize];

  // call to lame_encode_buffer
  int mp3size = lame_encode_buffer(
      global_flags_, 
      reinterpret_cast<int16_t*>(left_pcm_),
      reinterpret_cast<int16_t*>(right_pcm_),
      numSamples,
      mp3Buffer,
      mp3BufferSize);

  if (!(mp3size > 0)) {
    delete[] mp3Buffer;
    return EncodingErrors::kLameEncodingError;
  }
  /*
  // write to file
  std::ofstream mp3_file(output_file_, std::ios::out | std::ios::binary);
  mp3_file.write(mp3Buffer, mp3size);
  int flushSize = lame_encode_flush(global_flags_, mp3Buffer, mp3BufferSize);

  mp3_file.write(mp3Buffer, mp3size);

  FILE *out = fopen(output_file_, "wb+");
  fwrite((void*)mp3Buffer, sizeof(unsigned char), mp3size, out);

  // call to lame_encode_flush
  int flushSize = lame_encode_flush(global_flags_, mp3Buffer, mp3BufferSize);

  // write flushed buffers to file
  fwrite((void*)mp3Buffer, sizeof(unsigned char), flushSize, out);

  mp3_file.
  // call to lame_mp3_tags_fid (might be omitted)
  lame_mp3_tags_fid(global_flags_, out);

  fclose(out);
  */
  delete[] mp3Buffer;
}

EncodingErrors Mp3Encoder::CheckFormat() {
  if (fmt_header_.audio_format != 0x01)
    return EncodingErrors::kFmtNonPCMFormat;

  if (fmt_header_.num_channels != 1 && fmt_header_.num_channels != 2)
    return EncodingErrors::kFmtWrongChannelsNumber;

  if (fmt_header_.sub_chunk1_size != 16)
    return EncodingErrors::kFmtWrongChunkSize;

  if (fmt_header_.block_align !=
    fmt_header_.bits_per_sample * fmt_header_.num_channels / 8)
    return EncodingErrors::kFmtWrongBlockAlign;

  return EncodingErrors::kOk;
}

EncodingErrors Mp3Encoder::ReadHeaders(int &data_size, int &data_offset) {
  if (!file_.is_open())
    return EncodingErrors::kFileReadError;
  
  file_.seekg(0, std::ios::beg);
  
  // Read RIFF header.
  RiffHeader riff_header = { 0 };
  file_.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
  
  // Validate RIFF header.
  if (!(strncmp(riff_header.chunck_id, "RIFF", 4) == 0
      && strncmp(riff_header.format, "WAVE", 4) == 0
      && riff_header.chunck_sie > 0))
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
      found_format_header = true;
      break;
    }
    // Skip current chunk.
    file_.seekg(chunk_header.sub_chunk2_size, std::ios::cur);
  }

  if (!found_format_header)
    return EncodingErrors::KFmtHeaderNotFound;
  
  // Check fmt header.
  auto result = CheckFormat();
  if (result != EncodingErrors::kOk)
    return result;
  
  bool found_data_header = false;
  while (!found_data_header && !file_.eof()) {    
    file_.read(reinterpret_cast<char*>(&chunk_header), sizeof(DataHeader));

    if (strncmp(chunk_header.sub_chunk2_id, "data", 4) == 0) {
      found_data_header = true;
      data_size = chunk_header.sub_chunk2_size;
      data_offset = file_.tellg();
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

  left_pcm_ = nullptr;
  right_pcm_ = nullptr;

  // allocate PCM arrays
  int chunks_count = data_size / fmt_header_.num_channels / sizeof(uint16_t);
  left_pcm_ = new uint16_t[chunks_count];
  if (fmt_header_.num_channels > 1)
    right_pcm_ = new uint16_t[chunks_count];

  // capture each sample
  file_.seekg(data_offset);// set file pointer to beginning of data array

  if (fmt_header_.num_channels == 1) {
    file_.read(reinterpret_cast<char*>(left_pcm_),
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
  return EncodingErrors::kOk;
}

}  // namespace encoder
}  // namespace wav2mp3