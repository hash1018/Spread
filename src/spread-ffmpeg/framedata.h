
#ifndef FRAMEDATA_H
#define FRAMEDATA_H

#include <inttypes.h>

class FrameData{

    friend class VideoReader;

private:
    int width;
    int height;
    uint8_t *buffer;

    int64_t pts;

    int frameNumber;

    double ptsRealTime;

public:
    FrameData(int width,int height);
    ~FrameData();

public:
    inline int getWidth() const {return this->width;}
    inline int getHeight() const {return this->height;}
    inline int64_t getPts() const {return this->pts;}
    inline const uint8_t* getBuffer() const {return this->buffer;}
    inline int getFrameNumber() const {return this->frameNumber;}
    inline double getPtsRealTime() const {return this->ptsRealTime;}
};

#endif // FRAMEDATA_H
