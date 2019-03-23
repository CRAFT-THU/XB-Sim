#ifndef _CROSSBAR_H
#define _CROSSBAR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <random>
#include "config.h"

#define NSUM 25

using namespace std;

typedef struct Crossbar
{
    float *std_d;
    int CB_l;
    int CB_w;
    int CB_n;
    float *CB_cell;
    float *input;
    float *output;
    float *CB_std;
    std::default_random_engine eng;

    Crossbar(){}

    Crossbar(int n, int l, int w){
        CB_n = n;
        CB_l = l;
        CB_w = w;
        CB_cell = new float[CB_l*CB_w];
        CB_std = new float[CB_l*CB_w];
        input = new float[CB_l*(AD_WIDTH/DA_WIDTH)];
        output = new float[CB_w*(AD_WIDTH/DA_WIDTH)];
    }

    ~Crossbar(){
        delete []CB_cell;
        delete []CB_std;
        delete []input;
        delete []output;
    }

    void init(){
        float *tmp_cells = new float[CB_l*CB_w];
        // transform cb_cell
        for (int i = 0; i < CB_w; i++){
            for (int j = 0; j < CB_l; j++){
                tmp_cells[i*CB_l + j] = CB_cell[j*CB_w + i];
            }
        }
        memcpy(CB_cell, tmp_cells, sizeof(float)*CB_l*CB_w);
        delete []tmp_cells;
        get_std();
    }

    void init(float *CB_cells, int n, int l, int w)
    {
        CB_l = l;
        CB_w = w;
        CB_n = n;
        CB_cell = new float[CB_l*CB_w];
        input = new float[CB_l];
        output = new float[CB_w];
//		memcpy(CB_cell, CB_cells, CB_l*CB_w * sizeof(float));
        // transform cb_cell
        for (int i = 0; i < CB_w; i++){
            for (int j = 0; j < CB_l; j++){
                CB_cell[i*CB_l + j] = CB_cells[j*CB_w + i]; // +get_noise(CB_cells[j*CB_w + i]);
            }
        }
        get_std();
    }

    void get_std(){
        float max_conductance = 40;  // 25k ohm
        CB_std = new float[CB_l*CB_w];
        for (int i = 0; i < CB_w; ++i) {
            for (int j = 0; j < CB_l; ++j) {
                float tmp = fabsf(CB_cell[i*CB_l+j]);
//                CB_std[i*CB_l+j] = -0.0006034 * (tmp * 1000) * (tmp * 1000) + 0.06184 * tmp + 0.948661*0.000001;
                CB_std[i*CB_l+j] = (-0.0006034 * (tmp * max_conductance + 4) * (tmp * max_conductance + 4)
                                    + 0.06184 * (tmp * max_conductance + 4) + 0.7240) / max_conductance;
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

    double mygaussrand()
    {
        double x = 0;
        int i;
        for(i = 0; i < NSUM; i++)
        {
            x += (double)rand() / RAND_MAX;
        }

        x -= NSUM / 2.0;
        x /= sqrt(NSUM / 12.0);

        return x;
    }

    double mygaussrand2(){
        std::default_random_engine eng;
        std::normal_distribution<double> n(0, 1);
        return n(eng);
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
        std::normal_distribution<float> norm(0, 1);
#pragma omp parallel for private(i) //shared(w, l)
        for (i = 0; i < w; i++){
            float tmp = 0;
            int tmp_k = i*l;
            int j = 0;
#pragma omp parallel for private(j) reduction(+:tmp) shared(tmp_k)//, input, CB_cells)
            for (j = 0; j < l; j++){
//                float tmpres = input[j] * (CB_cells[tmp_k+j] + (CB_std[tmp_k+j] * mygaussrand2()));
                float tmpres = input[j] * (CB_cells[tmp_k+j] + (CB_std[tmp_k+j] * norm(eng)));
                tmp = tmp + tmpres;
            }
            output[i] = tmp;
        }
    }

    void run(){
        // crossbar computation for big crossbar
        int i = 0;
        int s = 0;
        std::normal_distribution<float> norm(0, 1);
        for (s = 0; s < AD_WIDTH/DA_WIDTH; s++) { // considering DA and AD
#pragma omp parallel for private(i)
            for (i = 0; i < CB_w; i++) {
                float tmp = 0;
                int tmp_k = i * CB_l;
                int tmp_m = s * CB_l;
                int j = 0;
#pragma omp parallel for private(j) reduction(+:tmp) shared(tmp_k, tmp_m)
                for (j = 0; j < CB_l; j++) {
                    float tmpres = input[tmp_m + j] * (CB_cell[tmp_k + j] + (CB_std[tmp_k+j] * norm(eng)));
                    tmp = tmp + tmpres;
                }
                output[s * CB_w + i] = tmp;
            }
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
        delete []CB_cell;
        delete []CB_std;
        delete []input;
        delete []output;
    }
}CROSSBAR;

CROSSBAR entire_cb(1, ENTIRE_L, ENTIRE_W);

#endif // !_CROSSBAR_H
