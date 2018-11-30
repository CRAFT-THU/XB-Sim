#ifndef _STAGE_LINEAR_17
#define _STAGE_LINEAR_17

#include "../crossbar.h"
#include "systemc.h"
#include "../ADC.h"
#include "../DAC.h"
#include <omp.h>

using namespace std;

SC_MODULE(stage_linear_17) {
	sc_in<float> input[INPUT_LINEAR_2];
	sc_out<float> output[OUTPUT_LINEAR];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		string filename = "./weights/weight_17.csv";
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
		cout << "load weights 17 complete. "  << filename << endl;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < OUTPUT_LINEAR; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	// run matrix multiply
	void stage_linear_run() {
		// with ad & da
		float input_buff[INPUT_LINEAR_2] = { 0.0 };
		float _max = 0.0;
		for (int i = 0; i < INPUT_LINEAR_2; ++i){
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
//		while (pow(2, double(n)) < _max)
//			n++;
        if (17 == 1)
            n = 8;
        else n = 14;
		if (n > AD_WIDTH){
			float para = pow(2, AD_WIDTH-n);
			for (int i = 0; i < INPUT_LINEAR_2; ++i){
				input_buff[i] = int(input_buff[i] * para);
                if (input_buff[i] > 255)
                    input_buff[i] = 255;
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
            for (int j = 0; j < INPUT_LINEAR_2; ++j){
                int bitnum = static_cast<int>(int(input_buff[j]) & move);
                dac.trans(bitnum, DA_WIDTH);
                tmp_input[CROSSBAR_L - INPUT_LINEAR_2 + j] = float(bitnum);
                input_buff[j] = input_buff[j] / pow(2, DA_WIDTH);
            }
            cb.run(tmp_input, tmp_output, false);
            // ad and shift add
            for (int j = 0; j < CROSSBAR_W; ++j){
                float tmp = tmp_output[j] / XB17_I;
                if (tmp > 1)
                	adc.trans(1.0);
                else if (tmp < -1) 
                	adc.trans(-1.0);
                else 
                	adc.trans(tmp);
                int tmp_ad = int(adc.AD_out);
                // if (tmp_ad > 0)
                // 	tmp_ad = floor(tmp_ad + 0.5);
                // else tmp_ad = ceil(tmp_ad - 0.5);
                ad_buff[j] = (tmp_ad) * pow(2, i) + ad_buff[j];
                // ad_buff[j] = (ad_buff[j] > 0) ? ad_buff[j]: 0;
                // ad_buff[j] = (tmp_output[j]) * pow(2, i) + ad_buff[j];
            }
        }

//		float ad_buff[CROSSBAR_W] = { 0.0 };
//		int split_width = AD_WIDTH/DA_WIDTH;
//		float tmp_input[CROSSBAR_L*AD_WIDTH/DA_WIDTH] = { 0.0 };
//		float tmp_output[CROSSBAR_W*AD_WIDTH/DA_WIDTH] = { 0.0 };
//		for (int i = 0; i < split_width; ++i){
//			// lower da_width bits
//			int j = 0;
////#pragma omp parallel for private(j) shared(i, tmp_input, input_buff, move)
//			for (j = 0; j < INPUT_LINEAR_2; ++j){
//				int bitnum = static_cast<int>(int(input_buff[j]) & move);
//				dac.trans(bitnum, DA_WIDTH);
//				tmp_input[CROSSBAR_L * i + CROSSBAR_L - INPUT_LINEAR_2 + j] = float(bitnum);
//				input_buff[j] = input_buff[j] / pow(2, DA_WIDTH);
//			}
//		}
//		cb.run(tmp_input, tmp_output, split_width, false);
//		// ad and shift add
//		for (int i = 0; i < split_width; ++i){
//			int j = 0;
////#pragma omp parallel for private(j) shared(i, tmp_output, ad_buff)
//			for (j = 0; j < CROSSBAR_W; ++j){
//				// float tmp = tmp_output[j] / XB17_I;
//				// if (tmp > 1)
//				// 	adc.trans(1.0);
//				// else {
//				// 	adc.trans(tmp);
//				// }
//				// ad_buff[j] = (adc.AD_out) * pow(2, i) + ad_buff[j];
//				ad_buff[j] = (tmp_output[i * CROSSBAR_W + j]) * pow(2, i) + ad_buff[j];
//			}
//		}

		// for (int i = 0; i < INPUT_LINEAR_2; ++i)
		// {
		// 	if (ad_buff[i] >= pow(2, 13))
		// 		ad_buff[i] = 255;
		// 	else
		// 		ad_buff[i] = (int(ad_buff[i] / 16.0) & 255);
		// }
		activation(ad_buff);
		for (int i = 0; i < OUTPUT_LINEAR; i++) {
			output[i].write(ad_buff[i]);
		}
		signal_out.write(signal_in.read());

//		 without ad & da
		// float tmp_input[CROSSBAR_L] = { 0.0 };
		// float tmp_output[CROSSBAR_W] = { 0.0 };
		// // read data from former layer
		// for (int i = 0; i < INPUT_LINEAR_2; i++) {
		// 	tmp_input[CROSSBAR_L-INPUT_LINEAR_2+i] = input[i].read();
		// }
		// cb.run(tmp_input, tmp_output, false);
		// activation(tmp_output);
		// for (int i = 0; i < OUTPUT_LINEAR; i++) {
		// 	output[i].write(tmp_output[i]);
		// }

		// signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_linear_17) {
		init_crossbar();

		SC_METHOD(stage_linear_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_linear_17() {
		cb.free_space();
	}
};

#endif // !_STAGE_LINEAR_17
