#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "systemc.h"
#include "config.h"
#include <fstream>

using namespace std;

SC_MODULE(display) {
	sc_in<int> signal_in;
	sc_out<int> signal_out;
	sc_in<float> res[CROSSBAR_W];

	void print_out() {
		static int counter = 0;
		cout << "this is display function" << endl;
		ofstream fout("display.out", ios::out|ios::app);
		for (int j = 0; j < CROSSBAR_W; j++){
			fout << res[j].read() << ' ';
		}
		cout << counter << endl;
		fout << counter << endl;
		fout.close();
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
