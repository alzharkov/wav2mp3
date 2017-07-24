#include "encoding_item.h"

namespace wav2mp3 {

EncodingItem::EncodingItem(const FilePath& file_name)
  :is_encoded_(false),
  file_name_(file_name){
}

void EncodingItem::EncodingCompleted() {
  is_encoded_ = true;
}


}  // namespace wav2mp3