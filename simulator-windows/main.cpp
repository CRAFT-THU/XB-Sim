#include "systemc.h"
#include "config.h"
#include "numgen.h"
#include "stage_conv_1.h"
#include "stage_conv_2.h"
#include "stage_conv_3.h"
#include "stage_conv_4.h"
#include "stage_conv_5.h"
#include "stage_conv_6.h"
#include "stage_conv_7.h"
#include "stage_conv_8.h"
#include "stage_conv_9.h"
#include "stage_conv_10.h"
#include "stage_conv_11.h"
#include "stage_conv_12.h"
#include "stage_conv_13.h"
#include "stage_conv_14.h"
#include "stage_conv_15.h"
#include "conv_buffer_1.h"
#include "conv_buffer_2.h"
#include "conv_buffer_3.h"
#include "conv_buffer_4.h"
#include "conv_buffer_5.h"
#include "conv_buffer_6.h"
#include "conv_buffer_7.h"
#include "conv_buffer_8.h"
#include "conv_buffer_9.h"
#include "conv_buffer_10.h"
#include "conv_buffer_11.h"
#include "conv_buffer_12.h"
#include "conv_buffer_13.h"
#include "conv_buffer_14.h"
#include "stage_linear_16.h"
#include "stage_linear_17.h"
#include "linear_buffer_15.h"
#include "linear_buffer_16.h"
#include "display.h"

#include <time.h>

