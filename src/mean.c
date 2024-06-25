
#include "jitterhist.h"

double mean(double *x, long int N)
{
long int i;
double sum = 0.0;

for(i=0;i < N;i++)
   {
   sum = sum + x[i];
   }
return sum/((double) i);

}
