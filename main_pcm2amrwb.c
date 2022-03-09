#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "enc_if.h"


// 编译时Makefile里控制
#ifdef ENABLE_DEBUG
    #define DEBUG(fmt, args...)     printf(fmt, ##args)
#else
    #define DEBUG(fmt, args...)
#endif


/* PCM参数 */
#define PCM_SAMPLERATE 	(16000) /* 只能编码 8 khz */
#define PCM_SAMPLEBITS 	(16) 	/* 只支持16位 */
#define PCM_CHANNELS 	(1) 	/* 不管PCM输入是单声道还是双声道，这里输出的amr都是单声道的 */

/* amr一帧数据是20ms，一秒50帧。16000,16,1 ==> 640 Bytes */
#define PCM_ONE_FRAME_SIZE  (PCM_SAMPLERATE/50 * PCM_SAMPLEBITS/8 * PCM_CHANNELS)

/*******************
      AMR参数
   mode    rate
    0      6600
    1      8850
    2     12650
    3     14250
    4     15850
    5     18250
    6     19850
    7     23050
    8     23850
 *******************/
#define AMR_ENCODE_MODE 	(8)


/* 是否使能背景噪声编码模式 */
#define DTX_DECODE_ENABLE 	1
#define DTX_DECODE_DISABLE 	0
#define DTX_DECODE_MODE 	DTX_DECODE_ENABLE



int main(int argc, char *argv[])
{
	int mode = AMR_ENCODE_MODE;
	int dtx = DTX_DECODE_MODE;
	void *vpAmr = NULL;
	FILE *fpAmr = NULL;
	FILE *fpPcm = NULL;
	uint8_t u8PcmBuf[PCM_ONE_FRAME_SIZE] = {0}; 		/* 保存在文件中一帧（20ms）PCM数据，8bit为单位，这里是unsigned */
	int16_t u16EncInBuf[PCM_ONE_FRAME_SIZE/2] = {0}; 	/* 编码需要的一帧（20ms）PCM数据，16bit为单位 */
	uint8_t u8EncOutBuf[PCM_ONE_FRAME_SIZE] = {0};		/* 编码出来的一帧（20ms）AMR数据（一般装不满） */
	int iReadPcmBytes = 0; 								/* 从PCM文件中读取出的数据大小，单位：字节 */
	int iEncAmrBytes = 0; 								/* 编码出的AMR数据大小，单位：字节 */


	/* 检查参数 */
	if(argc != 3)
	{
		printf("Usage: \n"
			   "\t %s ./audio/test_16000_16_1.pcm out1.amr\n"
			   "\t %s ./audio/test_16000_16_2.pcm out2.amr (need to modify PCM_CHANNELS=2)\n", argv[0], argv[0]);
		return -1;
	}
	printf("It will encode a PCM file as [sample rate: %d] - [sample bits: %d] - [channels: %d] !\n",
			PCM_SAMPLERATE, PCM_SAMPLEBITS, PCM_CHANNELS);

	/* 初始化编码器 */
	vpAmr = E_IF_init();
	if(vpAmr == NULL)
	{
		printf("E_IF_init() error!\n");
		goto exit;
	}

	/* 打开文件 */
	fpPcm = fopen(argv[1], "rb");
	if(fpPcm == NULL)
	{
		perror("argv[1]");
		goto exit;
	}

	fpAmr = fopen(argv[2], "wb");
	if(fpAmr == NULL)
	{
		perror("argv[2]");
		goto exit;
	}

	/* 先写入amr-wb的头部 */
	fwrite("#!AMR-WB\n", 1, 9, fpAmr);

	/* 循环编码 */
	while(1)
	{
		/* 读出一帧PCM数据 */
		iReadPcmBytes = fread(u8PcmBuf, 1, PCM_ONE_FRAME_SIZE, fpPcm);
		if(iReadPcmBytes < PCM_ONE_FRAME_SIZE)
		{
			break;
		}
		DEBUG("[in pcm bytes: %d]\t", iReadPcmBytes);

		/* 转换类型 */
		for(int i = 0; i < PCM_ONE_FRAME_SIZE/2; i++)
		{
			uint8_t *p = &u8PcmBuf[2*PCM_CHANNELS*i];
			u16EncInBuf[i] = (p[1] << 8) | p[0];
		}

		/* 编码 */
		iEncAmrBytes = E_IF_encode(vpAmr, mode, u16EncInBuf, u8EncOutBuf, dtx);

		DEBUG("[out amr-wb bytes: %d]\n", iEncAmrBytes);

		/* 写入到AMR文件中 */
		fwrite(u8EncOutBuf, 1, iEncAmrBytes, fpAmr);
	}

	printf("%s -> %s: Success!\n", argv[1], argv[2]);

exit:

	/* 关闭文件 */
	if(fpAmr)
		fclose(fpAmr);

	if(fpPcm)
		fclose(fpPcm);

	/* 关闭编码器 */
	if(vpAmr)
		E_IF_exit(vpAmr);

	return 0;
}
