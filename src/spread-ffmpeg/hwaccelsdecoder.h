
#ifndef HWACCELSDECODER_H
#define HWACCELSDECODER_H


extern "C"{



#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <inttypes.h>

}


/*
#include <qelapsedtimer.h>
#include <qdebug.h>

static AVBufferRef *hw_device_ctx = NULL;
static enum AVPixelFormat hw_pix_fmt;
static FILE *output_file = NULL;

static int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      NULL, NULL, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts)
{
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}

static int decode_write(AVCodecContext *avctx, AVPacket *packet)
{

    static int index=0;


    qDebug() <<"decode_write";

    AVFrame *frame = NULL, *sw_frame = NULL;
    AVFrame *tmp_frame = NULL;
    uint8_t *buffer = NULL;
    int size;
    int ret = 0;

    ret = avcodec_send_packet(avctx, packet);
    if (ret < 0) {
        qDebug() << "Error during decoding";
        return ret;
    }

    while (1) {

        QElapsedTimer timer;
        timer.start();
        if (!(frame = av_frame_alloc()) || !(sw_frame = av_frame_alloc())) {
            qDebug() << "Can not alloc frame";
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        ret = avcodec_receive_frame(avctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_frame_free(&frame);
            av_frame_free(&sw_frame);
            return 0;
        } else if (ret < 0) {
            qDebug() <<"Error while decoding";
            goto fail;
        }

        if (frame->format == hw_pix_fmt) {
            // retrieve data from GPU to CPU
            if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) {
                qDebug() <<"Error transferring the data to system memory";
                goto fail;
            }
            tmp_frame = sw_frame;
        } else
            tmp_frame = frame;

        size = av_image_get_buffer_size((enum AVPixelFormat) tmp_frame->format, tmp_frame->width,
                                        tmp_frame->height, 1);


        buffer = (uint8_t*)av_malloc(size);
        if (!buffer) {
            qDebug() <<"Can not alloc buffer";
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        ret = av_image_copy_to_buffer(buffer, size,
                                      (const uint8_t * const *)tmp_frame->data,
                                      (const int *)tmp_frame->linesize, (enum AVPixelFormat) tmp_frame->format,
                                      tmp_frame->width, tmp_frame->height, 1);
        if (ret < 0) {
            qDebug() << "Can not copy image to buffer";
            goto fail;
        }
        qDebug() << av_get_pix_fmt_name((enum AVPixelFormat)tmp_frame->format);

        qDebug() << "timer elapsed   " << timer.elapsed() << "  index " <<index++;

       // if ((ret = fwrite(buffer, 1, size, output_file)) < 0) {
       //     fprintf(stderr, "Failed to dump raw data.\n");
       //     goto fail;
       // }
       //

    fail:
        av_frame_free(&frame);
        av_frame_free(&sw_frame);
        av_freep(&buffer);
        if (ret < 0)
            return ret;
    }
}


class HwAccelsDecoder {


private:
    AVFormatContext *input_ctx = NULL;
    int video_stream;
    int ret;
    AVStream *video = NULL;
    AVCodecContext *decoder_ctx = NULL;
    AVCodec *decoder = NULL;
    AVPacket packet;
    enum AVHWDeviceType type;

private:
    bool opened=false;
    bool closed=false;

public:
    HwAccelsDecoder();
    ~HwAccelsDecoder();


public:
    bool open(const char *filePath);
    bool takeAction();

};

*/



static enum AVPixelFormat hwPixelFormat;
//static AVBufferRef *hwDeviceContext = NULL;

static enum AVPixelFormat getHwFormat(AVCodecContext *avCodecContext,
                                        const enum AVPixelFormat *avPixelFormat)
{
    const enum AVPixelFormat *p;

    for (p = avPixelFormat; *p != -1; p++) {
        if (*p == hwPixelFormat)
            return *p;
    }

    fprintf(stderr, "getHwFormat Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}



class FrameData;

class HwAccelsDecoder {

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


    enum AVHWDeviceType type;

private:
    bool closed;
    bool opened;

public:
    HwAccelsDecoder();
    ~HwAccelsDecoder();

    bool open(const char *filePath);
    bool readFrame(FrameData &frameData);
    bool close();
    bool seekFrame(int64_t frameIndex, FrameData &frameData);

public:
    inline int getFps() const {return this->fps;}
    inline int getWidth() const {return this->width; }
    inline int getHeight() const {return this->height; }
    inline int64_t getTotalFrameCount() const {return this->totalFrameCount;}


private:
    AVBufferRef *hwDeviceContext = NULL;
    int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type);
};



#endif // HWACCELSDECODER_H
