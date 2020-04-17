
#include "videoreader.h"
#include "framedata.h"

extern "C"{

#include <libavutil/imgutils.h>

}

VideoReader::VideoReader()
    :avFormatContext(NULL), avCodecContext(NULL), avFrame(NULL),
      avPacket(NULL), swsContext(NULL), width(0), height(0),
      videoStreamIndex(-1),fps(0), invalidFrameCount(0), closed(false),opened(false) {

}

VideoReader::~VideoReader(){

    if(this->closed==false)
        this->close();

}

bool VideoReader::open(const char *fileName){

    if(this->opened==true){

        printf("already opened\n");
        return false;
    }

    if(this->closed==true){

        printf("already closed\n");
        return false;
    }

    this->avFormatContext=avformat_alloc_context();
    if(!this->avFormatContext){

        printf("Couldn't created AVFormatContext\n");
        return false;
    }

    if(avformat_open_input(&this->avFormatContext, fileName, NULL, NULL) !=0) {

        printf("Couldn't open video file\n");
        return false;
    }

    this->videoStreamIndex=-1;
    AVCodecParameters *av_codec_params;
    AVCodec *av_codec;

    for(unsigned int i=0; i< this->avFormatContext->nb_streams; i++){

        av_codec_params=this->avFormatContext->streams[i]->codecpar;
        av_codec=avcodec_find_decoder(av_codec_params->codec_id);

        if(!av_codec){
            continue;
        }
        if(av_codec_params->codec_type==AVMEDIA_TYPE_VIDEO){

            this->videoStreamIndex=i;
            this->width=av_codec_params->width;
            this->height=av_codec_params->height;
            break;
        }
    }

    if(this->videoStreamIndex==-1){

        printf("Couldn't find vaild video stream file \n");
        return false;
    }


    this->avCodecContext=avcodec_alloc_context3(av_codec);
    if(!this->avCodecContext){
        printf("Couldn't create AVCodecContext\n");
        return false;
    }

    if(avcodec_parameters_to_context(this->avCodecContext,av_codec_params) < 0){
        printf("Couldn't initialize AVCodecContext\n");
        return false;
    }

    if(avcodec_open2(this->avCodecContext,av_codec,NULL) < 0 ){

        printf("Couldn't open codec\n");
        return false;
    }

    this->avFrame=av_frame_alloc();
    if(!this->avFrame){

        printf("Couldn't allocate AVFrame\n");
        return false;
    }

    this->avPacket=av_packet_alloc();
    if(!this->avPacket){
        printf("Couldn't allocate AVPacket\n");
        return false;
    }

    this->fps=this->avFormatContext->streams[this->videoStreamIndex]->avg_frame_rate.num /
            this->avFormatContext->streams[this->videoStreamIndex]->avg_frame_rate.den;

    this->totalFrameCount=this->avFormatContext->streams[this->videoStreamIndex]->nb_frames;
    this->opened=true;
    return true;
}