int sc_main(int argc, char* argv[]) {
	// define signals
	sc_signal<bool> clock; // clock 
	sc_signal<bool> clock_1;
	sc_signal<int> number; // number of input picture
	sc_signal<int> signal_0; // signals for modules
	sc_signal<int> signal_1;
	sc_signal<int> signal_2;
	sc_signal<int> signal_3;
	sc_signal<int> signal_4;
	sc_signal<int> signal_5;
	sc_signal<int> signal_6;
	sc_signal<int> signal_7;
	sc_signal<int> signal_8;
	sc_signal<int> signal_9;
	sc_signal<int> signal_10;
	sc_signal<int> signal_11;
	sc_signal<int> signal_12;
	sc_signal<int> signal_13;
	sc_signal<int> signal_14;
	sc_signal<int> signal_15;
	sc_signal<int> signal_16;
	sc_signal<int> signal_17;
	sc_signal<int> signal_18;
	sc_signal<int> signal_19;
	sc_signal<int> signal_20;
	sc_signal<int> signal_21;
	sc_signal<int> signal_22;
	sc_signal<int> signal_23;
	sc_signal<int> signal_24;
	sc_signal<int> signal_25;
	sc_signal<int> signal_26;
	sc_signal<int> signal_27;
	sc_signal<int> signal_28;
	sc_signal<int> signal_29;
	sc_signal<int> signal_30;
	sc_signal<int> signal_31;
	sc_signal<int> signal_32;
	sc_signal<int> signal_33;
	sc_signal<int> signal_34;
	// int signal_counter = 0;

	sc_signal<float> send_data1[INPUT_SIZE*CHANNELS_3];
	sc_signal<float> conv_1_out[CHANNELS_32];
	sc_signal<float> conv_2_out[CHANNELS_32];
	sc_signal<float> conv_3_out[CHANNELS_32];
	sc_signal<float> conv_4_out[CHANNELS_48];
	sc_signal<float> conv_5_out[CHANNELS_48];
	sc_signal<float> conv_6_out[CHANNELS_80];
	sc_signal<float> conv_7_out[CHANNELS_80];
	sc_signal<float> conv_8_out[CHANNELS_80];
	sc_signal<float> conv_9_out[CHANNELS_80];
	sc_signal<float> conv_10_out[CHANNELS_80];
	sc_signal<float> conv_11_out[CHANNELS_128];
	sc_signal<float> conv_12_out[CHANNELS_128];
	sc_signal<float> conv_13_out[CHANNELS_128];
	sc_signal<float> conv_14_out[CHANNELS_128];
	sc_signal<float> conv_15_out[CHANNELS_128];
	sc_signal<float> linear_1_out[INPUT_LINEAR_2];
	sc_signal<float> linear_2_out[OUTPUT_LINEAR];

	sc_signal<float> buff_1_out[CHANNELS_32][INPUT_SIZE];
	sc_signal<float> buff_2_out[CHANNELS_32][INPUT_SIZE];
	sc_signal<float> buff_3_out[CHANNELS_32][INPUT_SIZE];
	sc_signal<float> buff_4_out[CHANNELS_48][INPUT_SIZE];
	sc_signal<float> buff_5_out[CHANNELS_48][INPUT_SIZE];
	sc_signal<float> buff_6_out[CHANNELS_80][INPUT_SIZE];
	sc_signal<float> buff_7_out[CHANNELS_80][INPUT_SIZE];
	sc_signal<float> buff_8_out[CHANNELS_80][INPUT_SIZE];
	sc_signal<float> buff_9_out[CHANNELS_80][INPUT_SIZE];
	sc_signal<float> buff_10_out[CHANNELS_80][INPUT_SIZE];
	sc_signal<float> buff_11_out[CHANNELS_128][INPUT_SIZE];
	sc_signal<float> buff_12_out[CHANNELS_128][INPUT_SIZE];
	sc_signal<float> buff_13_out[CHANNELS_128][INPUT_SIZE];
	sc_signal<float> buff_14_out[CHANNELS_128][INPUT_SIZE];
	sc_signal<float> buff_linear_1_out[INPUT_LINEAR_1];
	sc_signal<float> buff_linear_2_out[INPUT_LINEAR_2];
	
	// number generator
	numgen Num_gen("number_generator");
	for (int j = 0; j < INPUT_SIZE*CHANNELS_3; j++){
		Num_gen.output[j](send_data1[j]);
	}
	Num_gen.clock(clock);
	Num_gen.clock_1(clock_1);
	Num_gen.signal_out(signal_0);

	// convolution module
	stage_conv_1 Conv_1("conv_module_1");
	for (int j = 0; j < INPUT_SIZE*CHANNELS_3; j++) {
		Conv_1.input[j](send_data1[j]);
	}
	for (int j = 0; j < CHANNELS_32; j++) {
		Conv_1.output[j](conv_1_out[j]);
	}
	Conv_1.signal_in(signal_0);
	Conv_1.signal_out(signal_1);

	// buffer 1
	conv_buffer_1 buff_1("stage_buffer_1");
	for (int j = 0; j < CHANNELS_32; j++){
		buff_1.input[j](conv_1_out[j]);
	}
	for (int j = 0; j < CHANNELS_32; j++){
		for (int k = 0; k < INPUT_SIZE; k++){
			buff_1.output[j][k](buff_1_out[j][k]);
		}
	}
	buff_1.signal_in(signal_1);
	buff_1.signal_out(signal_2);
	buff_1.clock_1(clock_1);

	// convolution module
	stage_conv_2 Conv_2("conv_module_2");
	for (int j = 0; j < CHANNELS_32; j++) {
		for (int k = 0; k < INPUT_SIZE; k++){
			Conv_2.input[j*INPUT_SIZE+k](buff_1_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_32; j++) {
		Conv_2.output[j](conv_2_out[j]);
	}
	Conv_2.signal_in(signal_2);
	Conv_2.signal_out(signal_3);

	// buffer 2
	conv_buffer_2 buff_2("stage_buffer_2");
	for (int j = 0; j < CHANNELS_32; j++) {
		buff_2.input[j](conv_2_out[j]);
	}
	for (int j = 0; j < CHANNELS_32; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_2.output[j][k](buff_2_out[j][k]);
		}
	}
	buff_2.signal_in(signal_3);
	buff_2.signal_out(signal_4);
	buff_2.clock_1(clock_1);

	// convolution module
	stage_conv_3 Conv_3("conv_module_3");
	for (int j = 0; j < CHANNELS_32; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_3.input[j*INPUT_SIZE + k](buff_2_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_32; j++) {
		Conv_3.output[j](conv_3_out[j]);
	}
	Conv_3.signal_in(signal_4);
	Conv_3.signal_out(signal_5);

	// buffer 3
	conv_buffer_3 buff_3("stage_buffer_3");
	for (int j = 0; j < CHANNELS_32; j++) {
		buff_3.input[j](conv_3_out[j]);
	}
	for (int j = 0; j < CHANNELS_32; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_3.output[j][k](buff_3_out[j][k]);
		}
	}
	buff_3.signal_in(signal_5);
	buff_3.signal_out(signal_6);
	buff_3.clock_1(clock_1);

	// convolution module
	stage_conv_4 Conv_4("conv_module_4");
	for (int j = 0; j < CHANNELS_32; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_4.input[j*INPUT_SIZE + k](buff_3_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_48; j++) {
		Conv_4.output[j](conv_4_out[j]);
	}
	Conv_4.signal_in(signal_6);
	Conv_4.signal_out(signal_7);

	// buffer 4
	conv_buffer_4 buff_4("stage_buffer_4");
	for (int j = 0; j < CHANNELS_48; j++) {
		buff_4.input[j](conv_4_out[j]);
	}
	for (int j = 0; j < CHANNELS_48; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_4.output[j][k](buff_4_out[j][k]);
		}
	}
	buff_4.signal_in(signal_7);
	buff_4.signal_out(signal_8);
	buff_4.clock_1(clock_1);

	// convolution module
	stage_conv_5 Conv_5("conv_module_5");
	for (int j = 0; j < CHANNELS_48; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_5.input[j*INPUT_SIZE + k](buff_4_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_48; j++) {
		Conv_5.output[j](conv_5_out[j]);
	}
	Conv_5.signal_in(signal_8);
	Conv_5.signal_out(signal_9);

	// buffer 5
	conv_buffer_5 buff_5("stage_buffer_5");
	for (int j = 0; j < CHANNELS_48; j++) {
		buff_5.input[j](conv_5_out[j]);
	}
	for (int j = 0; j < CHANNELS_48; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_5.output[j][k](buff_5_out[j][k]);
		}
	}
	buff_5.signal_in(signal_9);
	buff_5.signal_out(signal_10);
	buff_5.clock_1(clock_1);

	// convolution module
	stage_conv_6 Conv_6("conv_module_6");
	for (int j = 0; j < CHANNELS_48; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_6.input[j*INPUT_SIZE + k](buff_5_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		Conv_6.output[j](conv_6_out[j]);
	}
	Conv_6.signal_in(signal_10);
	Conv_6.signal_out(signal_11);

	// buffer 6
	conv_buffer_6 buff_6("stage_buffer_6");
	for (int j = 0; j < CHANNELS_80; j++) {
		buff_6.input[j](conv_6_out[j]);
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_6.output[j][k](buff_6_out[j][k]);
		}
	}
	buff_6.signal_in(signal_11);
	buff_6.signal_out(signal_12);
	buff_6.clock_1(clock_1);

	// convolution module
	stage_conv_7 Conv_7("conv_module_7");
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_7.input[j*INPUT_SIZE + k](buff_6_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		Conv_7.output[j](conv_7_out[j]);
	}
	Conv_7.signal_in(signal_12);
	Conv_7.signal_out(signal_13);

	// buffer 7
	conv_buffer_7 buff_7("stage_buffer_7");
	for (int j = 0; j < CHANNELS_80; j++) {
		buff_7.input[j](conv_7_out[j]);
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_7.output[j][k](buff_7_out[j][k]);
		}
	}
	buff_7.signal_in(signal_13);
	buff_7.signal_out(signal_14);
	buff_7.clock_1(clock_1);

	// convolution module
	stage_conv_8 Conv_8("conv_module_8");
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_8.input[j*INPUT_SIZE + k](buff_7_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		Conv_8.output[j](conv_8_out[j]);
	}
	Conv_8.signal_in(signal_14);
	Conv_8.signal_out(signal_15);

	// buffer 8
	conv_buffer_8 buff_8("stage_buffer_8");
	for (int j = 0; j < CHANNELS_80; j++) {
		buff_8.input[j](conv_8_out[j]);
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_8.output[j][k](buff_8_out[j][k]);
		}
	}
	buff_8.signal_in(signal_15);
	buff_8.signal_out(signal_16);
	buff_8.clock_1(clock_1);

	// convolution module
	stage_conv_9 Conv_9("conv_module_9");
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_9.input[j*INPUT_SIZE + k](buff_8_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		Conv_9.output[j](conv_9_out[j]);
	}
	Conv_9.signal_in(signal_16);
	Conv_9.signal_out(signal_17);

	// buffer 9
	conv_buffer_9 buff_9("stage_buffer_9");
	for (int j = 0; j < CHANNELS_80; j++) {
		buff_9.input[j](conv_9_out[j]);
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_9.output[j][k](buff_9_out[j][k]);
		}
	}
	buff_9.signal_in(signal_17);
	buff_9.signal_out(signal_18);
	buff_9.clock_1(clock_1);

	// convolution module
	stage_conv_10 Conv_10("conv_module_10");
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_10.input[j*INPUT_SIZE + k](buff_9_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		Conv_10.output[j](conv_10_out[j]);
	}
	Conv_10.signal_in(signal_18);
	Conv_10.signal_out(signal_19);

	// buffer 10
	conv_buffer_10 buff_10("stage_buffer_10");
	for (int j = 0; j < CHANNELS_80; j++) {
		buff_10.input[j](conv_10_out[j]);
	}
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_10.output[j][k](buff_10_out[j][k]);
		}
	}
	buff_10.signal_in(signal_19);
	buff_10.signal_out(signal_20);
	buff_10.clock_1(clock_1);

	// convolution module
	stage_conv_11 Conv_11("conv_module_11");
	for (int j = 0; j < CHANNELS_80; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_11.input[j*INPUT_SIZE + k](buff_10_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		Conv_11.output[j](conv_11_out[j]);
	}
	Conv_11.signal_in(signal_20);
	Conv_11.signal_out(signal_21);

	// buffer 11
	conv_buffer_11 buff_11("stage_buffer_11");
	for (int j = 0; j < CHANNELS_128; j++) {
		buff_11.input[j](conv_11_out[j]);
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_11.output[j][k](buff_11_out[j][k]);
		}
	}
	buff_11.signal_in(signal_21);
	buff_11.signal_out(signal_22);
	buff_11.clock_1(clock_1);

	// convolution module
	stage_conv_12 Conv_12("conv_module_12");
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_12.input[j*INPUT_SIZE + k](buff_11_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		Conv_12.output[j](conv_12_out[j]);
	}
	Conv_12.signal_in(signal_22);
	Conv_12.signal_out(signal_23);

	// buffer 12
	conv_buffer_12 buff_12("stage_buffer_12");
	for (int j = 0; j < CHANNELS_128; j++) {
		buff_12.input[j](conv_12_out[j]);
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_12.output[j][k](buff_12_out[j][k]);
		}
	}
	buff_12.signal_in(signal_23);
	buff_12.signal_out(signal_24);
	buff_12.clock_1(clock_1);

	// convolution module
	stage_conv_13 Conv_13("conv_module_13");
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_13.input[j*INPUT_SIZE + k](buff_12_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		Conv_13.output[j](conv_13_out[j]);
	}
	Conv_13.signal_in(signal_24);
	Conv_13.signal_out(signal_25);

	// buffer 13
	conv_buffer_13 buff_13("stage_buffer_13");
	for (int j = 0; j < CHANNELS_128; j++) {
		buff_13.input[j](conv_13_out[j]);
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_13.output[j][k](buff_13_out[j][k]);
		}
	}
	buff_13.signal_in(signal_25);
	buff_13.signal_out(signal_26);
	buff_13.clock_1(clock_1);

	// convolution module
	stage_conv_14 Conv_14("conv_module_14");
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_14.input[j*INPUT_SIZE + k](buff_13_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		Conv_14.output[j](conv_14_out[j]);
	}
	Conv_14.signal_in(signal_26);
	Conv_14.signal_out(signal_27);

	// buffer 14
	conv_buffer_14 buff_14("stage_buffer_14");
	for (int j = 0; j < CHANNELS_128; j++) {
		buff_14.input[j](conv_14_out[j]);
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			buff_14.output[j][k](buff_14_out[j][k]);
		}
	}
	buff_14.signal_in(signal_27);
	buff_14.signal_out(signal_28);
	buff_14.clock_1(clock_1);

	// convolution module
	stage_conv_15 Conv_15("conv_module_15");
	for (int j = 0; j < CHANNELS_128; j++) {
		for (int k = 0; k < INPUT_SIZE; k++) {
			Conv_15.input[j*INPUT_SIZE + k](buff_14_out[j][k]);
		}
	}
	for (int j = 0; j < CHANNELS_128; j++) {
		Conv_15.output[j](conv_15_out[j]);
	}
	Conv_15.signal_in(signal_28);
	Conv_15.signal_out(signal_29);

	// linear buffer 1
	linear_buffer_15 linear_buff_1("linear_buffer_15");
	for (int j = 0; j < CHANNELS_128; j++){
		linear_buff_1.input[j](conv_15_out[j]);
	}
	for (int j = 0; j < INPUT_LINEAR_1; j++){
		linear_buff_1.output[j](buff_linear_1_out[j]);
	}
	linear_buff_1.signal_in(signal_29);
	linear_buff_1.signal_out(signal_30);
	linear_buff_1.clock_1(clock_1);

	// linear module
	stage_linear_16 linear_1("stage_linear_16");
	for (int j = 0; j < INPUT_LINEAR_1; j++){
		linear_1.input[j](buff_linear_1_out[j]);
	}
	for (int j = 0; j < INPUT_LINEAR_2; j++){
		linear_1.output[j](linear_1_out[j]);
	}
	linear_1.signal_in(signal_30);
	linear_1.signal_out(signal_31);

	// linear buffer 2
	linear_buffer_16 linear_buff_2("linear_buffer_16");
	for (int j = 0; j < INPUT_LINEAR_2; j++) {
		linear_buff_2.input[j](linear_1_out[j]);
	}
	for (int j = 0; j < INPUT_LINEAR_2; j++) {
		linear_buff_2.output[j](buff_linear_2_out[j]);
	}
	linear_buff_2.signal_in(signal_31);
	linear_buff_2.signal_out(signal_32);
	linear_buff_2.clock_1(clock_1);

	// linear module
	stage_linear_17 linear_2("stage_linear_17");
	for (int j = 0; j < INPUT_LINEAR_2; j++) {
		linear_2.input[j](buff_linear_2_out[j]);
	}
	for (int j = 0; j < OUTPUT_LINEAR; j++) {
		linear_2.output[j](linear_2_out[j]);
	}
	linear_2.signal_in(signal_32);
	linear_2.signal_out(signal_33);

	// display module
	display D("display_module");
	for (int j = 0; j < OUTPUT_LINEAR; j++){
		D.res[j](linear_2_out[j]);
	}
	D.signal_in(signal_33);
	D.signal_out(signal_34);

	// init simulation
	sc_start(0, SC_NS);

	// simulation start
	// clock.write(1);
	time_t start_time, end_time;
	start_time = time(NULL);
	for (int i = 0; i < PICTURE_NUM; i++){
		//clock.write(0);
		clock.write(1);
		sc_start(SIMULATE_DURATION, SC_NS);// for number generation
		clock.write(0);
		sc_start(SIMULATE_DURATION, SC_NS);
		//clock.write(1);
		for (int j = 0; j < IMAGE_SIZE_32*IMAGE_SIZE_32; j++) {
			clock_1.write(1); // run tiles
			sc_start(SIMULATE_DURATION, SC_NS);
			clock_1.write(0); // send data to next layer
			sc_start(SIMULATE_DURATION, SC_NS);
		}
		
	}
	end_time = time(NULL);
	cout << "Total time usage: " << (double)(end_time - start_time) << "s." << endl;
//	system("pause");
	return 0;
}
