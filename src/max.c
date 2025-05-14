#include "jitterhist.h"

double max(double *px, long int N)
{
long int i;
double max_value = 0.0 - BIG_POS_NUM;

for(i=0;i < N;i++)
   {
   if(px[i] > max_value)
      max_value = px[i];
   }
return max_value;
}