
#include "hwaccelsdecoder.h"
#include "src/spread-ffmpeg/framedata.h"

/*
HwAccelsDecoder::HwAccelsDecoder() {

    AVHWDeviceType type=AVHWDeviceType::AV_HWDEVICE_TYPE_NONE;

    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
        qDebug() << av_hwdevice_get_type_name(type);
}

HwAccelsDecoder::~HwAccelsDecoder() {



    //packet.data = NULL;
    //packet.size = 0;
    //ret = decode_write(decoder_ctx, &packet);
    //av_packet_unref(&packet);


    //avcodec_free_context(&decoder_ctx);
    //avformat_close_input(&input_ctx);
    //av_buffer_unref(&hw_device_ctx);


}

bool HwAccelsDecoder::open(const char *filePath){


    this->type=AVHWDeviceType::AV_HWDEVICE_TYPE_VIDEOTOOLBOX;


    // open the input file
    if (avformat_open_input(&input_ctx, filePath, NULL, NULL) != 0) {
        qDebug() << "Cannot open input file";
        return false;
    }

    if (avformat_find_stream_info(input_ctx, NULL) < 0) {
        qDebug() << "Cannot find input stream information.";
        return false;
    }

    // find the video stream information
    ret = av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (ret < 0) {
        qDebug() << "Cannot find a video stream in the input file";
        return false;
    }

    video_stream = ret;

    int i;

    for (i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
        if (!config) {
            fprintf(stderr, "Decoder %s does not support device type %s.\n",
                    decoder->name, av_hwdevice_get_type_name(type));
            return -1;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    if (!(decoder_ctx = avcodec_alloc_context3(decoder))){
       // return AVERROR(ENOMEM);
        qDebug() << " cannot allocate codec context";
        return false;
    }

    video = input_ctx->streams[video_stream];
    if (avcodec_parameters_to_context(decoder_ctx, video->codecpar) < 0){

        qDebug() <<" cannot initialize codecContex";
        return false;
    }

    decoder_ctx->get_format  = get_hw_format;
    if (hw_decoder_init(decoder_ctx, type) < 0){

        qDebug() <<" failed to hw_decoder_init";
        return false;

    }

    if ((ret = avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
        qDebug() << "Failed to open codec for stream";
        return false;
    }


    return true;

}

bool HwAccelsDecoder::takeAction(){

    qDebug() << "takeAction";

    //open the file to dump raw data
    //output_file = fopen(argv[3], "w+");

    // actual decoding and dump the raw data
    while (ret >= 0) {
        if ((ret = av_read_frame(input_ctx, &packet)) < 0)
            break;

        if (video_stream == packet.stream_index)
            ret = decode_write(decoder_ctx, &packet);

        av_packet_unref(&packet);
    }


    return true;
}

*/





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

    //this->type=AVHWDeviceType::AV_HWDEVICE_TYPE_VIDEOTOOLBOX;

    //this->type=AVHWDeviceType::AV_HWDEVICE_TYPE_NONE;


    int count=0;
    AVHWDeviceType type=AV_HWDEVICE_TYPE_NONE;


    //this->type=AVHWDeviceType::AV_HWDEVICE_TYPE_QSV;
    //this->type=AVHWDeviceType::AV_HWDEVICE_TYPE_DXVA2;


    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE){
        printf( av_hwdevice_get_type_name(type));
        printf("\n");
        count++;
    }

    if(count==0){

        printf("no supported hardwareDevice format\n");
        return false;
    }

    int index=0;
    enum AVHWDeviceType *list=new enum AVHWDeviceType[count];

    type=AV_HWDEVICE_TYPE_NONE;
    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE){

        list[index]=type;
        index++;
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

    int i;
    int j=0;
    bool foundHwPixelFormat=false;

    for (i = 0;; i++) {
        printf("%d\n",i);
        const AVCodecHWConfig *config = avcodec_get_hw_config(av_codec, i);

        if (!config) {
            fprintf(stderr, "Decoder %s does not support device type\n" /*%s.  %d\n"*/,
                    av_codec->name/*, av_hwdevice_get_type_name(this->type), i*/);
            return false;
        }

        j=0;

        for(j; j< count; j++){

            if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                    config->device_type == list[j]) {
                hwPixelFormat = config->pix_fmt;

                printf("fdfdfdf\n");
                printf("%s\n", av_hwdevice_get_type_name(list[j]));
                this->type=list[j];
                foundHwPixelFormat=true;
                break;
            }
        }

        if(foundHwPixelFormat==true)
            break;
    }

    delete [] list;


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
    int size = av_image_get_buffer_size((enum AVPixelFormat) finalFrame->format, finalFrame->width,
                                    finalFrame->height, 1);

*/




    if(this->swsContext==NULL){

        this->swsContext=sws_getContext(finalFrame->width, finalFrame->height, //this->avCodecContext->pix_fmt
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

