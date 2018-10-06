#ifndef _CONFIG_H
#define _CONFIG_H

#define DA_V 1.0f			//DA参考电压
#define AD_V 1.0f			//AD参考电压
#define DA_WIDTH 1			//DA输入数据宽度
#define AD_WIDTH 8			//AD输出数据宽度
#define CROSSBAR_L 1152		//crossbar长度
#define CROSSBAR_W 128		//crossbar宽度
#define CROSSBAR_N 4		//crossbar个数 in one tile
#define AD_REUSE_NUM 32		//AD复用
// #define XB01_I 0.00492679327726364
// #define XB2_I 0.00398490577936172
// #define XB3_I 0.00257207546383142


#define XB0_I 255.0000
#define XB1_I 544.1123
#define XB2_I 577.6968
#define XB3_I 498.8477
#define XB4_I 416.2358
#define XB5_I 422.0193
#define XB6_I 551.4277
#define XB7_I 560.6636
#define XB8_I 514.5170
#define XB9_I 386.2259
#define XB10_I 384.3456
#define XB11_I 451.3769
#define XB12_I 401.3741
#define XB13_I 263.9953
#define XB14_I 207.4094
#define XB15_I 135.6547
#define XB16_I 36.3211
#define XB17_I 28.6738

#define KERNEL_SIZE 3
//输入8bit数据个数, a convolution block
#define INPUT_SIZE KERNEL_SIZE*KERNEL_SIZE	
#define CHANNELS_3 3		// each input picture channels
#define PICTURE_NUM	100		// number of input pictures
#define IMAGE_SIZE_32 32		// input picture size
#define IMAGE_SIZE_16 16
#define IMAGE_SIZE_8 8
#define SIMULATE_DURATION 400	//simulation duration, in nanoseconds

#define INPUT_LINEAR_1 128 // linear layer 1 input size
#define INPUT_LINEAR_2 128 // linear layer 1 output size and linear layer 2 input size
#define	OUTPUT_LINEAR 10 // 10 classifier result

#define CHANNELS_32 32
#define CHANNELS_48 48
#define CHANNELS_80 80
#define CHANNELS_128 128

#define POOLING_SIZE_1 1
#define POOLING_SIZE_2 2
#define POOLING_SIZE_8 8
#endif // !_CONFIG_H

