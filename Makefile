test: *.cpp
	g++ -g -O0 -o test *.cpp \
	-lavformat -lavcodec -lavutil \
	-I/g/open_source_lib/ffmpeg-3.4.1/include \
	-L/g/open_source_lib/ffmpeg-3.4.1/lib
