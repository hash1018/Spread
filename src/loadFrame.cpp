

extern "C"{

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
}

bool loadFrame(const char *fileName, int *width, int *height, unsigned char* *data){


printf(avformat_configuration());

	AVFormatContext *av_format_ctx=avformat_alloc_context();
	if(!av_format_ctx){

		printf("Couldn't created AVFormatContext\n");
		return false;
	}

	if(avformat_open_input(&av_format_ctx,fileName,NULL,NULL) !=0) {

		printf("Couldn't open video file\n");
		return false;
	}
	


	return false;
}
