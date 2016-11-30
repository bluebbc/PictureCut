#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};

int MSavePicture(AVFrame *pFrameYUV, char *out_file, int width, int height);