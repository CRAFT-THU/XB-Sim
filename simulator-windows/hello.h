#ifndef _HELLO_H
#define _HELLO_H
#include "systemc.h"

SC_MODULE(hello)
{
	sc_in<bool> clock;
	void run()
	{
		cout << "@" << sc_simulation_time << " hello world" << endl;
	}
	SC_CTOR(hello)
	{
		SC_METHOD(run);
		sensitive << clock.pos();
	}
};

#endif // !_HELLO_H