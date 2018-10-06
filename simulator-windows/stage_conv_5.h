/// automatic generate file

#ifndef _STAGE_CONV_5
#define _STAGE_CONV_5

#include "crossbar.h"
#include "systemc.h"
#include "ADC.h"
#include "DAC.h"

using namespace std;

// fifth convolution layer, input channel CHANNELS_48, output channel CHANNELS_48, pooling POOLING_SIZE_2

// #define POOLING_SIZE POOLING_SIZE_2

SC_MODULE(stage_conv_5) {
	sc_in<float> input[INPUT_SIZE*CHANNELS_48];
	sc_out<float> output[CHANNELS_48];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;
	float pooling_buffer[CHANNELS_48][IMAGE_SIZE_32*POOLING_SIZE_2];
	int pooling_pointer;

	// read crossbar data from file
	void init_crossbar() {
		// read from convolution layer 5
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		string filename = "./weights/weight_5.csv";
		ifstream inFile_x(filename.c_str(), ios::in);
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
		cout << "load weights 5 complete. " << filename << endl;

		// parameters initialize
		pooling_pointer = 0;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_48; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	void add_to_pooling_buffer(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_48; i++){
			pooling_buffer[i][pooling_pointer] = tmp_input[i];
		}
	}

	// do maxpooling
	void max_pooling() {
		float tmp_output[CHANNELS_48];
		int x = pooling_pointer / IMAGE_SIZE_32;
		int y = pooling_pointer % IMAGE_SIZE_32;

		if ((x % POOLING_SIZE_2 == (POOLING_SIZE_2 - 1)) && (y % POOLING_SIZE_2 == (POOLING_SIZE_2 - 1))) {
			for (int i = 0; i < CHANNELS_48; i++) {
				float _max = 0.0; // can not be smaller than 0.0 after relu
				for (int j = 0; j < POOLING_SIZE_2; j++){
					for (int k = 0; k < POOLING_SIZE_2; k++){
						float element = pooling_buffer[i][(x - j) % POOLING_SIZE_2*IMAGE_SIZE_32 + (y - k)];
						if (element > _max)
							_max = element;
					}
				}
				tmp_output[i] = _max;
			}

			// send to next layer (next buffer)
			for (int i = 0; i < CHANNELS_48; i++) {
				output[i].write(tmp_output[i]);
			}
			signal_out.write(signal_in.read());
		}
		pooling_pointer++;
		if (pooling_pointer >= IMAGE_SIZE_32 * POOLING_SIZE_2)
			pooling_pointer = 0;
	}

	// run convolution
	void stage_conv_run() {
		// read data
		int input_buff[INPUT_SIZE*CHANNELS_48] = { 0 };
		float _max = 0.0;
		for (int i = 0; i < INPUT_SIZE*CHANNELS_48; ++i){
			input_buff[i] = int(input[i].read());
			if (input_buff[i] > _max)
				_max = input_buff[i];
		}

		// for movement
		int move = 0;
		for (int i = 0; i < DA_WIDTH; ++i){
			move += int(pow(2, double(i)));
		}

		// scale input
		int n = 0;
		while (pow(2, double(n)) < _max)
			n++;
		if (n > AD_WIDTH){
			float para = pow(2, AD_WIDTH-n);
			for (int i = 0; i < INPUT_SIZE*CHANNELS_48; ++i){
				input_buff[i] /= para;
			}
		}

		// DA->XB->AD
		da dac(DA_V);
		ad adc(AD_V);
		float ad_buff[CROSSBAR_W] = { 0.0 };
		for (int i = 0; i < AD_WIDTH/DA_WIDTH; ++i){
			float tmp_input[CROSSBAR_L] = { 0.0 };
			float tmp_output[CROSSBAR_W] = { 0.0 };
			// lower da_width bits
			for (int j = 0; j < INPUT_SIZE*CHANNELS_48; ++j){
				int bitnum = static_cast<int>(input_buff[j] & move);
				dac.trans(bitnum, DA_WIDTH);
				tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_48 + j] = float(bitnum);
				input_buff[j] >>= DA_WIDTH;
			}
			cb.run(tmp_input, tmp_output);
			// ad and shift add
			for (int j = 0; j < CROSSBAR_W; ++j){
				adc.trans(tmp_output[j]);
				float tmp = adc.AD_out ; // / XB5_I; // divide by xb_i?
				tmp = (tmp > 0)? floor(tmp+0.5): ceil(tmp-0.5);
				ad_buff[j] = tmp + 2*ad_buff[j];
				ad_buff[j] = (ad_buff[j] > 0)? ad_buff[j]: 0;
			}
		}

		// float tmp_input[CROSSBAR_L] = { 0.0 };
		// float tmp_output[CROSSBAR_W] = { 0.0 };
		// // read data from former layer
		// for (int i = 0; i < INPUT_SIZE*CHANNELS_48; i++) {
		// 	tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_48 + i] = input_buff[i];
		// }
		// cb.run(tmp_input, tmp_output);
		activation(ad_buff);
		add_to_pooling_buffer(ad_buff);

		max_pooling(); // pooling size POOLING_SIZE_2
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
