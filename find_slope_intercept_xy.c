
#include "jitterhist.h"

int find_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,double *pslope, double *pintercept)
{

long int i = 0, num_equal_y_values = 1;
int zero_flag = 0;

double sum_x = 0.0, sum_x2 = 0.0, sum_y = 0.0, sum_y2 = 0.0, sum_xy = 0.0;
double denominator = 0.0, numerator = 0.0, x_mean = 0.0, y_mean = 0.0;

for (i = 0; i < number_of_xy_data_records;i++)
	{
	sum_x = sum_x + pxy[i].x;
	sum_y = sum_y + pxy[i].y;
	}

x_mean =sum_x/((double) number_of_xy_data_records);
y_mean =sum_y/((double) number_of_xy_data_records);

for (i = 0; i < number_of_xy_data_records;i++)
	{
	if (i > 0)
		{
		if (fabs(2.0*(pxy[i].y - pxy[i-1].y)/(pxy[i].y + pxy[i-1].y)) < EPSILON)
			num_equal_y_values++;
		}
	numerator = numerator + (pxy[i].x - x_mean)*(pxy[i].y - y_mean);
	denominator = denominator + pow((pxy[i].x - x_mean),2);
	sum_y = sum_y + pxy[i].y;
	}

if (num_equal_y_values == number_of_xy_data_records)
	numerator = 0.0;
else
	numerator = numerator;
	
if ( denominator == 0)
	{
	zero_flag = 1;
	*pslope = 0.0;
	*pintercept = 0.0;
	}
else
	{
	*pslope = numerator/denominator;
	*pintercept = y_mean - *pslope*x_mean;
	}

if (zero_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
