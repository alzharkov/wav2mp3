all:
	g++ -Wall -g -std=c++11 -DUSE_POSIX_THREADS src/*.cpp -I/usr/local/include/lame -lpthread -lmp3lame -o wav2mp3
