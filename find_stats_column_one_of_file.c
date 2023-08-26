#include "jitterhist.h"

int find_stats_column_one_of_file(char *pfin, double *ave_input_signal, double *min_input_signal, double *max_input_signal,long int *number_of_input_signal_lines)
{
char *pinput_string, input_string[LINELENGTH + 1];

int tokens, number_of_data_columns = 1, error_flag = 0;
long int i = 0, number_of_input_lines = 0;

double *pdoubles_array, sample_value = 0.0;
double max_value = BIG_NEG_NUM, min_value = BIG_POS_NUM, sum_values = 0.0, ave_value = 0.0;

FILE *fpw1;

pinput_string = &input_string[0];

if ((pdoubles_array = (double *) calloc(number_of_data_columns,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in find_stats_column_one_of_file()...exiting\n");
	error_flag = 1;
	}
else
	{
	fpw1 = fopen(pfin,"r");
	while(!feof(fpw1))
		{
		fgets(pinput_string,LINELENGTH,fpw1);
		remove_carriage_return(pinput_string);
		if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
			{
			sample_value = pdoubles_array[0];
			if (sample_value > max_value)
				max_value = sample_value;
			if (sample_value < min_value)
				min_value = sample_value;
			sum_values = sum_values + sample_value;
			number_of_input_lines++;
			}
		}
	fclose(fpw1);
	ave_value = sum_values/((double) number_of_input_lines);
		
	*max_input_signal = max_value;
	*min_input_signal = min_value;
	*ave_input_signal = ave_value;
	*number_of_input_signal_lines = number_of_input_lines;
	}

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

