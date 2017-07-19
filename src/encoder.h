#ifndef ENCODER_H
#define ENCODER_H

#include <memory>
#include "encoding_item.h"

namespace wav2mp3 {
namespace encoder {

void Encode(std::shared_ptr<EncodingItem> encoding_item);

}  // namespace encoder
}  // namespace wav2mp3

#endif  // ENCODER_H
