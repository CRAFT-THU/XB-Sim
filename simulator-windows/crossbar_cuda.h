#ifndef crossbar_cuda
#define crossbar_cuda
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <curand.h>
#include <time.h>
#include <math.h>
#include "config.h"

__global__ void CUDA_abs(float *a,float *b,int cols,int rows);

__global__ void CUDA_add(float *a,float *b,float *c,int cols,int rows);

__global__ void CUDA_mul(float *a,float b,float *c,int cols,int rows);

__global__ void CUDA_mmul(float *a,float *b,float *c,int cols,int rows);

__global__ void CUDA_shift(float *a,float b,float *c,int cols,int rows);

__global__ void CUDA_MatrixMui(float *a,float *b,float *c,int cols,int rows);


typedef struct Crossbar
{
	float *CB_cell;
	float *std_d;
	float *input;
	float *output;
	int CB_n;
	int CB_l;
	int CB_w;
    curandGenerator_t gen;

    // for big crossbar
    Crossbar();
    ~Crossbar();
    Crossbar(int n, int l, int w);
    void init();
    void run();
	
	void init(float *CB_cells, int n, int l, int w);

	void printcrossbar();

	void get_std();//-0.0006034 * (x * 1e3) ** 2 + 0.06184 * x + 0.7240 * 1e-6

	void get_noise(float *noise);

    void printstd();

    void run(float *input, float *output, bool use_noise=true);

	void free_space();
}CROSSBAR;

CROSSBAR entire_cb(1, ENTIRE_L, ENTIRE_W);
#endif
