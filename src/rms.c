#include "jitterhist.h"

double rms(double *px, long int N,double *paverage,int zero_mean_rms_flag)
{
long int i;
double sum = 0.0, sum_squares = 0.0, average = 0.0;


for(i = 0; i < N; i++)
   {
   sum = sum + px[i];
   }
average = sum/((double) N);
*paverage = average;

if (zero_mean_rms_flag == 1)
	average = average;
else
	average = 0.0;
	
for(i = 0; i < N; i++)
   {
   sum_squares = sum_squares + pow(px[i] - average,2.0);
   }
   
return pow(sum_squares/N,0.50);

}