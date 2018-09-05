#ifndef _STAGE_CONV_14
#define _STAGE_CONV_14

#include "crossbar.h"
#include "systemc.h"

using namespace std;

// fourteenth convolution layer, input channel 128, output channel 128

SC_MODULE(stage_conv_14) {
	sc_in<float> input[INPUT_SIZE*CHANNELS_128];
	sc_out<float> output[CHANNELS_128];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		// read from convolution layer 14
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		char filename[35] = { 0 };
		strcpy_s(filename, "./weights/weight_13.csv");
		ifstream inFile_x(filename, ios::in);
		for (int i = 0; i < CROSSBAR_L; i++) {
			string lineStr_x;
			getline(inFile_x, lineStr_x); // read one row data
			stringstream ss(lineStr_x);
			string str;
			for (int j = 0; j < CROSSBAR_W; j++) {
				// cell[i*CROSSBAR_W + j] = i * CROSSBAR_W + j;
				getline(ss, str, ',');
				istringstream iss(str);
				float num;
				iss >> num;
				cell[i*CROSSBAR_W + j] = num;
			}
		}
		cb.init(cell, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;
		cout << "load weights 13 complete." << endl;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_128; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	// do maxpooling
	void max_pooling() {
		int pooling_size = 1;
	}

	// run convolution
	void stage_conv_run() {

		float tmp_input[CROSSBAR_L] = { 0.0 };
		float tmp_output[CROSSBAR_W] = { 0.0 };
		// read data from former layer
		for (int i = 0; i < INPUT_SIZE*CHANNELS_128; i++) {
			tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_128 + i] = input[i].read();
		}
		cb.run(tmp_input, tmp_output);
		activation(tmp_output);
		max_pooling(); // pooling size 1, do nothing
		for (int i = 0; i < CHANNELS_128; i++) {
			output[i].write(tmp_output[i]);
		}
		signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_conv_14) {
		init_crossbar();

		SC_METHOD(stage_conv_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_conv_14() {
		cb.free_space();
	}
};

#endif // !_STAGE_CONV_14
