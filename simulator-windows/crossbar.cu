#include "crossbar_cuda.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <curand.h>
#include <time.h>
#include <math.h>

__global__ void CUDA_abs(float *a,float *b,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    int col = threadIdx.x;
    b[n_cell*rows*cols+row*cols+col]=fabs(a[n_cell*rows*cols+row*cols+col]);
}

__global__ void CUDA_add(float *a,float *b,float *c,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    int col = threadIdx.x;
    c[n_cell*rows*cols+row*cols+col]=a[n_cell*rows*cols+row*cols+col]+b[n_cell*rows*cols+row*cols+col];
}

__global__ void CUDA_mul(float *a,float b,float *c,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    int col = threadIdx.x;
    c[n_cell*rows*cols+row*cols+col]=a[n_cell*rows*cols+row*cols+col]*b;
}


__global__ void CUDA_mmul(float *a,float *b,float *c,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    int col = threadIdx.x;
    c[n_cell*rows*cols+row*cols+col]=a[n_cell*rows*cols+row*cols+col]*b[n_cell*rows*cols+row*cols+col];
}

__global__ void CUDA_shift(float *a,float b,float *c,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    int col = threadIdx.x;
    c[n_cell*rows*cols+row*cols+col]=a[n_cell*rows*cols+row*cols+col]+b;
}

__global__ void CUDA_MatrixMui(float *a,float *b,float *c,int cols,int rows) {
    int n_cell= blockIdx.x ;
    int row  = blockIdx.y ;
    //int col = threadIdx.x;
    float temp = 0;
    for (int i=0;i<cols;i++)
    {
        temp+=a[n_cell*cols+i]*b[n_cell*rows*cols+row*cols+i];

    }

    c[n_cell*rows+row]=temp;
}

void Crossbar::init(float *CB_cells, int n, int l, int w)
{
    CB_l=l;
    CB_w=w;
    CB_n=n;
    cudaMalloc((void **)&CB_cell, CB_n*CB_l*CB_w*sizeof(float));
    cudaMemcpy(CB_cell, CB_cells, CB_n*CB_l*CB_w * sizeof(float),cudaMemcpyHostToDevice);
    get_std();
    curandCreateGenerator(&gen, CURAND_RNG_PSEUDO_DEFAULT);
    clock_t time;
    time=clock();
    curandSetPseudoRandomGeneratorSeed(gen, (int)time);

}

void Crossbar::printcrossbar() {
    float *temp_cell;
    temp_cell = (float*)calloc(CB_n*CB_l*CB_w,sizeof(float));
    //temp_cell = new float [CB_n*CB_l*CB_w];
    cudaMemcpy(temp_cell, CB_cell, CB_n*CB_l*CB_w* sizeof(float),cudaMemcpyDeviceToHost) ;
    printf ("_______________\n");
    for (int i=0;i<CB_n;i++)
    {
        for (int j=0;j<CB_l;j++)
        {
            for(int k=0;k<CB_w;k++)
            {
                printf("%f,%d,%d,%d,%d ",temp_cell[i*CB_l*CB_w+j*CB_w+k],i,j,k,i*CB_l*CB_w+j*CB_w+k);

            }
            printf ("\n");
        }
        printf ("\n");
    }
    printf ("_______________\n");
    free(temp_cell);
}

