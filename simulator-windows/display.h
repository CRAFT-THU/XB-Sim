#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "systemc.h"
#include "config.h"
#include <fstream>
#include <string>

using namespace std;

SC_MODULE(display) {
	sc_in<int> signal_in;
	sc_out<int> signal_out;
	sc_in<float> res[OUTPUT_LINEAR];

	int labels[PICTURE_NUM];
	int number;
	int total;
	void init() {
		number = 0;
		total = 0;
		string filename = "labels.csv";
		ifstream inFile_x(filename.c_str(), ios::in);
		for (int i = 0; i < PICTURE_NUM; i++){
			string line;
			getline(inFile_x, line);
			stringstream ss(line);
			ss >> labels[i];
		}
		inFile_x.close();
	}

	void print_out() {
		int counter = 0;
		float result = 0.0;
		float tmp_res[OUTPUT_LINEAR];
		for (int j = 0; j < OUTPUT_LINEAR; j++){
			tmp_res[j] = res[j].read();
		}
		for (int j = 0; j < OUTPUT_LINEAR; j++){
			if (tmp_res[j] > result){
				result = tmp_res[j];
				counter = j;
			}
		}
		if (counter == labels[number])
			total++;
		cout << "pic: " << number << " res: " << counter << " label: " << labels[number] << " total_right: " << total << endl;
		ofstream fout("result_1.txt", ios::app);
		fout << "pic: " << number << " res: " << counter << " label: " << labels[number] << " total_right: " << total << endl;
		fout.close();
		number++;
		signal_out.write(signal_in.read());
	}

	SC_CTOR(display) {
		init();

		SC_METHOD(print_out);
		sensitive << signal_in;
		dont_initialize();
	}

	~display() {

	}
};

#endif // !_DISPLAY_H
