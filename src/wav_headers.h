#ifndef WAV_HEADERS_H
#define WAV_HEADERS_H
namespace wav2mp3 {

// WAV file headers
// http://soundfile.sapp.org/doc/WaveFormat/

// RIFF chunk descriptor.
struct RiffHeader {
  char chunck_id[4];   // "RIFF"
  uint32_t chunck_sie; // The size of the entire file in bytes
                       // minus 8 bytes for the two fields 
                       // not included in this count:
                       // ChunkID and ChunkSize.
  char format[4];      // "WAVE"
};

struct FmtHeader {
  char sub_chunck1_id[4];   // "fmt "
  uint32_t sub_chunk1_size; // 16 for PCM
  uint16_t audio_format;    // 0x01 for PCM
  uint16_t num_channels;    // 1 mono, 2 stereo
  uint32_t sample_rate;     // 8000, 44100, etc.
  uint32_t byte_rate;       // == SampleRate * NumChannels * BitsPerSample/8
  uint16_t block_align;     // == NumChannels * BitsPerSample/8
  uint16_t bits_per_sample; // 8 bits = 8, 16 bits = 16, etc.
};

struct DataHeader {
  char sub_chunk2_id[4];    // "data"
  uint32_t sub_chunk2_size; // == NumSamples * NumChannels * BitsPerSample/8
                            // This is the number of bytes in the data.
};

}
#endif  // WAV_HEADERS_H