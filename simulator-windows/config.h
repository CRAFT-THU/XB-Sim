#ifndef _CONFIG_H
#define _CONFIG_H

#define DA_V 1.0f			//DA参考电压
#define AD_V 1.0f			//AD参考电压
#define DA_WIDTH 1			//DA输入数据宽度
#define AD_WIDTH 8			//AD输出数据宽度
#define CROSSBAR_L 1152		//crossbar长度
#define CROSSBAR_W 128		//crossbar宽度
#define CROSSBAR_N 1		//crossbar个数
#define AD_REUSE_NUM 32		//AD复用
#define XB01_I 0.00492679327726364
#define XB2_I 0.00398490577936172
#define XB3_I 0.00257207546383142

#define KERNEL_SIZE 3
//输入8bit数据个数, a convolution block
#define INPUT_SIZE KERNEL_SIZE*KERNEL_SIZE	
#define CHANNELS 2			// each input picture channels
#define PICTURE_NUM	1		// number of input pictures
#define IMAGE_SIZE 32		// input picture size
#define SIMULATE_DURATION 400	//simulation duration, in nanoseconds
#define ALL_SIGNALS	2

#endif // !_CONFIG_H

