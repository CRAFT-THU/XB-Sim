#include "systemc.h"
#include "config.h"
#include "numgen.h"
#include "stage_conv.h"
#include "stage_buffer.h"
#include "display.h"

int sc_main(int argc, char* argv[]) {
	// define signals
	sc_signal<bool> clock; // clock (could be replaced by sc_clock?)
	sc_signal<bool> clock_1;
	sc_signal<int> number; // number of input picture
	sc_signal<int> signal_0; // signals for modules
	sc_signal<int> signal_1;
	sc_signal<int> signal_2;
	// int signal_counter = 0;

	sc_signal<float> buffer[INPUT_SIZE];
	sc_signal<float> conv_out[CROSSBAR_W];
	
	// number generator
	numgen Num_gen("number_generator");
	for (int j = 0; j < INPUT_SIZE; j++){
		Num_gen.output[j](buffer[j]);
	}
	Num_gen.clock(clock);
	Num_gen.clock_1(clock_1);
	Num_gen.signal_out(signal_0);

	// convolution module
	stage_conv Conv("conv_module");
	for (int j = 0; j < INPUT_SIZE; j++) {
		Conv.input[j](buffer[j]);
	}
	for (int j = 0; j < CROSSBAR_W; j++) {
		Conv.output[j](conv_out[j]);
	}
	Conv.signal_in(signal_0);
	Conv.signal_out(signal_1);

	// buffer
	stage_buffer buff("stage_buffer");

	// display module
	display D("display_module");
	for (int j = 0; j < CROSSBAR_W; j++){
		D.res[j](conv_out[j]);
	}
	D.signal_in(signal_1);
	D.signal_out(signal_2);

	// init simulation
	sc_start(0, SC_NS);

	// simulation start
	// clock.write(1);
	for (int i = 0; i < PICTURE_NUM; i++){
		//clock.write(0);
		clock.write(1);
		sc_start(SIMULATE_DURATION, SC_NS);// for number generation
		clock.write(0);
		sc_start(SIMULATE_DURATION, SC_NS);
		//clock.write(1);
		for (int j = 0; j < IMAGE_SIZE*IMAGE_SIZE; j++) {
			clock_1.write(1);
			sc_start(SIMULATE_DURATION, SC_NS);
			clock_1.write(0);
			sc_start(SIMULATE_DURATION, SC_NS);
		}
		
	}
	system("pause");
	return 0;
}