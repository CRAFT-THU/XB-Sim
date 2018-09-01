#ifndef _STAGE_CONV_10
#define _STAGE_CONV_10

#include "crossbar.h"
#include "systemc.h"

using namespace std;

// tenth convolution layer, input channel 80, output channel 80

#define POOLING_SIZE 2

SC_MODULE(stage_conv_10) {
	sc_in<float> input[INPUT_SIZE*CHANNELS_80];
	sc_out<float> output[CHANNELS_80];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;
	float pooling_buffer[CHANNELS_80][IMAGE_SIZE_16*POOLING_SIZE];
	int pooling_pointer;

	// read crossbar data from file
	void init_crossbar() {
		// read from convolution layer 10
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		for (int i = 0; i < CROSSBAR_L; i++) {
			for (int j = 0; j < CROSSBAR_W; j++) {
				cell[i*CROSSBAR_W + j] = i * CROSSBAR_W + j;
			}
		}
		cb.init(cell, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;

		pooling_pointer = 0;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_80; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	void add_to_pooling_buffer(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_80; i++) {
			pooling_buffer[i][pooling_pointer] = tmp_input[i];
		}
	}

	// do maxpooling
	void max_pooling() {
		float tmp_output[CHANNELS_80];
		int x = pooling_pointer / IMAGE_SIZE_16;
		int y = pooling_pointer % IMAGE_SIZE_16;

		if ((x % POOLING_SIZE == (POOLING_SIZE - 1)) && (y % POOLING_SIZE == (POOLING_SIZE - 1))) {
			for (int i = 0; i < CHANNELS_80; i++) {
				float _max = 0.0; // can not be smaller than 0.0 after relu
				for (int j = 0; j < POOLING_SIZE; j++) {
					for (int k = 0; k < POOLING_SIZE; k++) {
						float element = pooling_buffer[i][(x - j) % POOLING_SIZE*IMAGE_SIZE_16 + (y - k)];
						if (element > _max)
							_max = element;
					}
				}
				tmp_output[i] = _max;
			}

			// send to next layer (next buffer)
			for (int i = 0; i < CHANNELS_80; i++) {
				output[i].write(tmp_output[i]);
			}
			signal_out.write(signal_in.read());
		}
		pooling_pointer++;
		if (pooling_pointer >= IMAGE_SIZE_16 * POOLING_SIZE)
			pooling_pointer = 0;
	}

	// run convolution
	void stage_conv_run() {

		float tmp_input[CROSSBAR_L] = { 0.0 };
		float tmp_output[CROSSBAR_W] = { 0.0 };
		// read data from former layer
		for (int i = 0; i < INPUT_SIZE*CHANNELS_80; i++) {
			tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_80 + i] = input[i].read();
		}
		cb.run(tmp_input, tmp_output);
		activation(tmp_output);
		add_to_pooling_buffer(tmp_output);

		max_pooling(); // pooling size 2
	}

	SC_CTOR(stage_conv_10) {
		init_crossbar();

		SC_METHOD(stage_conv_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_conv_10() {
		cb.free_space();
	}
};

#endif // !_STAGE_CONV_10
