#include <stdio.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#include "ffmpeg_g726.h"

typedef struct {
	AVCodec *codec;
	AVCodecContext *dec_ctx;
	AVFrame *decoded_frame;
	AVPacket avpkt;

} ffmpeg_g726ctx;

int codec_init(void** in_ctx) {
	avcodec_register_all();
	*in_ctx = av_mallocz(sizeof(ffmpeg_g726ctx));
	ffmpeg_g726ctx* ctx = (ffmpeg_g726ctx*)(*in_ctx);
	ctx->codec = avcodec_find_decoder(AV_CODEC_ID_ADPCM_G726LE);
	if (!ctx->codec) 
		return 1;

	auto c = avcodec_alloc_context3(ctx->codec);
	c->bits_per_coded_sample = 5;
	c->channels = 1;
	c->sample_fmt = AV_SAMPLE_FMT_S16;
	c->sample_rate = 8000;
	c->codec_type = AVMEDIA_TYPE_AUDIO;
	c->bit_rate = 16000;
	ctx->dec_ctx = c;

	int iRet = avcodec_open2(c, ctx->codec,NULL);
	if (iRet < 0)
		return 2;

	ctx->decoded_frame = av_frame_alloc();
	av_init_packet(&ctx->avpkt);
	return 0;
}

int codec_destroy(void** ctx) {
	auto x = (ffmpeg_g726ctx*)(*ctx);
	if (x == nullptr)
		return 0;

	if (x->decoded_frame) {
		av_frame_free(&x->decoded_frame);
	}

	if (x->dec_ctx) {
		avcodec_close(x->dec_ctx);
		avcodec_free_context(&x->dec_ctx);
	}

	av_freep(x);
	return 0;
}

int decode(void* ctx, char* g726data, int in_len, char* pcm, int out_len) {
	
	int data_size = 0;
	int sample_size = 0;
	ffmpeg_g726ctx* ffctx = (ffmpeg_g726ctx*)ctx;
	ffctx->avpkt.data = (uint8_t*)g726data;
	ffctx->avpkt.size = in_len;

	sample_size = av_get_bytes_per_sample(ffctx->dec_ctx->sample_fmt);
	int len = avcodec_send_packet(ffctx->dec_ctx, &ffctx->avpkt);
	if (len < 0) {
		printf("avcodec_send_packet faild, return: %d\n", len);
		return len;
	}

	if (len >= 0) {
		if (!ffctx->decoded_frame && !(ffctx->decoded_frame = av_frame_alloc())) {
			fprintf(stderr, "Could not allocate audio frame\n");
			return 0;
		}
		int ret = avcodec_receive_frame(ffctx->dec_ctx, ffctx->decoded_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        	printf("avcodec_receive_frame faild: %d\n", ret);
            return 0;
        }

		data_size = sample_size * ffctx->decoded_frame->nb_samples;
		memcpy(pcm, ffctx->decoded_frame->data[0], FFMIN(out_len, data_size));
	}


	return data_size;
}
