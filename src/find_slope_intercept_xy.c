
#include "jitterhist.h"

int find_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,char *ptimestamp,double *pslope, double *pintercept)
{

long int i = 0, num_equal_y_values = 1, j = 0, number_of_iterations = 0;
int zero_flag = 0, error_flag = 0;

double sum_x = 0.0, sum_x2 = 0.0, sum_y = 0.0, sum_y2 = 0.0, sum_xy = 0.0;
double denominator = 0.0, numerator = 0.0, x_mean = 0.0, y_mean = 0.0;
double y_min = BIG_POS_NUM, y_max = BIG_NEG_NUM;
double init_slope, coarse_slope = 0.0, coarse_intercept = 0.0, coarse_peak_to_peak = 0.0;
double fine_slope = 0.0, fine_intercept = 0.0, fine_peak_to_peak = 0.0;
double ultra_fine_slope = 0.0, ultra_fine_intercept = 0.0, ultra_fine_peak_to_peak = 0.0;


FILE *fpw1;

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
	init_slope = numerator/denominator;
	if(find_min_pp_slope_intercept_xy(pxy,number_of_xy_data_records,ptimestamp,init_slope,x_mean,y_mean,&coarse_slope,&coarse_intercept,&coarse_peak_to_peak) != EXIT_SUCCESS)
		{
		printf("Error occurred in coarse find_min_pp_slope_intercept_xy() in find_slope_intercept_xy.c.\n");
		error_flag = 1;
		}
	else
		{
		#ifdef DEBUG_FIND_SLOPE_INTERCEPT
			printf("After coarse slope search, minimum peak_to_peak = %1.6e coarse_slope = %1.6e coarse_intercept = %1.6e\n",
			coarse_peak_to_peak,coarse_slope,coarse_intercept);
		#endif	
		if(find_min_pp_slope_intercept_xy(pxy,number_of_xy_data_records,ptimestamp,coarse_slope,x_mean,y_mean,&fine_slope,&fine_intercept,&fine_peak_to_peak) != EXIT_SUCCESS)
			{
			printf("Error occurred in fine find_min_pp_slope_intercept_xy() in find_slope_intercept_xy.c.\n");
			error_flag = 1;
			}
		else
			{
			#ifdef DEBUG_FIND_SLOPE_INTERCEPT
				printf("After fine slope search, minimum peak_to_peak = %1.6e fine_slope = %1.6e fine_intercept = %1.6e\n",
				fine_peak_to_peak,fine_slope,fine_intercept);
			#endif
			if(find_min_pp_slope_intercept_xy(pxy,number_of_xy_data_records,ptimestamp,fine_slope,x_mean,y_mean,&ultra_fine_slope,&ultra_fine_intercept,&ultra_fine_peak_to_peak) != EXIT_SUCCESS)
				{
				printf("Error occurred in ultrafine find_min_pp_slope_intercept_xy() in find_slope_intercept_xy.c.\n");
				error_flag = 1;
				}
			else
				{
				#ifdef DEBUG_FIND_SLOPE_INTERCEPT
					printf("After ultrafine slope search, minimum peak_to_peak = %1.6e ultra_fine_slope = %1.6e ultra_fine_intercept = %1.6e\n",
					ultra_fine_peak_to_peak,ultra_fine_slope,ultra_fine_intercept);
				#endif	
				*pslope = ultra_fine_slope;
				*pintercept = ultra_fine_intercept;
				}
			}
		}	
	}

if ((zero_flag == 1) || (error_flag == 1))
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
