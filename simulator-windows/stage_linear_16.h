#ifndef _STAGE_LINEAR_16
#define _STAGE_LINEAR_16

#include "crossbar.h"
#include "systemc.h"

using namespace std;

SC_MODULE(stage_linear_16) {
	sc_in<float> input[INPUT_LINEAR_1];
	sc_out<float> output[INPUT_LINEAR_2];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		string filename = "./weights/weight_16.csv";
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
		cout << "load weights 16 complete. "  << filename << endl;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < INPUT_LINEAR_2; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	// run matrix multiply
	void stage_linear_run() {
		// with ad & da
//		float input_buff[INPUT_LINEAR_1] = { 0.0 };
//		float _max = 0.0;
//		for (int i = 0; i < INPUT_LINEAR_1; ++i){
//			input_buff[i] = input[i].read();
//			if (input_buff[i] > _max)
//				_max = input_buff[i];
//		}
//
//		// for movement
//		int move = 0;
//		for (int i = 0; i < DA_WIDTH; ++i){
//			move += int(pow(2, double(i)));
//		}
//
//		// scale input
//		int n = 0;
//		while (pow(2, double(n)) < _max)
//			n++;
//		if (n > AD_WIDTH){
//			float para = pow(2, AD_WIDTH-n);
//			for (int i = 0; i < INPUT_LINEAR_1; ++i){
//				input_buff[i] = int(input_buff[i] * para);
//			}
//		}
//
//		// DA->XB->AD
//		da dac(DA_V);
//		ad adc(AD_V);
//		float ad_buff[CROSSBAR_W] = { 0.0 };
//		for (int i = 0; i < AD_WIDTH/DA_WIDTH; ++i){
//			float tmp_input[CROSSBAR_L] = { 0.0 };
//			float tmp_output[CROSSBAR_W] = { 0.0 };
//			// lower da_width bits
//			for (int j = 0; j < INPUT_LINEAR_1; ++j){
//				int bitnum = static_cast<int>(int(input_buff[j]) & move);
//				dac.trans(bitnum, DA_WIDTH);
//				tmp_input[CROSSBAR_L - INPUT_LINEAR_1 + j] = float(bitnum);
//				input_buff[j] = input_buff[j] / pow(2, DA_WIDTH);
//			}
//			cb.run(tmp_input, tmp_output, false);
//			// ad and shift add
//			for (int j = 0; j < CROSSBAR_W; ++j){
//				// float tmp = tmp_output[j] / XB16_I;
//				// if (tmp > 1)
//				// 	adc.trans(1.0);
//				// else {
//				// 	adc.trans(tmp);
//				// }
//				// ad_buff[j] = (adc.AD_out) * pow(2, i) + ad_buff[j];
//				ad_buff[j] = (tmp_output[j]) * pow(2, i) + ad_buff[j];
//			}
//		}
//
//		// for (int i = 0; i < INPUT_LINEAR_1; ++i)
//		// {
//		// 	if (ad_buff[i] >= pow(2, 13))
//		// 		ad_buff[i] = 255;
//		// 	else
//		// 		ad_buff[i] = (int(ad_buff[i] / 16.0) & 255);
//		// }
//		activation(ad_buff);
//		for (int i = 0; i < INPUT_LINEAR_2; i++) {
//			output[i].write(ad_buff[i]);
//		}
//		signal_out.write(signal_in.read());

//		 without ad & da
		 float tmp_input[CROSSBAR_L] = { 0.0 };
		 float tmp_output[CROSSBAR_W] = { 0.0 };
		 // read data from former layer
		 for (int i = 0; i < INPUT_LINEAR_1; i++) {
		 	tmp_input[CROSSBAR_L-INPUT_LINEAR_1+i] = input[i].read();
		 }
		 cb.run(tmp_input, tmp_output, false);
		 activation(tmp_output);
		 for (int i = 0; i < INPUT_LINEAR_2; i++) {
		 	output[i].write(tmp_output[i]);
		 }

		 signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_linear_16) {
		init_crossbar();

		SC_METHOD(stage_linear_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_linear_16() {
		cb.free_space();
	}
};

#endif // !_STAGE_LINEAR_16
