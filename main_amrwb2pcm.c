#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dec_if.h"


// 编译时Makefile里控制
#ifdef ENABLE_DEBUG
    #define DEBUG(fmt, args...)     printf(fmt, ##args)
#else
    #define DEBUG(fmt, args...)
#endif


/* amrwb解码出来的PCM就是这个参数 */
#define PCM_SAMPLERATE  (16000)
#define PCM_SAMPLEBITS  (16)
#define PCM_CHANNELS    (1)

/* amr一帧数据是20ms，一秒50帧。16000,16,1 ==> 640 Bytes */
#define PCM_ONE_FRAME_SIZE  (PCM_SAMPLERATE/50 * PCM_SAMPLEBITS/8 * PCM_CHANNELS)



int main(int argc, char *argv[])
{
	void *vpAmr = NULL;
	FILE *fpAmr = NULL;
	FILE *fpPcm = NULL;
	uint8_t u8AmrHeader[9] = {0};
	int iReadBytes = 0;
	int iFrameSize = 0; 		// 根据AMR文件每帧的头部获取该帧数据大小
	const int iFrameSizes[] = { 17, 23, 32, 36, 40, 46, 50, 58, 60, 5, -1, -1, -1, -1, -1, 0 };
	uint8_t u8AmrBuf[PCM_ONE_FRAME_SIZE] = {0}; 	// 给予足够的空间，一般装不满
	uint8_t u8PcmBuf[PCM_ONE_FRAME_SIZE] = {0}; 	// 解码出来的是以8bit为单位
	int16_t u16DecBuf[PCM_ONE_FRAME_SIZE/2] = {0}; 	// 解码出来的是以16bit为单位


	/* 检查参数 */
	if(argc != 3)
	{
		printf("Usage: \n"
			   "\t %s ./audio/test.amr out_16000_16_1.pcm\n", argv[0]);
		return -1;
	}

	/* 初始化解码器 */
	vpAmr = D_IF_init();
	if(vpAmr == NULL)
	{
		printf("Decoder_Interface_init() error!\n");
		goto exit;
	}

	/* 打开文件 */
	fpAmr = fopen(argv[1], "rb");
	if(fpAmr == NULL)
	{
		perror("argv[1]");
		goto exit;
	}
	fpPcm = fopen(argv[2], "wb");
	if(fpPcm == NULL)
	{
		perror("argv[2]");
		goto exit;
	}

	/* 判断是否为AMR文件 */
	iReadBytes = fread(u8AmrHeader, 1, 9, fpAmr);
	if (iReadBytes != 9 || memcmp(u8AmrHeader, "#!AMR-WB\n", 9)) {
		printf("%s is not a amr(wb) file!\n", argv[1]);
		goto exit;
	}

	/* 循环解码 */
	while(1)
	{
		/* 获取AMR规格 */
		iReadBytes = fread(u8AmrBuf, 1, 1, fpAmr);
		if(iReadBytes <= 0)
			break;

		/* 获取一帧的大小 */
		iFrameSize = iFrameSizes[(u8AmrBuf[0] >> 3) & 0x0F];

		/* 读取一帧AMR数据，需要注意的是记得偏移一个地址 */
		iReadBytes = fread(&u8AmrBuf[1], 1, iFrameSize, fpAmr);
		if(iFrameSize != iReadBytes)
			break;

		DEBUG("[in amr bytes: %d]\t", 1 + iReadBytes);
#if 0
		/* 解码方式 1：像官方测试程序一样解码出来存到short类型的缓存里 */
		/* 将AMR数据解码 */
		D_IF_decode(vpAmr, u8AmrBuf, u16DecBuf, 0/* 参数未使用 */);

		uint8_t *p = u8PcmBuf;
		for(int i = 0; i < PCM_ONE_FRAME_SIZE/2; i++)
		{
			*p++ = (u16DecBuf[i] >> 0) & 0xff;
			*p++ = (u16DecBuf[i] >> 8) & 0xff;
		}
#else
		/* 解码方式2：传参时直接强制类型转换即可 */
		/* 将AMR数据解码 */
		D_IF_decode(vpAmr, u8AmrBuf, (short *)u8PcmBuf, 0/* 参数未使用 */);
#endif

		DEBUG("[out pcm bytes: %d]\n", PCM_ONE_FRAME_SIZE);

		fwrite(u8PcmBuf, 1, PCM_ONE_FRAME_SIZE, fpPcm);
	}

	printf("%s -> %s : Success!\n", argv[1], argv[2]);

exit:

	/* 关闭文件 */
	if(fpAmr)
		fclose(fpAmr);
	if(fpPcm)
		fclose(fpPcm);

	/* 关闭解码器 */
	if(vpAmr)
		D_IF_exit(vpAmr);

	return 0;
}
