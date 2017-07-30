cmake_minimum_required(VERSION 3.8)
project(wav2mp3)

set(CMAKE_CXX_STANDARD 11)

include_directories(BEFORE /usr/local/include/lame)
link_directories(/usr/local/lib/)

set(SOURCE_FILES
        src/encoder.cpp
        src/encoder.h
        src/encoding_item.cpp
        src/encoding_item.h
        src/encoding_queue.cpp
        src/encoding_queue.h
        src/encoding_thread.cpp
        src/encoding_thread.h
        src/encoding_threads_pool.cpp
        src/encoding_threads_pool.h
        src/file_helper.cpp
        src/file_helper.h
        src/file_helper_win.cpp
        src/main.cpp
        src/string_type.h
        src/wav_headers.h
        tests/main.cpp
        Makefile)

add_executable(wav2mp3 ${SOURCE_FILES})