//
///*#include <speex/speex_callbacks.h>*/  
//#include <speex/speex.h> 
//#include <stdio.h> 
///*֡�Ĵ�С�������������һ���̶���ֵ,���������Ǳ�������*/
//#define FRAME_SIZE 160 
//
//int main(int argc, char **argv)
//{
//	char *inFile;
//	FILE *fin;
//	short in[FRAME_SIZE];//160 
//	float input[FRAME_SIZE];//160 
//	char cbits[200];
//	int nbBytes;
//
//	/*��������״̬*/
//	void *state;
//	/*�����ֽ�������ǿ��Ա�speex�����д*/
//	SpeexBits bits;
//	int i, tmp;
//	//�½�һ���µı���״̬��խ��(narrowband)ģʽ�� 
//	state = speex_encoder_init(&speex_nb_mode);
//	//��������Ϊ8(15kbps) 
//	tmp = 8;
//	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);
//
//	inFile = argv[1];
//	fprintf(stderr, "inFile:%s \n", inFile);
//	fin = fopen(inFile, "r");
//	//��ʼ���ṹʹ���Ǳ������� 
//	speex_bits_init(&bits);
//	fprintf(stderr, "  nbBits:%s \n", bits.nbBits);
//	fprintf(stderr, "buf_size:%s \n", bits.buf_size);
//	while (1)
//	{
//		//����һ֡16bits������ 
//		fread(in, sizeof(short), FRAME_SIZE, fin);//160 
//		if (feof(fin))
//			break;
//
//		//��16bits��ֵת��Ϊfloat,�Ա�speex����������湤�� 
//		for (i = 0; i<FRAME_SIZE; i++)
//			input[i] = in[i];
//
//		//�������ṹ�������е��ֽ�,�Ա����ǿ��Ա���һ���µ�֡ 
//		speex_bits_reset(&bits);
//		//��֡���б��� 
//		speex_encode(state, input, &bits);
//		//��bits������һ������д����char������ 
//		nbBytes = speex_bits_write(&bits, cbits, 200);
//		//����д��֡�Ĵ�С,����sampledec�ļ���Ҫ��һ��ֵ,�������Ӧ�ó����п��ܲ�һ�� 
//		fwrite(&nbBytes, sizeof(int), 1, stdout);
//		//д��ѹ��������� 
//		fwrite(cbits, 1, nbBytes, stdout);
//	}
//
//	//�ͷű�����״̬�� 
//	speex_encoder_destroy(state);
//	//�ͷ�bit_packing�ṹ 
//	speex_bits_destroy(&bits);
//	fclose(fin);
//	getchar();
//	return 0;
//}