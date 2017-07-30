all:
	g++ -Wall -g -std=c++11 src/*.cpp -I/usr/local/include/lame -lpthread -lmp3lame -o wav2mp3
