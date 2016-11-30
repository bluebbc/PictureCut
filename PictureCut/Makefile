# Makefile

all: static

dynamic :main.cpp SavePictrue.cpp SavePictrue.h
	g++ -o PictrueCut main.cpp SavePictrue.cpp \
		-D__STDC_CONSTANT_MACROS \
	-I/home/maozheng/local/ffmpeg/include  -lavcodec   -lavfilter  -lavutil    -lswresample  -lavdevice  -lavformat  -lpostproc  -lswscale

static : main.cpp SavePictrue.cpp SavePictrue.h
	g++ -o PictrueCut main.cpp SavePictrue.cpp \
		-D__STDC_CONSTANT_MACROS \
		-I/home/maozheng/include \
		/home/maozheng/local/ffmpeg/lib/libavcodec.a  \
		/home/maozheng/local/ffmpeg/lib/libavfilter.a \
		/home/maozheng/local/ffmpeg/lib/libavdevice.a \
		/home/maozheng/local/ffmpeg/lib/libavformat.a \
		/home/maozheng/local/ffmpeg/lib/libavcodec.a  \
		/home/maozheng/local/ffmpeg/lib/libpostproc.a \
		/home/maozheng/local/ffmpeg/lib/libswscale.a  \
		/home/maozheng/local/ffmpeg/lib/libavutil.a   \
		/home/maozheng/local/ffmpeg/lib/libswresample.a   \
		/home/maozheng/local/x264/lib/libx264.a       \
		-lpthread -lz -ldl -lbz2  -lrt 

clean:
	rm *.jpg -f
	rm PictrueCut -f

