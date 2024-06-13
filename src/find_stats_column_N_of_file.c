#include "jitterhist.h"
#define CONSTANT_SLOPE 1

int find_stats_column_N_of_file(char *pfin, int column_number, double *ave_input_signal, double *min_input_signal, double *max_input_signal,long int *number_of_input_signal_lines, int *sorted_flag, int *slope_constant_flag)
{
char *pinput_string, input_string[LINELENGTH + 1];
char *pstring, string[LINELENGTH + 1];

int tokens, error_flag = 0, local_slope_constant_flag = CONSTANT_SLOPE;
long int i = 0, number_of_input_lines = 0;

double *pdoubles_array, sample_value = 0.0, last_sample_value = 0.0, slope = 0.0, last_slope = 0.0;
double max_value = BIG_NEG_NUM, min_value = BIG_POS_NUM, sum_values = 0.0, ave_value = 0.0;

FILE *fpw1;

pstring = &string[0];
pinput_string = &input_string[0];

if ((pdoubles_array = (double *) calloc(column_number,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in find_stats_column_N_of_file()...exiting\n");
	error_flag = 1;
	}
else
	{
	fpw1 = fopen(pfin,"r");
	*sorted_flag = 1;
	while(!feof(fpw1))
		{
		fgets(pinput_string,LINELENGTH,fpw1);
		remove_carriage_return(pinput_string);
		if (!feof(fpw1))
			{
			if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
				{
				sample_value = pdoubles_array[column_number - 1];
				if (sample_value > max_value)
					max_value = sample_value;
				if (sample_value < min_value)
					min_value = sample_value;
				sum_values = sum_values + sample_value;
				if (number_of_input_lines > 0)
					{
					slope = sample_value - last_sample_value;
					if (slope <= 0.0)
						*sorted_flag = 0;
					if (number_of_input_lines > 1)
						{
						if ((fabs(2.0*(slope - last_slope)/(slope + last_slope)) > EPSILON) && (local_slope_constant_flag == CONSTANT_SLOPE))
							{
							#ifdef DEBUG_INTEGRATION_TYPE
								snprintf(pstring,LINELENGTH,"At number_of_input_lines = %ld, slope was found to be non-constant:\n",number_of_input_lines);
								printf("%s",pstring);
								snprintf(pstring,LINELENGTH,"slope = %1.6e, last_slope = %1.6e, fabs(2.0*(slope - last_slope)/(slope + last_slope)) = %1.6e\n",
								slope,last_slope,fabs(2.0*(slope - last_slope)/(slope + last_slope)));
								printf("%s",pstring);
							#endif
							local_slope_constant_flag = 0;
							}
						#ifdef VERBOSE_DEBUG_INTEGRATION_TYPE
							snprintf(pstring,LINELENGTH,"At number_of_input_lines = %ld:\n",number_of_input_lines);
							printf("%s",pstring);
							snprintf(pstring,LINELENGTH,"slope = %1.6e, last_slope = %1.6e, fabs(2.0*(slope - last_slope)/(slope + last_slope)) = %1.6e\n",
							slope,last_slope,fabs(2.0*(slope - last_slope)/(slope + last_slope)));
							printf("%s",pstring);
						#endif
						}
					last_slope = slope;
					}		
				number_of_input_lines++;
				last_sample_value = sample_value;
				}
			}
		}
	fclose(fpw1);
	if (number_of_input_lines != 0)
		ave_value = sum_values/((double) number_of_input_lines);
	else
		{
		printf("Number of valid input lines is 0.\n");
		error_flag = 1;
		}
	*max_input_signal = max_value;
	*min_input_signal = min_value;
	*ave_input_signal = ave_value;
	*number_of_input_signal_lines = number_of_input_lines;
	free(pdoubles_array);
	}

*slope_constant_flag = local_slope_constant_flag;

if (error_flag == 1)
	{
	*max_input_signal = 0.0;
	*min_input_signal = 0.0;
	*ave_input_signal = 0.0;
	*number_of_input_signal_lines = 0;
	return EXIT_FAILURE;
	}
else
	return EXIT_SUCCESS;
	
}

