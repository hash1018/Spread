
#ifndef VIDEOREADER_H
#define VIDEOREADER_H

extern "C"{

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}


class FrameData;

class VideoReader {

private:
    AVFormatContext *avFormatContext;
    AVCodecContext *avCodecContext;
    AVFrame *avFrame;
    AVPacket *avPacket;
    SwsContext *swsContext;

    int width;
    int height;

    int videoStreamIndex;

    int fps;

private:
    bool closed;
    bool opened;

public:
    VideoReader();
    ~VideoReader();

    bool open(const char *fileName);
    //bool readFrame(uint8_t *buffer);
    bool readFrame(FrameData &frameData);
    bool close();

public:
    inline int getFps() const {return this->fps;}
    inline int getWidth() const {return this->width; }
    inline int getHeight() const {return this->height; }
};

#endif // VIDEOREADER_H
