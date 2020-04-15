#include "framedata.h"

FrameData::FrameData(int width,int height)
    :width(width),height(height),buffer(nullptr),pts(0),frameIndex(0),ptsRealTime(0),finalFrame(false) {

    this->bufferSize=this->width * this->height * 4;
    this->buffer=new uint8_t [this->bufferSize];

}

FrameData::~FrameData(){

    if(this->buffer!=nullptr)
        delete [] this->buffer;
}


void FrameData::setBufferSize(int bufferSize){

    this->bufferSize=bufferSize;

    if(this->buffer!=nullptr)
        delete [] this->buffer;

    this->buffer=new uint8_t [this->bufferSize];

}

#include <string.h>
void FrameData::copyToBuffer(uint8_t *buffer){

    memcpy(this->buffer,buffer,this->bufferSize);
}
