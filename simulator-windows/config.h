#ifndef _CONFIG_H
#define _CONFIG_H

#define DA_V 1.0f			//DA�ο���ѹ
#define AD_V 1.0f			//AD�ο���ѹ
#define DA_WIDTH 1			//DA�������ݿ��
#define AD_WIDTH 8			//AD������ݿ��
#define CROSSBAR_L 1152		//crossbar����
#define CROSSBAR_W 128		//crossbar���
#define CROSSBAR_L_2304 2304
#define CROSSBAR_W_512 512
#define CROSSBAR_L_4608 4608
#define CROSSBAR_W_4096 4096
#define CROSSBAR_W_2048 2048
#define CROSSBAR_N 4		//crossbar���� in one tile
#define AD_REUSE_NUM 32		//AD����
// #define XB01_I 0.00492679327726364
// #define XB2_I 0.00398490577936172
// #define XB3_I 0.00257207546383142


#define XB0_I 255.0000
//#define XB1_I 4.00 // 544.1123
//#define XB2_I 2.80 // 577.6968
//#define XB3_I 2.60 // 498.8477
//#define XB4_I 2.90 // 416.2358
//#define XB5_I 3.00 // 422.0193
//#define XB6_I 3.80 // 551.4277
//#define XB7_I 4.15 // 560.6636
//#define XB8_I 3.00 // 514.5170
//#define XB9_I 2.70 // 386.2259
//#define XB10_I 2.60 // 384.3456
//#define XB11_I 3.40 // 451.3769
//#define XB12_I 3.10 // 401.3741
//#define XB13_I 2.50 // 263.9953
//#define XB14_I 1.95 // 207.4094
//#define XB15_I 1.75 // 135.6547
//#define XB16_I 1.10 // 36.3211
//#define XB17_I 1.50 // 28.6738

#define XB1_I 9.00 // 544.1123
#define XB2_I 3.00 // 577.6968
#define XB3_I 3.00 // 498.8477
#define XB4_I 3.10 // 416.2358
#define XB5_I 3.30 // 422.0193
#define XB6_I 4.50 // 551.4277
#define XB7_I 5.30 // 560.6636
#define XB8_I 4.90 // 514.5170
#define XB9_I 3.30 // 386.2259
#define XB10_I 3.20 // 384.3456
#define XB11_I 3.70 // 451.3769
#define XB12_I 3.10 // 401.3741
#define XB13_I 3.00 // 263.9953
#define XB14_I 1.90 // 207.4094
#define XB15_I 1.80 // 135.6547
#define XB16_I 1.50 // 36.3211
#define XB17_I 2.10 // 28.6738
#define XB18_I 1.00
#define XB19_I 6.00

#define KERNEL_SIZE 3
//����8bit���ݸ���, a convolution block
#define INPUT_SIZE KERNEL_SIZE*KERNEL_SIZE	
#define CHANNELS_3 3		// each input picture channels
#define PICTURE_NUM	10000		// number of input pictures
#define IMAGE_SIZE_32 32		// input picture size
#define IMAGE_SIZE_16 16
#define IMAGE_SIZE_8 8
#define SIMULATE_DURATION 400	//simulation duration, in nanoseconds

//#define INPUT_LINEAR_1 128 // linear layer 1 input size
//#define INPUT_LINEAR_2 128 // linear layer 1 output size and linear layer 2 input size
//#define	OUTPUT_LINEAR 10 // 10 classifier result

#define INPUT_LINEAR_1 128 // linear layer 1 input size
#define INPUT_LINEAR_2 500 // linear layer 1 output size and linear layer 2 input size
#define INPUT_LINEAR_3 4096 // linear layer 2 output size and linear layer 3 input size
#define INPUT_LINEAR_4 2048 // linear layer 3 output size and linear layer 4 input size
#define	OUTPUT_LINEAR 10 // 10 classifier result

#define CHANNELS_32 32
#define CHANNELS_48 48
#define CHANNELS_80 80
#define CHANNELS_128 128

#define POOLING_SIZE_1 1
#define POOLING_SIZE_2 2
#define POOLING_SIZE_8 8
#endif // !_CONFIG_H

