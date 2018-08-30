#ifndef _STAGE_CONV_5
#define _STAGE_CONV_5

#include "crossbar.h"
#include "systemc.h"

using namespace std;

// fifth convolution layer, input channel 48, output channel 48, pooling 2

SC_MODULE(stage_conv_5) {
	sc_in<float> input[INPUT_SIZE*CHANNELS_32];
	sc_out<float> output[CHANNELS_32];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		// read from convolution layer 5
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		for (int i = 0; i < CROSSBAR_L; i++) {
			for (int j = 0; j < CROSSBAR_W; j++) {
				cell[i*CROSSBAR_W + j] = i * CROSSBAR_W + j;
			}
		}
		cb.init(cell, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_48; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	// do maxpooling
	void max_pooling() {
		int pooling_size = 2;
	}

	// run convolution
	void stage_conv_run() {

		float tmp_input[CROSSBAR_L] = { 0.0 };
		float tmp_output[CROSSBAR_W] = { 0.0 };
		// read data from former layer
		for (int i = 0; i < INPUT_SIZE*CHANNELS_48; i++) {
			tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_48 + i] = input[i].read();
		}
		cb.run(tmp_input, tmp_output);
		activation(tmp_output);

		max_pooling(); // pooling size 2
		
		for (int i = 0; i < CHANNELS_48; i++) {
			output[i].write(tmp_output[i]);
		}
		signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_conv_5) {
		init_crossbar();

		SC_METHOD(stage_conv_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_conv_5() {
		cb.free_space();
	}
};

#endif // !_STAGE_CONV_5
