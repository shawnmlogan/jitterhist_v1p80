
#include "jitterhist.h"

int find_min_pp_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,char *ptimestamp,double computed_slope, double x_mean, double y_mean, double *pslope, double *pintercept, double *ppeak_to_peak)
{

int error_flag = 0;
long int i = 0, j = 0, number_of_iterations = 0;
static int function_call_number;
static double init_delta_slope;

char *pdebug_filename,debug_filename[LINELENGTH + 1];

double y_min = BIG_POS_NUM, y_max = BIG_NEG_NUM;
double delta_slope, delta_pp = BIG_NEG_NUM;
double init_slope, slope[MAXIMUM_FIND_SLOPE_ITERATIONS]= {0.0}, intercept[MAXIMUM_FIND_SLOPE_ITERATIONS] = {0.0};
double peak_to_peak[MAXIMUM_FIND_SLOPE_ITERATIONS], min_peak_to_peak = 0.0;

FILE *fpw1;

pdebug_filename = &debug_filename[0];

function_call_number++;

if((function_call_number == 1) || (function_call_number == 4))
	{
	init_slope = 0.0 - computed_slope;
	if (init_slope < 0.0)
		delta_slope = 4.0*fabs(init_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
	else
		delta_slope = 0.0 - 4.0*fabs(init_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
	init_delta_slope = delta_slope;
	}
else
	{
	if((function_call_number == 2) || (function_call_number == 5))
		{
		init_slope = computed_slope - init_delta_slope;
		if (init_slope < 0.0)
			delta_slope = 0.0 - 2.0*fabs(init_delta_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
		else
			delta_slope = 2.0*fabs(init_delta_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
		init_delta_slope = delta_slope;
		}
	else
		{
		init_slope = computed_slope - init_delta_slope;
		if (init_slope < 0.0)
			delta_slope = 0.0 - 2.0*fabs(init_delta_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
		else
			delta_slope = 2.0*fabs(init_delta_slope)/((double) MAXIMUM_FIND_SLOPE_ITERATIONS);
		}
	}
	
#ifdef DEBUG_FIND_SLOPE_INTERCEPT	
	printf("\ninit_slope = %1.6e, delta_slope = %1.6e\n",init_slope,delta_slope);
	snprintf(pdebug_filename,LINELENGTH,"slope_vs_pp_iteration_%d_%s.csv",function_call_number,ptimestamp);
	fpw1 = fopen(pdebug_filename,"w");
	fprintf(fpw1,"Iteration,Slope,Peak-to-peak (UI)\n");
#endif

for (j = 0; j < MAXIMUM_FIND_SLOPE_ITERATIONS; j++)
	peak_to_peak[j] = BIG_POS_NUM;

j = 0;
delta_pp = BIG_NEG_NUM;

#ifdef DEBUG_FIND_SLOPE_INTERCEPT
	while (j < MAXIMUM_FIND_SLOPE_ITERATIONS)
#endif

#ifndef DEBUG_FIND_SLOPE_INTERCEPT
	while ((j < MAXIMUM_FIND_SLOPE_ITERATIONS) && (delta_pp < 0.0))
#endif

	{
	slope[j] = init_slope + delta_slope * (double) j;
	intercept[j] = y_mean - slope[j]*x_mean;
	y_max = BIG_NEG_NUM;
	y_min = BIG_POS_NUM;
	for (i = 0; i < number_of_xy_data_records;i++)
		{
		if((pxy[i].y - (slope[j]*pxy[i].x + intercept[j])) > y_max)
			y_max = pxy[i].y - (slope[j]*pxy[i].x + intercept[j]);
		if((pxy[i].y - (slope[j]*pxy[i].x + intercept[j])) < y_min)
			y_min = pxy[i].y - (slope[j]*pxy[i].x + intercept[j]);
		}
	peak_to_peak[j] = y_max - y_min;
	number_of_iterations = j;
	if (j > 0)
		delta_pp = peak_to_peak[j] - peak_to_peak[j - 1];
	
	#ifdef DEBUG_FIND_SLOPE_INTERCEPT
		fprintf(fpw1,"%ld,%1.6e,%1.6e\n",j,slope[j],peak_to_peak[j]);
	#endif
	
	j++;
	}
min_peak_to_peak = min(peak_to_peak,number_of_iterations);
for (j = 0; j < number_of_iterations; j++)
	{
	if(peak_to_peak[j] == min_peak_to_peak)
		{
		#ifdef DEBUG_FIND_SLOPE_INTERCEPT
			printf("After %ld iterations (function call %d), minimum peak_to_peak[%ld] = %1.6e slope[%ld] = %1.6e\n",
			number_of_iterations,function_call_number,j,peak_to_peak[j],j,slope[j]);
			fclose(fpw1);
		#endif	
		*pslope = slope[j];
		}
	}
*pintercept = y_mean - *pslope*x_mean;
*ppeak_to_peak = min_peak_to_peak;

if (error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
