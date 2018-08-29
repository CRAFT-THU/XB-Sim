#ifndef _STAGE_BUFFER
#define _STAGE_BUFFER

#include "systemc.h"
#include "config.h"

using namespace std;

SC_MODULE(stage_buffer) {
	sc_in<float> input[CHANNELS]; // for one conv crossbar
	sc_out<float> output[CHANNELS][INPUT_SIZE];
	sc_in<int> signal_in;
	sc_in<bool> clock_1;
	sc_out<int> signal_out;

	float buffer[CHANNELS][IMAGE_SIZE*KERNEL_SIZE];
	int in_current; // input pointer in buffer
	int out_current; // output pointer in buffer
	int total; // output position in one image
	int ready_num; // ready number for data sending
	bool begin; // begin to send data

	void init() {
		in_current = 0, out_current = 0;
		total = 0;
		begin = false;
		ready_num = 0;
	}

	void send_to_next_layer() {

		// for one conv crossbar
		for (int i = 0; i < CHANNELS; i++) {
			float tmp_data[KERNEL_SIZE][KERNEL_SIZE] = { 0.0 };
			 
			if (total < IMAGE_SIZE) { // first row in picture, need padding
				int x = out_current / IMAGE_SIZE; // row number in buffer
				int y = out_current % IMAGE_SIZE; // column number in bufer
				for (int j = 1; j < KERNEL_SIZE; j++){
					for (int k = 0; k < KERNEL_SIZE; k++){
						if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE))
							continue;
						tmp_data[j][k] = buffer[i][((x - 1 + j) % KERNEL_SIZE)*IMAGE_SIZE + (y - 1 + k)];
					}
				}
			}
			else if (total >= IMAGE_SIZE * (IMAGE_SIZE - 1)) { // last row in picture
				int x = out_current / IMAGE_SIZE; // row number in buffer
				int y = out_current % IMAGE_SIZE; // column number in bufer
				for (int j = 0; j < KERNEL_SIZE - 1; j++) {
					for (int k = 0; k < KERNEL_SIZE; k++) {
						if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE))
							continue;
						tmp_data[j][k] = buffer[i][((x - 1 + j) % KERNEL_SIZE)*IMAGE_SIZE + (y - 1 + k)];
					}
				}
			}
			else {
				int x = out_current / IMAGE_SIZE; // row number in buffer
				int y = out_current % IMAGE_SIZE; // column number in bufer
				for (int j = 0; j < KERNEL_SIZE; j++) {
					for (int k = 0; k < KERNEL_SIZE; k++) {
						if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE))
							continue;
						tmp_data[j][k] = buffer[i][((x - 1 + j) % KERNEL_SIZE)*IMAGE_SIZE + (y - 1 + k)];
					}
				}
			}
			// write to signal
			for (int k = 0; k < KERNEL_SIZE; k++)
				for (int j = 0; j < KERNEL_SIZE; j++)
					output[i][k*KERNEL_SIZE + j].write(tmp_data[k][j]);
		}

		signal_out.write(signal_in.read());

		total++;
		out_current++;
		ready_num--;
		if (out_current >= IMAGE_SIZE * KERNEL_SIZE) // exceed buffer size
			out_current = 0;
		if (total >= IMAGE_SIZE * IMAGE_SIZE) // exceed image size
			total = 0;
	}

	void add_to_buffer() {
		if ((in_current % IMAGE_SIZE == 0) && (in_current != 0)){
			// do something
		}
		else {
			// put data in buffer
			// for one conv corssbar
			for (int i = 0; i < CHANNELS; i++){
				buffer[i][in_current] = input[i].read();
			}
			in_current++;
		}

		// detect whether send data to next layer or not
		if (in_current > IMAGE_SIZE + 1)
			begin = true;
		if (begin)
			ready_num++; // for one conv crossbar
		if (begin && ready_num > 0)
			send_to_next_layer();
	}

	SC_CTOR(stage_buffer) {
		init();

		SC_METHOD(add_to_buffer);
		sensitive << signal_in;
		dont_initialize();

		/*SC_METHOD(send_to_next_layer);
		sensitive << clock_1.neg();
		dont_initialize();*/
	}
};


#endif // !_STAGE_BUFFER