void Crossbar::get_std() {//-0.0006034 * (x * 1e3) ** 2 + 0.06184 * x + 0.7240 * 1e-6
    dim3 numBlocks(CB_n, CB_l);
    cudaMalloc((void **)&std_d, CB_n*CB_l*CB_w*sizeof(float));
    float *temp_1;
    cudaMalloc((void **)&temp_1, CB_n*CB_l*CB_w*sizeof(float));
    //cudaMemcpy(temp_1, CB_cell, CB_n*CB_l*CB_w* sizeof(float),cudaMemcpyDeviceToDevice) ;
    CUDA_abs<<<numBlocks,CB_w>>>(CB_cell,temp_1,CB_w,CB_l);
    float *temp_2;
    cudaMalloc((void **)&temp_2, CB_n*CB_l*CB_w*sizeof(float));
    CUDA_mul<<<numBlocks,CB_l>>>(temp_1,1000,temp_2,CB_w,CB_l);
    cudaMemcpy(temp_1, temp_2, CB_n*CB_l*CB_w* sizeof(float),cudaMemcpyDeviceToDevice) ;
    float *temp_3;
    cudaMalloc((void **)&temp_3, CB_n*CB_l*CB_w*sizeof(float));
    CUDA_mmul<<<numBlocks,CB_w>>>(temp_1,temp_2,temp_3,CB_w,CB_l);
    CUDA_mul<<<numBlocks,CB_w>>>(temp_3,-0.0006034,temp_1,CB_w,CB_l);
    CUDA_mul<<<numBlocks,CB_w>>>(CB_cell,0.06184,temp_2,CB_w,CB_l);
    CUDA_add<<<numBlocks,CB_w>>>(temp_1,temp_2,temp_3,CB_w,CB_l);
    CUDA_shift<<<numBlocks,CB_w>>>(temp_3,0.7240*0.000001,temp_1,CB_w,CB_l);
    cudaMemcpy(std_d, temp_1, CB_n*CB_l*CB_w* sizeof(float),cudaMemcpyDeviceToDevice) ;
    cudaFree( temp_1 );
    cudaFree( temp_2 );
    cudaFree( temp_3 );
}

void Crossbar::get_noise(float *noise) {
    cudaMalloc((void **)&noise, CB_n*CB_l*CB_w*sizeof(float));
    curandGenerateNormal(gen, noise, CB_n*CB_l*CB_w, 0, 1);
}

void Crossbar::printstd() {
    printf ("~~~~~~~~~~~~~~~~~~~\n");
    float *temp_cell;
    temp_cell = (float*)calloc(CB_n*CB_l*CB_w,sizeof(float));
    cudaMemcpy(temp_cell, std_d, CB_n*CB_l*CB_w* sizeof(float),cudaMemcpyDeviceToHost) ;
    for (int i=0;i<CB_n;i++)
    {
        for (int j=0;j<CB_l;j++)
        {
            for(int p=0;p<CB_w;p++)
            {
                printf("%f ",temp_cell[i*CB_l*CB_w+j*CB_w+p]);
            }
            printf ("\n");
        }
        printf ("\n");
    }
    printf ("~~~~~~~~~~~~~~~~~~~\n");
    free(temp_cell);
}

void Crossbar::run(float *input, float *output, bool use_noise=true) {
    float *input_d,*output_d;
    cudaMalloc((void **)&input_d, CB_n*CB_w*sizeof(float));
    cudaMalloc((void **)&output_d, CB_n*CB_l*sizeof(float));
    cudaMemcpy(input_d, input, CB_n*CB_w * sizeof(float),cudaMemcpyHostToDevice);
    dim3 numBlocks(CB_n, CB_l);
    if (use_noise)
    {
        float *temp_noise,*temp_cell;
        cudaMalloc((void **)&temp_noise, CB_n*CB_w*CB_l*sizeof(float));
        cudaMalloc((void **)&temp_cell, CB_n*CB_w*CB_l*sizeof(float));
        get_noise(temp_noise);
        CUDA_add<<<numBlocks,CB_w>>>(CB_cell,temp_noise,temp_cell,CB_w,CB_l);
        CUDA_MatrixMui<<<numBlocks,1>>>(input_d,temp_cell,output_d,CB_w,CB_l);
    }
    else
    {
        CUDA_MatrixMui<<<numBlocks,1>>>(input_d,CB_cell,output_d,CB_w,CB_l);
    }
    cudaMemcpy(output, output_d, CB_n*CB_l* sizeof(float),cudaMemcpyDeviceToHost) ;
    cudaFree( input_d );
    cudaFree( output_d );
}

void Crossbar::free_space() {
    // do something
}