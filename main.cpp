#include <stdio.h>
#include "ffmpeg_g726.h"

int main(int argc, char* argv[]) {
	printf("hello world!\n");

	if (argc < 3)  {
		printf("%s in.g726 out.pcm");
		return -1;
	}

	constexpr int in_size = 100;
	constexpr int out_size = 320;
	char in_buff[in_size];
	char out_buff[out_size];

	FILE *in, *out;
	in = fopen(argv[1], "rb");
	out = fopen(argv[2], "wb");

	void *ctx;
	if (codec_init(&ctx) != 0) {
		printf("codec_init faild!");
		goto clean;
	}
	printf("codec_init success!\n");

	while(fread(in_buff, 1, sizeof(in_buff), in) == in_size) {
		int n = decode(ctx, in_buff, in_size, out_buff, out_size);
		printf("got pcm len: %d\n", n);
		fwrite(out_buff, 1, n, out);
	}

clean:
	if (in) 
		fclose(in);
	if (out)
		fclose(out);
	if(ctx)
		codec_destroy(&ctx);
	return 0;
}