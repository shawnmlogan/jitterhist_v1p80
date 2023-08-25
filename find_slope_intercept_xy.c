
#include "jitterhist.h"

int find_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,double *pslope, double *pintercept)
{

long int i = 0;
int zero_flag = 0;

double sum_x = 0.0, sum_x2 = 0.0, sum_y = 0.0, sum_y2 = 0.0, sum_xy = 0.0;
double denominator = 0.0, numerator = 0.0;

while (i < number_of_xy_data_records)
	{
	sum_x = sum_x + pxy[i].x;
	sum_x2 = sum_x2 + pow(pxy[i].x,2);

	sum_y = sum_y + pxy[i].y;
	sum_y2 = sum_y2 + pow(pxy[i].y,2);
	
	sum_xy = sum_xy + pxy[i].x*pxy[i].y;
	i++;
	}

/* m =  (N*Σ(xy) − Σx Σy)/(N*Σ(x2) − (Σx)2) */
/* b =  (Σy − m Σx)/N */

denominator = ((double) i *sum_x2 - pow(sum_x,2.0));
numerator = ((double) i *sum_xy - sum_x*sum_y);

if ( denominator == 0)
	{
	zero_flag = 1;
	*pslope = 0.0;
	*pintercept = 0.0;
	}
else
	{
	*pslope = numerator/denominator;
	*pintercept = (sum_y - *pslope*sum_x)/(double) i;
	}

if (zero_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
