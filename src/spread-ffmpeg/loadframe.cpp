

extern "C"{

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

bool loadFrame(const char *fileName, int *width_out, int *height_out, unsigned char* *data_out){

    AVFormatContext *av_format_ctx=avformat_alloc_context();
    if(!av_format_ctx){

        printf("Couldn't created AVFormatContext\n");
        return false;
    }

    if(avformat_open_input(&av_format_ctx, fileName, NULL, NULL) !=0) {

        printf("Couldn't open video file\n");
        return false;
    }

    int video_stream_index=-1;
    AVCodecParameters *av_codec_params;
    AVCodec *av_codec;

    for(unsigned int i=0; i< av_format_ctx->nb_streams; i++){

        av_codec_params=av_format_ctx->streams[i]->codecpar;
        av_codec=avcodec_find_decoder(av_codec_params->codec_id);

        if(!av_codec){
            continue;
        }
        if(av_codec_params->codec_type==AVMEDIA_TYPE_VIDEO){

            video_stream_index=i;
            break;
        }
    }

    if(video_stream_index==-1){

        printf("Couldn't find vaild video stream file \n");
        return false;
    }


    AVCodecContext *av_codec_ctx=avcodec_alloc_context3(av_codec);
    if(!av_codec_ctx){
        printf("Couldn't create AVCodecContext\n");
        return false;
    }

    if(avcodec_parameters_to_context(av_codec_ctx,av_codec_params) < 0){
        printf("Couldn't initialize AVCodecContext\n");
        return false;
    }

    if(avcodec_open2(av_codec_ctx,av_codec,NULL) < 0 ){

        printf("Couldn't open codec\n");
        return false;
    }

    AVFrame *av_frame=av_frame_alloc();
    if(!av_frame){

        printf("Couldn't allocate AVFrame\n");
        return false;
    }

    AVPacket *av_packet=av_packet_alloc();
    if(!av_packet){
        printf("Couldn't allocate AVPacket\n");
        return false;
    }


    while ( av_read_frame(av_format_ctx,av_packet) >=0){

        if(av_packet->stream_index != video_stream_index){
            continue;
        }

        int response=avcodec_send_packet(av_codec_ctx,av_packet);
        if(response  < 0){

            printf("Failed to decode packet: %s\n",av_err2str(response));
            return false;
        }
        response= avcodec_receive_frame(av_codec_ctx,av_frame);
        if(response == AVERROR(EAGAIN) || response == AVERROR_EOF){
            continue;
        }
        else if(response < 0){

            printf("Failed to decode packet: %s\n",av_err2str(response));
            return false;
        }

        av_packet_unref(av_packet);
        break;
    }
/*
    unsigned char *data=new unsigned char[av_frame->width * av_frame->height * 3 ];
    for(int x=0; x< av_frame->width; x++){

        for(int y=0; y< av_frame->height; y++){
            data[y * av_frame->width *3 + x * 3    ] = av_frame->data[0][y * av_frame->linesize[0] + x ];
            data[y * av_frame->width *3 + x * 3 + 1] = av_frame->data[0][y * av_frame->linesize[0] + x ];
            data[y * av_frame->width *3 + x * 3 + 2]  =av_frame->data[0][y * av_frame->linesize[0] + x ];

        }
    }

    *width_out=av_frame->width;
    *height_out=av_frame->height;
    *data_out=data;
*/

    SwsContext *sws_ctx=sws_getContext(av_frame->width, av_frame->height, av_codec_ctx->pix_fmt,
                                       av_frame->width,av_frame->height, AV_PIX_FMT_RGB0,
                                       SWS_BILINEAR, NULL,NULL,NULL);

    if(!sws_ctx){

        printf("Couldn't initialize swsContext\n");
        return false;
    }

    uint8_t *data=new uint8_t[av_frame->width * av_frame->height *4 ];

    uint8_t *dest[4] = {data,NULL,NULL,NULL};
    int dest_linesize[4] = { av_frame->width *4, 0,0,0};


    sws_scale(sws_ctx,av_frame->data,av_frame->linesize,0, av_frame->height,dest,dest_linesize);

    *width_out=av_frame->width;
    *height_out=av_frame->height;
    *data_out=data;



    sws_freeContext(sws_ctx);
    avformat_close_input(&av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);

    return true;
}
