#ifndef _LINEAR_BUFFER_15
#define _LINEAR_BUFFER_15

#include "systemc.h"
#include "config.h"

using namespace std;

SC_MODULE(linear_buffer_15) {
	// linear buffer 15
	sc_in<float> input[CHANNELS_128]; // in linear buffer 15, upper layer is conv layer and its channels is 128
	sc_out<float> output[INPUT_LINEAR_1];
	sc_in<int> signal_in;
	sc_in<bool> clock_1;
	sc_out<int> signal_out;

	float buffer[INPUT_LINEAR_1];
	int current;

	void init() {
		current = 0;
	}

	void send_to_next_layer() {
		if (current == INPUT_LINEAR_1) {
			for (int i = 0; i < INPUT_LINEAR_1; i++)
				output[i].write(buffer[i]);
			signal_out.write(signal_in.read());
			current = 0;
		}
	}

	void add_to_buffer() {
		for (int i = 0; i < CHANNELS_128; i++)
			buffer[current / CHANNELS_128 + i * INPUT_LINEAR_1 / CHANNELS_128] = input[i].read();
		current += CHANNELS_128;
	}

	SC_CTOR(linear_buffer_15) {
		init();

		SC_METHOD(add_to_buffer);
		sensitive << signal_in;
		dont_initialize();

		SC_METHOD(send_to_next_layer);
		sensitive << clock_1.neg();
		dont_initialize();
	}
};


#endif // !_LINEAR_BUFFER_15
