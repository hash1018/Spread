#include "framedata.h"

FrameData::FrameData(int width,int height)
    :width(width),height(height),buffer(nullptr),pts(0),frameNumber(0),ptsRealTime(0) {

    this->buffer=new uint8_t [this->width * this->height * 4];
}

FrameData::~FrameData(){

    if(this->buffer!=nullptr)
        delete [] this->buffer;
}
