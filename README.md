
##### 前言

 - AMR-NB(AMR-NarrowBind)：语音带宽范围：300 - 3700Hz，8KHz采样频率；
 - AMR-WB(AMR-WideBand)：语音带宽范围50 - 7000Hz，16KHz采样频率。

不管NB还是WB，一帧都是20ms，一秒50帧。



### 1、开源库下载地址

opencore-amr-0.1.3.tar.gz：支持amr-nb的编码与解码、amr-wb的解码，但不支持amr-wb的编码；

vo-amrwbenc-0.1.3.tar.gz：仅支持amr-wb的编码。

opencore-amr/vo-amrwbenc的下载地址：[https://sourceforge.net/projects/opencore-amr/files/](https://sourceforge.net/projects/opencore-amr/files/)



### 2、编译开源库

 - **opencore-amr**

```bash
#!/bin/bash

tar xzf opencore-amr-0.1.3.tar.gz
cd opencore-amr-0.1.3/
./configure --prefix=$PWD/_install # --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
make -j96
make install
```

- **vo-amrwbenc**

```bash
#!/bin/bash

tar xzf vo-amrwbenc-0.1.3.tar.gz
cd vo-amrwbenc-0.1.3
./configure --prefix=$PWD/_install # --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
make -j96
make install
```



### 3、demo使用

- **编译**

```bash
make
# 或者打开编译调试信息
make clean && make DEBUG=1
```

- **使用**

```bash
# 编码
$ ./pcm2amrwb
Usage:
         ./pcm2amrwb ./audio/test_16000_16_1.pcm out1.amr
         ./pcm2amrwb ./audio/test_16000_16_2.pcm out2.amr (need to modify PCM_CHANNELS=2)

# 解码
$ ./amrwb2pcm
Usage:
         ./amrwb2pcm ./audio/test.amr out_16000_16_1.pcm
```



### 4、参考资料

- [AMR编码文件解析\_hanzhen7541的博客-CSDN博客_amr编码](https://blog.csdn.net/hanzhen7541/article/details/100932834)

- opencore-amr-0.1.3/test/amrwb-dec.c

- vo-amrwbenc-0.1.3/amrwb-enc.c



### 附录（demo目录架构）

```bash
$ tree
.
├── audio
│   ├── test_16000_16_1.pcm
│   ├── test_16000_16_2.pcm
│   └── test.amr
├── include
│   ├── dec_if.h
│   └── enc_if.h
├── lib
│   ├── libopencore-amrwb.a
│   └── libvo-amrwbenc.a
├── main_amrwb2pcm.c
├── main_pcm2amrwb.c
├── Makefile
├── README.md
└── reference
    ├── amrwb-dec.c
    ├── amrwb-enc.c
    └── AMR编码文件解析_hanzhen7541的博客-CSDN博客.mhtml
```

