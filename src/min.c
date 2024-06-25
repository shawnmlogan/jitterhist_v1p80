#include "jitterhist.h"

double min(double *px, long int N)
{
long int i;
double min_value = 0.0 + BIG_POS_NUM;

for(i=0;i < N;i++)
   {
   if(px[i] < min_value)
      min_value = px[i];
   }
return min_value;
}
