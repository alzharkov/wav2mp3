# wav2mp3
Simple solution for batch wav to mp3 conversion using lame library
Supports only 16-bit PCM format (AudioCD)

# Build
For Windows build it is expected that lame folder is placed in same level as wav2mp3 solution folder

For Linux build there are make (Makefile) and CMAKE (CMakeLists.txt) files

# Samples

Folder test_files contains wav files with different sampling rates (source http://www.mauvecloud.net/sounds/index.html)
Current implementation supports only pcm16*.wav files
