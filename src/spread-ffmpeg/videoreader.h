
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
    int64_t totalFrameCount;


    //this is the value of counting for when avcodec_receive_frame is called , got response to AVERROR(EAGAIN)
    //this response means there is no data enough to be fully decoded.
    //in the readFrame function. we never get AVERROR_EOF to find out if decoded frame is final one.
    //so count this value to figure it out.
    //in frameNumber == totalFrameCount - invalidFrameCount case, decoded frame is the last one.
    int invalidFrameCount;

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
    bool seekFrame(int64_t frameIndex, FrameData &frameData);

public:
    inline int getFps() const {return this->fps;}
    inline int getWidth() const {return this->width; }
    inline int getHeight() const {return this->height; }
    inline int64_t getTotalFrameCount() const {return this->totalFrameCount;}

};

#endif // VIDEOREADER_H
