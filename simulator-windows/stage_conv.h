#ifndef _STAGE_CONV
#define _STAGE_CONV

#include "crossbar.h"
#include "systemc.h"

using namespace std;

SC_MODULE(stage_conv) {
	sc_in<float> input[INPUT_SIZE];
	sc_out<float> output[CROSSBAR_W];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		for (int i = 0; i < CROSSBAR_L; i++){
			for (int j = 0; j < CROSSBAR_W; j++){
				cell[i*CROSSBAR_W + j] = i * CROSSBAR_W + j;
			}
		}
		cb.init(cell, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;
	}

	// run convolution
	void stage_conv_run() {
		/*for (int j = 0; j < INPUT_SIZE; j++) {
			output[j].write(input[j].read() * 2);
		}*/
		float tmp_input[CROSSBAR_L] = { 0.0 };
		float tmp_output[CROSSBAR_W] = { 0.0 };
		for (int i = 0; i < INPUT_SIZE; i++){
			tmp_input[i] = input[i].read();
		}
		cb.run(tmp_input, tmp_output);
		for (int i = 0; i < CROSSBAR_W; i++){
			output[i].write(tmp_output[i]);
		}
		signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_conv) {
		init_crossbar();

		SC_METHOD(stage_conv_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_conv() {
		cb.free_space();
	}
};

#endif // !_STAGE_CONV
