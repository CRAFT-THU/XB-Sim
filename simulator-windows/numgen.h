#ifndef _NUMGEN_H
#define _NUMGEN_H

#include "systemc.h"
#include "config.h"

using namespace std;

SC_MODULE(numgen) {
	// sc_in<int> number;
	sc_in<bool> clock;
	sc_in<bool> clock_1;
	sc_out<int> signal_out;
	sc_out<float> output[INPUT_SIZE*CHANNELS];

	float img_data[IMAGE_SIZE][IMAGE_SIZE][CHANNELS];
	
	// generate data
	void generate_data() {
		static int counter = 0; // picture number
		if (counter < PICTURE_NUM) {
			//img_data = new float*[IMAGE_SIZE];
			for (int j = 0; j < IMAGE_SIZE; j++){
				// read data from file
				//img_data[j] = new float[IMAGE_SIZE];
				for (int k = 0; k < IMAGE_SIZE; k++){
					for (int s = 0; s < CHANNELS; s++) {
						img_data[j][k][s] = j * IMAGE_SIZE + k;
					}
				}
			}
			counter++;
		} 
	}

	// transport data to next layer
	void send_data() {
		static int x = 0;
		static int y = 0;
		float tmp_data[KERNEL_SIZE][KERNEL_SIZE][CHANNELS] = { 0.0 };
		int tmp_x = 0;
		int tmp_y = 0;
		for (int i = x - 1; i < x + 2; i++) {
			if (i < 0 || i == IMAGE_SIZE) {
				tmp_x++;
				continue;
			}
			for (int j = y - 1; j < y + 2; j++) {
				if (j < 0 || j == IMAGE_SIZE) {
					tmp_y++;
					continue;
				}
				for (int k = 0; k < CHANNELS; k++) {
					tmp_data[tmp_x][tmp_y][k] = img_data[i][j][k];
				}
				tmp_y++;
			}
			tmp_x++;
			tmp_y = 0;
		}
		// first channel in first input_size position
		// second channel in second input_size position
		for (int k = 0; k < CHANNELS; k++){
			for (int i = 0; i < KERNEL_SIZE; i++) {
				for (int j = 0; j < KERNEL_SIZE; j++) {
					output[k*INPUT_SIZE + i*KERNEL_SIZE + j].write(tmp_data[i][j][k]);
				}
			}
		}
		
		signal_out.write(x * IMAGE_SIZE + y + 1);
		y++;
		if (x == IMAGE_SIZE-1 && y == IMAGE_SIZE) {
			/*for (int j = 0; j < IMAGE_SIZE; j++){
				delete[] img_data[j];
			}
			delete[] img_data;*/
			x = 0;
			y = 0;
		}
		else if (y == IMAGE_SIZE) {
			x++;
			y = 0;
		}
	}
	// constructor
	SC_CTOR(numgen) {
		SC_METHOD(generate_data);
		sensitive << clock.pos();
		dont_initialize();
		SC_METHOD(send_data);
		sensitive << clock_1.pos();
		dont_initialize();
	}
};

#endif // !_NUMGEN_H
