all: pcm2amrwb amrwb2pcm

# our static lib is compile by x86_64 gcc
CC := gcc
CFLAGS := -I./include

# 控制程序里的DEBUG打印开关
ifeq ($(DEBUG), 1)
CFLAGS += -DENABLE_DEBUG
endif

pcm2amrwb: main_pcm2amrwb.c lib/libvo-amrwbenc.a
	$(CC) $^ $(CFLAGS) -o $@

amrwb2pcm: main_amrwb2pcm.c lib/libopencore-amrwb.a
	$(CC) $^ $(CFLAGS) -o $@

clean :
	rm -rf pcm2amrwb amrwb2pcm out*
.PHONY := clean

