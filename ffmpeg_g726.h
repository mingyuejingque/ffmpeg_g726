

int codec_init(void** in_ctx); //返回0成功， 其他值表示相应错误码。
int codec_destroy(void** ctx);
int decode(void* ctx, char* g726data, int in_len, char* pcm, int out_len);
