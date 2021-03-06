#ifndef ENCODER_H
#define ENCODER_H

#include <memory>

namespace wav2mp3 {

enum EncodingErrors {
  kOk = 0,
  kInvalidLameEncodingParameters,
  kLameEncodingError,
  kWrongRiffHeader,
  KFmtHeaderNotFound,
  kFmtNonPCMFormat,
  kFmtWrongChannelsNumber,
  kFmtWrongChunkSize,
  kFmtWrongBlockAlign,
  kDataHeaderNorFound,
  kWrongDataSize,
  kFileReadError,
  kInvalidParameters,
  kOutputFileOpenError,
  kUnknownError = 9999
};

class EncodingItem;

namespace encoder {

EncodingErrors Encode(std::shared_ptr<EncodingItem> encoding_item);

}  // namespace encoder

}  // namespace wav2mp3

#endif  // ENCODER_H
