#ifndef _CROSSBAR_H
#define _CROSSBAR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "config.h"

typedef struct Crossbar
{
	float *std_d;
	int CB_l;
	int CB_w;
	int CB_n;
	float *CB_cell;
	float *CB_std;
	void init(float *CB_cells, int n, int l, int w)
	{
		CB_l = l;
		CB_w = w;
		CB_n = n;
		CB_cell = new float[CB_l*CB_w];
//		memcpy(CB_cell, CB_cells, CB_l*CB_w * sizeof(float));
		// transform cb_cell
		for (int i = 0; i < CB_w; i++){
			for (int j = 0; j < CB_l; j++){
				CB_cell[i*CB_l + j] = CB_cells[j*CB_w + i];
			}
		}
		get_std();
	}

	void get_std(){
		CB_std = new float[CB_l*CB_w];
		for (int i = 0; i < CB_w; ++i) {
			for (int j = 0; j < CB_l; ++j) {
				float tmp = fabsf(CB_cell[i*CB_l+j]);
				CB_std[i*CB_l+j] = -0.0006034 * (tmp * 1000) * (tmp * 1000) + 0.06184 * tmp + 0.948661*0.000001;
			}
		}
	}

	double gaussrand()
	{
		static double V1, V2, S;
		static int phase = 0;
		double X;
		if (phase == 0) {
			do {
				double U1 = (double)rand() / RAND_MAX;
				double U2 = (double)rand() / RAND_MAX;
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S == 0);
			X = V1 * sqrt(-2 * log(S) / S);
		}
		else
			X = V2 * sqrt(-2 * log(S) / S);
		phase = 1 - phase;
		return X;

	}

	float get_noise(float x)
	{
		// float noise;
		x = fabsf(x);
		float noise = -0.0006034 * (x * 1000) * (x * 1000) + 0.06184 * x + 0.948661*0.000001;
		noise = noise * gaussrand();
		return noise;
	}

	void MatrixMul(float *input, float *CB_cells, float *output, int w, int l)
	{
        int i = 0;
#pragma omp parallel for private(i) //shared(w, l)
        for (i = 0; i < w; i++){
            float tmp = 0;
            int tmp_k = i*l;
            int j = 0;
#pragma omp parallel for private(j) reduction(+:tmp) shared(tmp_k)//, input, CB_cells)
            for (j = 0; j < l; j++){
                float tmpres = input[j] * (CB_cells[tmp_k+j] /*+ (CB_std[tmp_k+j] * gaussrand())*/);
                tmp = tmp + tmpres;
            }
            output[i] = tmp;
        }

	}

	void run(float *input, float *output, bool noise=true)
	{
		float *output_d = new float[CB_w];
		float *input_d = new float[CB_l];
		memcpy(input_d, input, CB_l * sizeof(float));

		MatrixMul(input_d, CB_cell, output_d, CB_w, CB_l);

		memcpy(output, output_d, CB_w * sizeof(float));
		delete[] output_d;
		delete[] input_d;
	}

	void free_space()
	{
		delete[] CB_cell;
		delete[] CB_std;
	}
}CROSSBAR;

#endif // !_CROSSBAR_H
