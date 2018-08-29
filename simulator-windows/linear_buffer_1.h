#ifndef _LINEAR_BUFFER_1
#define _LINEAR_BUFFER_1

#include "systemc.h"
#include "config.h"

#define LAST_CHANNELS 128

using namespace std;

SC_MODULE(linear_buffer_1) {
	// linear buffer 1
	sc_in<float> input[LAST_CHANNELS]; // in linear buffer 1, upper layer is conv layer and its channels is 128
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
		for (int i = 0; i < LAST_CHANNELS; i++)
			buffer[current / LAST_CHANNELS + i * INPUT_LINEAR_1 / LAST_CHANNELS] = input[i].read();
		current += LAST_CHANNELS;
	}

	SC_CTOR(linear_buffer_1) {
		init();

		SC_METHOD(add_to_buffer);
		sensitive << signal_in;
		dont_initialize();

		SC_METHOD(send_to_next_layer);
		sensitive << clock_1.neg();
		dont_initialize();
	}
};


#endif // !_LINEAR_BUFFER_1
