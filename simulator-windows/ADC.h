#ifndef ADC
#define ADC
#include <stdio.h>
#include <stdlib.h>

typedef struct ad
{
	float AD_v;
	float AD_out;

	ad(float ad_v) :AD_v(ad_v)
	{}

	void trans(float AD_in)
	{
		float tmp = AD_in * AD_v * 255;
		AD_out = tmp;
	}
}A;

#endif // !ADC