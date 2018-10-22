//
// Created by 刘鹤 on 2018/10/22.
//

#ifndef DAC_H
#define DAC_H

#include <stdio.h>
#include <stdlib.h>
//1bit DAC
typedef struct da
{
    float DA_v;
    float DA_out;

    da(float da_v) :DA_v(da_v)
    {}

    void trans(int DA_in, int DA_width)
    {
        if (DA_width == 1)
            DA_out = DA_in * float(0.15);
        if (DA_width == 2)
            DA_out = DA_in * float(0.05);
    }

}D;

#endif //DAC_H
