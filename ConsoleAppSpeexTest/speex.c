// ConsoleAppSpeexTest.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#ifdef HAVE_CONFIG_H  
#include "config.h"  
#endif  

#include <stdio.h>  
#include <stdlib.h>  
#include <speex/speex_callbacks.h>  
#include <speex/speex.h>  

#ifdef FIXED_DEBUG  
extern long long spx_mips;
#endif  

#define FRAME_SIZE 160  
#include <math.h>  

//C++中利用Speex进行音频压缩
//测试：E:\myCode\speex\m0000.wav E:\myCode\speex\m0000Decode.wav E:\myCode\speex\m0000Encode.wav  
int main(int argc, char **argv)
{
	char *inFile, *outFile, *bitsFile;
	FILE *fin, *fout, *fbits = NULL;
	short in_short[FRAME_SIZE]; //编解码的数据160   
	short out_short[FRAME_SIZE];//160
	int snr_frames = 0;
	char cbits[200];
	int nbBits;
	int i;
	void *st;
	void *dec;
	SpeexBits bits;
	spx_int32_t tmp;
	int bitCount = 0;
	spx_int32_t skip_group_delay;
	SpeexCallback callback;

	//初始化编解码  
	st = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
	dec = speex_decoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
	/* BEGIN: You probably don't need the following in a real application */
	//设置一些回调属性，这些我没用到 ：）  
	callback.callback_id = SPEEX_INBAND_CHAR;
	callback.func = speex_std_char_handler;
	callback.data = stderr;
	speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);

	callback.callback_id = SPEEX_INBAND_MODE_REQUEST;
	callback.func = speex_std_mode_request_handler;
	callback.data = st;
	speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);
	/* END of unnecessary stuff */

	tmp = 1;
	speex_decoder_ctl(dec, SPEEX_SET_ENH, &tmp);
	tmp = 0;
	speex_encoder_ctl(st, SPEEX_SET_VBR, &tmp);
	tmp = 8;
	speex_encoder_ctl(st, SPEEX_SET_QUALITY, &tmp);
	tmp = 1;
	speex_encoder_ctl(st, SPEEX_SET_COMPLEXITY, &tmp);

	// 设置属性值  
	/* Turn this off if you want to measure SNR (on by default) */
	tmp = 1;
	speex_encoder_ctl(st, SPEEX_SET_HIGHPASS, &tmp);
	speex_decoder_ctl(dec, SPEEX_SET_HIGHPASS, &tmp);

	speex_encoder_ctl(st, SPEEX_GET_LOOKAHEAD, &skip_group_delay);
	speex_decoder_ctl(dec, SPEEX_GET_LOOKAHEAD, &tmp);
	skip_group_delay += tmp;

	if (argc != 4 && argc != 3)
	{
		fprintf(stderr, "Usage: encode [in file] [out file] [bits file]\nargc = %d", argc);
		getchar();
		exit(1);
	}

	inFile = argv[1];
	fin = fopen(inFile, "rb");// 读文件  
	printf("inFile:%s\n", inFile);

	outFile = argv[2];
	fout = fopen(outFile, "wb+"); // 准备将编码后文件写入此文件  
	printf("fout:%s\n", outFile);
	printf("argc:%d\n", argc);

	if (argc == 4)
	{
		bitsFile = argv[3];
		fbits = fopen(bitsFile, "wb");
		printf("bitsFile:%s\n", bitsFile);
	}
	speex_bits_init(&bits);
	while (!feof(fin))
	{
		fread(in_short, sizeof(short), FRAME_SIZE, fin); //读取160    
		if (feof(fin))
			break;

		speex_bits_reset(&bits);
		speex_encode_int(st, in_short, &bits);
		nbBits = speex_bits_write(&bits, cbits, 200); //通过speex编码  
		bitCount += bits.nbBits;
		printf("bitCount:%d %d %d\n", bitCount, nbBits, skip_group_delay);

		if (argc == 4)
			fwrite(cbits, 1, nbBits, fbits);

		speex_bits_rewind(&bits);
		speex_decode_int(dec, &bits, out_short);
		speex_bits_reset(&bits);

		fwrite(&out_short[skip_group_delay], sizeof(short), FRAME_SIZE - skip_group_delay, fout); // 写入  
		skip_group_delay = 0;
	}
	fprintf(stderr, "Total encoded size: %d bits\n", bitCount);
	speex_encoder_destroy(st);
	speex_decoder_destroy(dec);
	speex_bits_destroy(&bits); // 释放speex资源  

#ifndef DISABLE_FLOAT_API  
	{
		float sigpow, errpow, snr, seg_snr = 0;
		sigpow = 0;
		errpow = 0;

		/* This code just computes SNR, so you don't need it either */
		rewind(fin);
		rewind(fout);

		while (FRAME_SIZE == fread(in_short, sizeof(short), FRAME_SIZE, fin)
			&&
			FRAME_SIZE == fread(out_short, sizeof(short), FRAME_SIZE, fout))
		{
			float s = 0, e = 0;
			for (i = 0; i <FRAME_SIZE; ++i) {
				s += (float)in_short[i] * in_short[i];
				e += ((float)in_short[i] - out_short[i]) * ((float)in_short[i] - out_short[i]);
			}
			seg_snr += 10 * log10((s + 160) / (e + 160));
			sigpow += s;
			errpow += e;
			snr_frames++;
		}
		snr = 10 * log10(sigpow / errpow);
		seg_snr /= snr_frames;
		fprintf(stderr, "SNR = %f\nsegmental SNR = %f\n", snr, seg_snr);

#ifdef FIXED_DEBUG  
		printf("Total: %f MIPS\n", (float)(1e-6 * 50 * spx_mips / snr_frames));
#endif  
	}
#endif  

	fclose(fin);
	fclose(fout);
	if (argc == 4)
		fclose(fbits);
	getchar();
	return 0;
}







