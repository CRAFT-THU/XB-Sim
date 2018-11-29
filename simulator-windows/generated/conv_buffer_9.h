#ifndef _CONV_BUFFER_9
#define _CONV_BUFFER_9

#include "systemc.h"
#include "config.h"

using namespace std;

// convolution buffer 9, input channels CHANNELS_80, output channels CHANNELS_80
// image size IMAGE_SIZE_16

SC_MODULE(conv_buffer_9) {
	sc_in<float> input[CHANNELS_80]; // for one conv crossbar
	sc_out<float> output[CHANNELS_80][INPUT_SIZE];
	sc_in<int> signal_in;
	sc_in<bool> clock_1; // for data transfer
	sc_out<int> signal_out;

	float buffer[CHANNELS_80][IMAGE_SIZE_16*KERNEL_SIZE];
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

		if (begin && ready_num > 0) {
			// for one conv crossbar
			for (int i = 0; i < CHANNELS_80; i++) {
				float tmp_data[KERNEL_SIZE][KERNEL_SIZE] = { 0.0 };

				if (total < IMAGE_SIZE_16) { // first row in picture, need padding
					int x = out_current / IMAGE_SIZE_16; // row number in buffer
					int y = out_current % IMAGE_SIZE_16; // column number in bufer
					for (int j = 1; j < KERNEL_SIZE; j++) {
						for (int k = 0; k < KERNEL_SIZE; k++) {
							// first column or last column in one image
							if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE_16))
								continue;
							tmp_data[j][k] = buffer[i][((x - 1 + j + KERNEL_SIZE) % KERNEL_SIZE)*IMAGE_SIZE_16 + (y - 1 + k)];
						}
					}
				}
				else if (total >= IMAGE_SIZE_16 * (IMAGE_SIZE_16 - 1)) { // last row in picture
					int x = out_current / IMAGE_SIZE_16; // row number in buffer
					int y = out_current % IMAGE_SIZE_16; // column number in bufer
					for (int j = 0; j < KERNEL_SIZE - 1; j++) {
						for (int k = 0; k < KERNEL_SIZE; k++) {
							if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE_16))
								continue;
							tmp_data[j][k] = buffer[i][((x - 1 + j + KERNEL_SIZE) % KERNEL_SIZE)*IMAGE_SIZE_16 + (y - 1 + k)];
						}
					}
				}
				else {
					int x = out_current / IMAGE_SIZE_16; // row number in buffer
					int y = out_current % IMAGE_SIZE_16; // column number in bufer
					for (int j = 0; j < KERNEL_SIZE; j++) {
						for (int k = 0; k < KERNEL_SIZE; k++) {
							if ((y - 1 + k < 0) || (y - 1 + k >= IMAGE_SIZE_16))
								continue;
							tmp_data[j][k] = buffer[i][((x - 1 + j + KERNEL_SIZE) % KERNEL_SIZE)*IMAGE_SIZE_16 + (y - 1 + k)];
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
			if (out_current >= IMAGE_SIZE_16 * KERNEL_SIZE) // exceed buffer size
				out_current = 0;
			if (total >= IMAGE_SIZE_16 * IMAGE_SIZE_16)
				// exceed image size, a new image stater
				total = 0;
		}
	}

	void add_to_buffer() {
		// put data in buffer
		for (int i = 0; i < CHANNELS_80; i++) {
			buffer[i][in_current] = input[i].read();
		}
		in_current++;
		// in current pointer equals to buffer size
		if (in_current == IMAGE_SIZE_16 * KERNEL_SIZE)
			in_current = 0;

		// detect whether send data to next layer or not
		if (in_current > IMAGE_SIZE_16 + 1)
			begin = true;
		if (begin)
			ready_num++; // for one conv crossbar
	}

	SC_CTOR(conv_buffer_9) {
		init();

		SC_METHOD(add_to_buffer);
		sensitive << signal_in;
		dont_initialize();

		SC_METHOD(send_to_next_layer);
		sensitive << clock_1.neg();
		dont_initialize();
	}
};


#endif // !_CONV_BUFFER_9
