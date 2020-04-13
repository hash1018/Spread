
#ifndef FRAMEDATA_H
#define FRAMEDATA_H

#include <inttypes.h>

class FrameData{

    friend class VideoReader;
    friend class HwAccelsDecoder;

private:
    int width;
    int height;
    uint8_t *buffer;

    int64_t pts;

    int frameIndex;

    double ptsRealTime;
    bool finalFrame;

public:
    FrameData(int width,int height);
    ~FrameData();

public:
    inline int getWidth() const {return this->width;}
    inline int getHeight() const {return this->height;}
    inline int64_t getPts() const {return this->pts;}
    inline const uint8_t* getBuffer() const {return this->buffer;}
    inline int getFrameIndex() const {return this->frameIndex;}
    inline double getPtsRealTime() const {return this->ptsRealTime;}
    inline bool isFinalFrame() const {return this->finalFrame;}
};

#endif // FRAMEDATA_H
