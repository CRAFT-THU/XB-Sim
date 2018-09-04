#ifndef _NUMGEN_H
#define _NUMGEN_H

#include <fstream>
#include <sstream>
#include <string>
#include "systemc.h"
#include "config.h"

using namespace std;

SC_MODULE(numgen) {
	// sc_in<int> number;
	sc_in<bool> clock;
	sc_in<bool> clock_1;
	sc_out<int> signal_out;
	sc_out<float> output[INPUT_SIZE*CHANNELS_3];

	float img_data[CHANNELS_3][IMAGE_SIZE_32][IMAGE_SIZE_32];
	
	// generate data
	void generate_data() {
		static int counter = 0; // picture number
		if (counter < PICTURE_NUM) {
			// open input file
			cout << "picture number: " << counter << endl;
			char filename[30] = { 0 };
			char num[5] = { 0 };
			strcpy_s(filename, "./input/x_");
			_itoa_s(counter, num, 10);
			strcat_s(filename, num);
			strcat_s(filename, ".csv");
			ifstream inFile_x(filename, ios::in);

			for (int s = 0; s < CHANNELS_3; s++){
				string lineStr_x;
				getline(inFile_x, lineStr_x); // read one channel data
				stringstream ss(lineStr_x);
				string str;
				for (int j = 0; j < IMAGE_SIZE_32; j++){
					for (int k = 0; k < IMAGE_SIZE_32; k++){
						// read data from file
						// img_data[s][j][k] = j * IMAGE_SIZE_32 + k;
						getline(ss, str, ',');
						istringstream iss(str);
						float num;
						iss >> num;
						img_data[s][j][k] = num;
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
		float tmp_data[CHANNELS_3][KERNEL_SIZE][KERNEL_SIZE] = { 0.0 };
		int tmp_x = 0;
		int tmp_y = 0;
		for (int i = x - 1; i < x + 2; i++) {
			if (i < 0 || i == IMAGE_SIZE_32) {
				tmp_x++;
				continue;
			}
			for (int j = y - 1; j < y + 2; j++) {
				if (j < 0 || j == IMAGE_SIZE_32) {
					tmp_y++;
					continue;
				}
				for (int k = 0; k < CHANNELS_3; k++) {
					tmp_data[k][tmp_x][tmp_y] = img_data[k][i][j];
				}
				tmp_y++;
			}
			tmp_x++;
			tmp_y = 0;
		}
		// first channel in first input_size position
		// second channel in second input_size position
		for (int k = 0; k < CHANNELS_3; k++){
			for (int i = 0; i < KERNEL_SIZE; i++) {
				for (int j = 0; j < KERNEL_SIZE; j++) {
					output[k*INPUT_SIZE + i*KERNEL_SIZE + j].write(tmp_data[k][i][j]);
				}
			}
		}
		
		signal_out.write(x * IMAGE_SIZE_32 + y + 1);
		y++;
		if (x == IMAGE_SIZE_32-1 && y == IMAGE_SIZE_32) {
			/*for (int j = 0; j < IMAGE_SIZE; j++){
				delete[] img_data[j];
			}
			delete[] img_data;*/
			x = 0;
			y = 0;
		}
		else if (y == IMAGE_SIZE_32) {
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
