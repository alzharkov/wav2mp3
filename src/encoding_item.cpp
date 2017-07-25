#include "encoding_item.h"

namespace wav2mp3 {

EncodingItem::EncodingItem(const FilePath& file_name)
  :is_encoded_(false),
  file_name_(file_name){
}

EncodingErrors EncodingItem::EncodingCompleted(const EncodingErrors error_code) {
  is_encoded_ = true;
  encoding_error_code_ = error_code;
  return encoding_error_code_;
}

const FilePath& EncodingItem::FileName() const {
  return file_name_;
}

bool EncodingItem::IsEncoded() const {
  return is_encoded_;
}
bool EncodingItem::HasErrors() const {
  return encoding_error_code_ != EncodingErrors::kOk;
}
EncodingErrors EncodingItem::GetErrorCode() const {
  return encoding_error_code_;
}


}  // namespace wav2mp3