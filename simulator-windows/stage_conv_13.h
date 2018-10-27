/// automatic generate file

#ifndef _STAGE_CONV_13
#define _STAGE_CONV_13

#include "crossbar.h"
#include "systemc.h"
#include "ADC.h"
#include "DAC.h"
#include <omp.h>

using namespace std;

// fifth convolution layer, input channel CHANNELS_128, output channel CHANNELS_128, pooling POOLING_SIZE_1

// #define POOLING_SIZE POOLING_SIZE_1

SC_MODULE(stage_conv_13) {
	sc_in<float> input[INPUT_SIZE*CHANNELS_128];
	sc_out<float> output[CHANNELS_128];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;
	float pooling_buffer[CHANNELS_128][IMAGE_SIZE_8*POOLING_SIZE_1];
	int pooling_pointer;
	int print;

	// read crossbar data from file
	void init_crossbar() {
		// read from convolution layer 13
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		string filename = "./weights/weight_13.csv";
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
		cb.init(cell, 1, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;
		cout << "load weights 13 complete. " << filename << endl;
		// cb.printcrossbar();
		// parameters initialize
		pooling_pointer = 0;
		print = 0;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_128; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	void add_to_pooling_buffer(float tmp_input[]) {
		for (int i = 0; i < CHANNELS_128; i++){
			pooling_buffer[i][pooling_pointer] = tmp_input[i];
		}
	}

	// do maxpooling
	void max_pooling() {
		float tmp_output[CHANNELS_128];
		int x = pooling_pointer / IMAGE_SIZE_8;
		int y = pooling_pointer % IMAGE_SIZE_8;

		if ((x % POOLING_SIZE_1 == (POOLING_SIZE_1 - 1)) && (y % POOLING_SIZE_1 == (POOLING_SIZE_1 - 1))) {
			for (int i = 0; i < CHANNELS_128; i++) {
				float _max = 0.0; // can not be smaller than 0.0 after relu
				for (int j = 0; j < POOLING_SIZE_1; j++){
					for (int k = 0; k < POOLING_SIZE_1; k++){
						float element = pooling_buffer[i][(x - j) % POOLING_SIZE_1*IMAGE_SIZE_8 + (y - k)];
						if (element > _max)
							_max = element;
					}
				}
				tmp_output[i] = _max;
			}

			// send to next layer (next buffer)
			for (int i = 0; i < CHANNELS_128; i++) {
				output[i].write(tmp_output[i]);
			}
			signal_out.write(signal_in.read());
		}
		pooling_pointer++;
		if (pooling_pointer >= IMAGE_SIZE_8 * POOLING_SIZE_1)
			pooling_pointer = 0;
	}

	// run convolution
	void stage_conv_run() {
		// with ad & da
		// read data
		float input_buff[INPUT_SIZE*CHANNELS_128] = { 0.0 };
		float _max = 0.0;
		for (int i = 0; i < INPUT_SIZE*CHANNELS_128; ++i){
			input_buff[i] = input[i].read();
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
			for (int i = 0; i < INPUT_SIZE*CHANNELS_128; ++i){
				input_buff[i] = int(input_buff[i] * para);
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
            for (int j = 0; j < INPUT_SIZE*CHANNELS_128; ++j){
                int bitnum = static_cast<int>(int(input_buff[j]) & move);
                dac.trans(bitnum, DA_WIDTH);
                tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_128 + j] = float(bitnum);
                input_buff[j] = input_buff[j] / pow(2, DA_WIDTH);
            }
            cb.run(tmp_input, tmp_output, false);
            // ad and shift add
            for (int j = 0; j < CROSSBAR_W; ++j){
                // float tmp = tmp_output[j] / XB13_I;
                // if (tmp > 1)
                // 	adc.trans(1.0);
                // else {
                // 	adc.trans(tmp);
                // }
                // ad_buff[j] = (adc.AD_out) * pow(2, i) + ad_buff[j];
                ad_buff[j] = (tmp_output[j]) * pow(2, i) + ad_buff[j];
            }
        }
//		float ad_buff[CROSSBAR_W] = { 0.0 };
//		int split_width = AD_WIDTH/DA_WIDTH;
//		float tmp_input[CROSSBAR_L*AD_WIDTH/DA_WIDTH] = { 0.0 };
//		float tmp_output[CROSSBAR_W*AD_WIDTH/DA_WIDTH] = { 0.0 };
//		for (int i = 0; i < split_width; ++i){
//			// lower da_width bits
//			int j = 0;
//#pragma omp parallel for private(j) shared(i, tmp_input, input_buff, move)
//			for (j = 0; j < INPUT_SIZE*CHANNELS_128; ++j){
//				int bitnum = static_cast<int>(int(input_buff[j]) & move);
//				dac.trans(bitnum, DA_WIDTH);
//				tmp_input[CROSSBAR_L * i + CROSSBAR_L - INPUT_SIZE * CHANNELS_128 + j] = float(bitnum);
//				input_buff[j] = input_buff[j] / pow(2, DA_WIDTH);
//			}
//		}
//		cb.run(tmp_input, tmp_output, split_width, false);
//		// ad and shift add
//		for (int i = 0; i < split_width; ++i){
//			int j = 0;
//#pragma omp parallel for private(j) shared(i, tmp_output, ad_buff)
//			for (int j = 0; j < CROSSBAR_W; ++j){
//				// float tmp = tmp_output[j] / XB13_I;
//				// if (tmp > 1)
//				// 	adc.trans(1.0);
//				// else {
//				// 	adc.trans(tmp);
//				// }
//				// ad_buff[j] = (adc.AD_out) * pow(2, i) + ad_buff[j];
//				ad_buff[j] = (tmp_output[i * CROSSBAR_W + j]) * pow(2, i) + ad_buff[j];
//			}
//		}
		activation(ad_buff);
		add_to_pooling_buffer(ad_buff);

		max_pooling(); // pooling size POOLING_SIZE_1
		
		// for (int i = 0; i < INPUT_SIZE*CHANNELS_128; ++i)
		// {
		// 	if (ad_buff[i] >= pow(2, 13))
		// 		ad_buff[i] = 255;
		// 	else
		// 		ad_buff[i] = (int(ad_buff[i] / 16.0) & 255);
		// }

		// without ad & da
	 	// float tmp_input[CROSSBAR_L] = { 0.0 };
	 	// float tmp_output[CROSSBAR_W] = { 0.0 };
	 	// // read data from former layer
	 	// for (int i = 0; i < INPUT_SIZE*CHANNELS_128; i++) {
	 	// 	tmp_input[CROSSBAR_L - INPUT_SIZE * CHANNELS_128 + i] = input[i].read();
	 	// }
	 	// cb.run(tmp_input, tmp_output, false);
	 	// activation(tmp_output);
	 	// add_to_pooling_buffer(tmp_output);

	 	// max_pooling(); // pooling size POOLING_SIZE_1
	}

	SC_CTOR(stage_conv_13) {
		init_crossbar();

		SC_METHOD(stage_conv_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_conv_13() {
		cb.free_space();
	}
};

#endif // !_STAGE_CONV_13
