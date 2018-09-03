#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "systemc.h"
#include "config.h"
#include <fstream>

using namespace std;

SC_MODULE(display) {
	sc_in<int> signal_in;
	sc_out<int> signal_out;
	sc_in<float> res[OUTPUT_LINEAR];

	void print_out() {
		int counter = 0;
		float result = 0.0;
		for (int j = 0; j < OUTPUT_LINEAR; j++){
			if (res[j].read() > result){
				result = res[j].read();
				counter = j;
			}
		}
		cout << counter << endl;
		counter++;
		signal_out.write(signal_in.read());
	}

	SC_CTOR(display) {
		SC_METHOD(print_out);
		sensitive << signal_in;
		dont_initialize();
	}

	~display() {

	}
};

#endif // !_DISPLAY_H