bool VideoReader::readFrame(FrameData &frameData){

    if(this->opened==false){

        printf("videoReader is not open\n");
        return false;
    }

    if(this->closed==true){

        printf("videoReader is already closed\n");
        return false;
    }

    while ( av_read_frame(this->avFormatContext,this->avPacket) >=0){

        if(this->avPacket->stream_index == this->videoStreamIndex){

            int response=avcodec_send_packet(this->avCodecContext,this->avPacket);
            if(response  < 0){

                printf("Failed to decode packet:\n");

                av_packet_unref(this->avPacket);

                return false;
            }
            response= avcodec_receive_frame(this->avCodecContext,this->avFrame);
            if(response == AVERROR(EAGAIN)) {

                av_packet_unref(this->avPacket);

                av_frame_unref(this->avFrame);

                this->invalidFrameCount++;
                continue;
            }
            else if( response == AVERROR_EOF){

                av_packet_unref(this->avPacket);

                av_frame_unref(this->avFrame);

                return false;
            }
            else if(response < 0){

                printf("Failed to decode packet: \n");

                av_packet_unref(this->avPacket);
                av_frame_unref(this->avFrame);


                return false;
            }

            av_packet_unref(this->avPacket);
            break;
        }

        av_packet_unref(this->avPacket);
    }

    frameData.pts=this->avFrame->pts;
    frameData.ptsRealTime=this->avFrame->pts *
            (double)this->avFormatContext->streams[this->videoStreamIndex]->time_base.num /
            (double)this->avFormatContext->streams[this->videoStreamIndex]->time_base.den;
    frameData.frameIndex=(double(this->fps*this->avFrame->pts) /
                          (double)this->avFormatContext->streams[this->videoStreamIndex]->time_base.den) *
            (double)this->avFormatContext->streams[this->videoStreamIndex]->time_base.num;

    if(frameData.frameIndex == this->totalFrameCount - this->invalidFrameCount -1 /* -1 is because frameNumber starts from zero.*/){

        frameData.finalFrame=true;
    }

    ////////////////////////////////////  11
/*
    printf("this->avFrame %s \n",av_get_pix_fmt_name((enum AVPixelFormat)this->avFrame->format) );




    int size = av_image_get_buffer_size((enum AVPixelFormat) this->avFrame->format, this->avFrame->width,
                                    this->avFrame->height, 1);

    if(frameData.getBufferSize() != size){

        frameData.setBufferSize(size);
    }

    printf("size %d\n", size );


    uint8_t* buffer=NULL;
    buffer = (uint8_t*)av_malloc(size);
    if (buffer == NULL) {

        printf("not alloced\n");
        return false;

    }
    int ret=av_image_copy_to_buffer(buffer, size,
                                  (const uint8_t * const *)this->avFrame->data,
                                  (const int *)this->avFrame->linesize, (enum AVPixelFormat) this->avFrame->format,
                                  this->avFrame->width, this->avFrame->height, 1);

    if(ret < 0 ){

        printf("failed copy\n");
        return false;
    }

    frameData.copyToBuffer(buffer);

    av_freep(&buffer);



    printf("kkkkk\n");
*/


///////////////////////////////////// 22
/*

    if(this->swsContext==NULL){

       // this->swsContext=sws_getContext(this->avFrame->width, this->avFrame->height, this->avCodecContext->pix_fmt,
       //                                frameData.width,frameData.height, AV_PIX_FMT_RGB0,
       //                                SWS_FAST_BILINEAR, NULL,NULL,NULL);


    }

    if(!this->swsContext){

        printf("Couldn't initialize swsContext\n");
        return false;
    }


    //uint8_t *dest[4] = {frameData.buffer, NULL, NULL, NULL};
    //int dest_linesize[4] = { frameData.width *4, 0, 0, 0};

    //sws_scale(this->swsContext,this->avFrame->data,this->avFrame->linesize, 0, this->avFrame->height, dest, dest_linesize);
*/



///////////////////////////////////////  33

    int size = av_image_get_buffer_size(AVPixelFormat::AV_PIX_FMT_YUV420P, this->avFrame->width,
                                    this->avFrame->height, 1);



    if(this->swsContext == NULL){
        this->swsContext=sws_getContext(this->avFrame->width, this->avFrame->height, this->avCodecContext->pix_fmt,
                                        frameData.width,frameData.height, AV_PIX_FMT_YUV420P,
                                        SWS_FAST_BILINEAR, NULL,NULL,NULL);
    }

    if(!this->swsContext){

        printf("Couldn't initialize swsContext\n");
        return false;
    }

    AVPicture picture;
    avpicture_alloc(&picture,AVPixelFormat::AV_PIX_FMT_YUV420P,this->avFrame->width,this->avFrame->height);

    sws_scale(this->swsContext,this->avFrame->data,this->avFrame->linesize,0,frameData.height,picture.data,picture.linesize);

    if(frameData.getBufferSize() != size){
        frameData.setBufferSize(size);
    }

    uint8_t *buffer =new uint8_t [size];

    memcpy(buffer,picture.data[0],this->avFrame->width*this->avFrame->height);

    buffer += this->avFrame->width * this->avFrame->height;
    memcpy(buffer,picture.data[1],this->avFrame->width * this->avFrame->height / 4);

    buffer += this->avFrame->width * this->avFrame->height / 4;
    memcpy(buffer,picture.data[2],this->avFrame->width * this->avFrame->height / 4);


    buffer -= this->avFrame->width * this->avFrame->height;
    buffer -= this->avFrame->width * this->avFrame->height / 4;
    frameData.copyToBuffer(buffer);

    avpicture_free(&picture);
    delete [] buffer;



    printf ("sdsa %d  ",size);

    /////////////////////


    av_frame_unref(this->avFrame);

    return true;
}


bool VideoReader::close(){

    if(this->opened==false)
        return false;

    if(this->closed==true)
        return false;

    if(this->swsContext!=NULL)
        sws_freeContext(this->swsContext);

    avformat_close_input(&this->avFormatContext);
    av_frame_free(&this->avFrame);
    av_packet_free(&this->avPacket);
    avformat_free_context(this->avFormatContext);
    avcodec_free_context(&this->avCodecContext);
    this->closed=true;

    return true;
}

bool VideoReader::seekFrame(int64_t frameIndex, FrameData &frameData) {

    if(this->opened == false){

        printf("VideoReader is not opened yet.\n");
        return false;
    }

    if(this->closed == true){

        printf("VideoReader is already closed. \n");
        return false;
    }

    /*int64_t seekTarget=frameNumber * this->avFormatContext->streams[this->videoStreamIndex]->time_base.num * AV_TIME_BASE /
            this->avFormatContext->streams[this->videoStreamIndex]->time_base.den;
    */


    if(av_seek_frame(this->avFormatContext, this->videoStreamIndex, frameIndex, AVSEEK_FLAG_BACKWARD) < 0){

        printf("Couldn't seek frame.\n");
        return false;
    }

    avcodec_flush_buffers(this->avCodecContext);

    do{

        this->readFrame(frameData);

    }while(frameData.frameIndex < frameIndex );


    return true;
}
