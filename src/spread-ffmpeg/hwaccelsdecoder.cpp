
#include "hwaccelsdecoder.h"
#include "src/spread-ffmpeg/framedata.h"


HwAccelsDecoder::HwAccelsDecoder()
    :avFormatContext(NULL), avCodecContext(NULL), avFrame(NULL),
      avPacket(NULL), swsContext(NULL), width(0), height(0),
      videoStreamIndex(-1),fps(0), invalidFrameCount(0),
      type(AVHWDeviceType::AV_HWDEVICE_TYPE_NONE),
      closed(false),opened(false) {


}

HwAccelsDecoder::~HwAccelsDecoder(){

    if(this->closed==false)
        this->close();

}

bool HwAccelsDecoder::open(const char *filePath){

    if(this->opened==true){

        printf("already opened\n");
        return false;
    }

    if(this->closed==true){

        printf("already closed\n");
        return false;
    }

    enum AVHWDeviceType *list=NULL;
    int count=0;
    HwAccelsDecoder::getSupportedHwDeviceTypes(&list,&count);

    if(list==NULL){

        printf("no supported hwDevice.\n");
        return false;
    }



    this->avFormatContext=avformat_alloc_context();
    if(!this->avFormatContext){

        printf("Couldn't created AVFormatContext\n");
        return false;
    }

    if(avformat_open_input(&this->avFormatContext, filePath, NULL, NULL) !=0) {

        printf("Couldn't open video file\n");
        return false;
    }


    this->videoStreamIndex = -1;
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

    int i = 0;

    for(i; i< count; i++){

        if(HwAccelsDecoder::checkIfCodecSupportDeviceType(av_codec,list[i])==true){

            this->type = list[i];
            break;
        }
    }

    delete [] list;

    if(i == count){

        printf("Codec doesn't support any of deviceType\n");
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

    this->avCodecContext->get_format  = getHwFormat;
    if (hw_decoder_init(this->avCodecContext, this->type) < 0){

        printf(" failed to hw_decoder_init\n");
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

bool HwAccelsDecoder::readFrame(FrameData &frameData){

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

                printf("Failed to decode packet: \n");

                av_packet_unref(this->avPacket);

                return false;

            }
            response= avcodec_receive_frame(this->avCodecContext,this->avFrame);
            if(response == AVERROR(EAGAIN)) {

                printf("eagain");

                av_packet_unref(this->avPacket);

                av_frame_unref(this->avFrame);

                this->invalidFrameCount++;
                continue;
            }
            else if( response == AVERROR_EOF){

                printf("averror_eof");

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


    AVFrame *finalFrame = NULL;
    AVFrame *swFrame = av_frame_alloc();

    if(swFrame==NULL){

        return false;
    }


    if(this->avFrame->format == hwPixelFormat){
        //retrive data from gpu to cpu.
        if(av_hwframe_transfer_data(swFrame, this->avFrame, 0) < 0 ){

            av_frame_unref(this->avFrame);
            av_frame_free(&swFrame);
            return false;
        }
        finalFrame=swFrame;
    }
    else{

        finalFrame=this->avFrame;
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

/*
    printf("finalFormat %s \n",av_get_pix_fmt_name((enum AVPixelFormat)finalFrame->format) );




    int size = av_image_get_buffer_size((enum AVPixelFormat) finalFrame->format, finalFrame->width,
                                    finalFrame->height, 1);

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
                                  (const uint8_t * const *)finalFrame->data,
                                  (const int *)finalFrame->linesize, (enum AVPixelFormat) finalFrame->format,
                                  finalFrame->width, finalFrame->height, 1);

    if(ret < 0 ){

        printf("failed copy\n");
        return false;
    }

    frameData.copyToBuffer(buffer);

    av_freep(&buffer);



    printf("kkkkk\n");

*/

/*

    if(this->swsContext==NULL){

        this->swsContext=sws_getContext(finalFrame->width, finalFrame->height,
                                        (enum AVPixelFormat) finalFrame->format,
                                       frameData.width,frameData.height, AV_PIX_FMT_RGB0,
                                       SWS_FAST_BILINEAR, NULL,NULL,NULL);
    }

    if(!this->swsContext){

        printf("Couldn't initialize swsContext\n");
        return false;
    }

    uint8_t *dest[4] = {frameData.buffer, NULL, NULL, NULL};
    int dest_linesize[4] = { frameData.width *4, 0, 0, 0};

    sws_scale(this->swsContext,finalFrame->data,finalFrame->linesize, 0, finalFrame->height, dest, dest_linesize);
*/






    /////////////////////////
    ///
    ///
    ///
    int size = av_image_get_buffer_size(AVPixelFormat::AV_PIX_FMT_YUV420P, finalFrame->width,
                                    finalFrame->height, 1);



    if(this->swsContext == NULL){
        this->swsContext=sws_getContext(finalFrame->width, finalFrame->height, (enum AVPixelFormat)finalFrame->format,
                                        frameData.width,frameData.height, AV_PIX_FMT_YUV420P,
                                        SWS_FAST_BILINEAR, NULL,NULL,NULL);
    }

    if(!this->swsContext){

        printf("Couldn't initialize swsContext\n");
        return false;
    }

    AVPicture picture;
    avpicture_alloc(&picture,AVPixelFormat::AV_PIX_FMT_YUV420P,finalFrame->width,finalFrame->height);

    sws_scale(this->swsContext,finalFrame->data,finalFrame->linesize,0,frameData.height,picture.data,picture.linesize);

    if(frameData.getBufferSize() != size){
        frameData.setBufferSize(size);
    }

    uint8_t *buffer =new uint8_t [size];

    memcpy(buffer,picture.data[0],finalFrame->width*finalFrame->height);

    buffer += finalFrame->width * finalFrame->height;
    memcpy(buffer,picture.data[1],finalFrame->width * finalFrame->height / 4);

    buffer += finalFrame->width * finalFrame->height / 4;
    memcpy(buffer,picture.data[2],finalFrame->width * finalFrame->height / 4);


    buffer -= finalFrame->width * finalFrame->height;
    buffer -= finalFrame->width * finalFrame->height / 4;
    frameData.copyToBuffer(buffer);

    avpicture_free(&picture);
    delete [] buffer;











    av_frame_unref(swFrame);
    av_frame_free(&swFrame);

    av_frame_unref(this->avFrame);

    return true;
}


bool HwAccelsDecoder::close(){


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

bool HwAccelsDecoder::seekFrame(int64_t frameIndex, FrameData &frameData){


    if(this->opened == false){

        printf("VideoReader is not opened yet.\n");
        return false;
    }

    if(this->closed == true){

        printf("VideoReader is already closed. \n");
        return false;
    }

    //int64_t seekTarget=frameNumber * this->avFormatContext->streams[this->videoStreamIndex]->time_base.num * AV_TIME_BASE /
      //      this->avFormatContext->streams[this->videoStreamIndex]->time_base.den;



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

int HwAccelsDecoder::hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&this->hwDeviceContext, type,
                                      NULL, NULL, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(this->hwDeviceContext);

    return err;
}


void HwAccelsDecoder::getSupportedHwDeviceTypes(enum AVHWDeviceType* *list,int *count) {

    AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;

    *count = 0;
    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE){
        printf( "%s" ,av_hwdevice_get_type_name(type));
        printf("\n");
        (*count)++;
    }

    if(*count == 0){

        *list = NULL;
        return;
    }

    int index=0;
    *list = new enum AVHWDeviceType[*count];

    type = AV_HWDEVICE_TYPE_NONE;
    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE){

        (*list)[index]=type;
        index++;
    }

}


bool HwAccelsDecoder::checkIfCodecSupportDeviceType(AVCodec *avCodec,enum AVHWDeviceType type){

    int i = 0;

    for (i = 0;; i++) {

        const AVCodecHWConfig *config = avcodec_get_hw_config(avCodec, i);

        if (!config) {
            fprintf(stderr, "Decoder %s does not support device type\n" /*%s.  %d\n"*/,
                    avCodec->name/*, av_hwdevice_get_type_name(this->type), i*/);
            return false;
        }

        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                config->device_type == type) {

            hwPixelFormat = config->pix_fmt;
            printf("%s\n", av_hwdevice_get_type_name(type));

            return true;
        }
    }

    return false;
}

