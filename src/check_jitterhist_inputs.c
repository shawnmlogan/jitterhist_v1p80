#include "jitterhist.h"

int check_jitterhist_inputs(char *argv[], int argc, char *pfin, int *column_number, char *pfout, double *fs_GHz, double *threshold_value,
int *num_moving_average_samples, int *use_ave_freq, double *ave_freq_MHz,long int *number_of_input_lines,int *correct_slope_flag)
{
int input_error_flag = 0;
int tokens = 0, sorted_flag = 1;
int data_column_number = 0;

char *pinput_string, input_string[LINELENGTH + 1];
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];

double ave_input_signal = 0.0, max_input_signal = BIG_NEG_NUM, min_input_signal = BIG_POS_NUM;

FILE *fpw1;

pinput_string = &input_string[0];

/* Read and verify input file */

strcpy(pfin,argv[1]);

if ((fpw1 = fopen(pfin,"r")) == NULL)
	{  
	printf("File \"%s\" does not exist!\nCheck and re-enter input file of signal to analyze for jitter", pfin);
	input_error_flag = 1;
	}  
 else
	{
	strcpy(pinput_string,argv[2]);
	if (atoi(pinput_string) < 1)
		{
		printf("Enter a column number greater than 0. Read a value of %d.\n",atoi(pinput_string));
		input_error_flag = 1;
		}
	else
		{
		*column_number = atoi(pinput_string);

		/* Check to verify sufficient columns exist in input file */
		
		if (validate_input_file(pfin,column_number,number_of_input_lines) != EXIT_SUCCESS)
	   	input_error_flag = 1;
	   else
	   	{
	   	printf("Input file: %s\n",pfin);
	   	if (find_stats_column_N_of_file(pfin,*column_number,&ave_input_signal,&min_input_signal,
			&max_input_signal,number_of_input_lines,&sorted_flag) == EXIT_SUCCESS)
				{
				printf("Column number: %d\n",*column_number);
				}
			else
				{
				printf("find_stats_column_N_of_file() failed...exiting...\n");
				exit(0);
				}
			if (input_error_flag != 1)
				{
				strcpy(pfout,argv[3]);
				printf("Output file: %s\n",pfout);
	
				/* Read, assign, and check sample frequency */
				
				strcpy(pinput_string,argv[4]);
				
				if (atof(pinput_string) <= 0)
					{
					printf("Sample frequency (in GHz) must be greater than zero, re-enter value of %s.\n",pinput_string);
					input_error_flag = 1;
					}
				else
					{
					if (input_error_flag != 1)
						{
						*fs_GHz = atof(pinput_string);
						printf("Sample frequency is %s.\n",add_units((*fs_GHz)*1e9,4,"Hz",value_string[0]));
						}
					}
				}
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Read, assign, and verify threshold value */
	strcpy(pinput_string,argv[5]);
	if (atof(pinput_string) >= max_input_signal)
		{
		printf("Threshold value of %s exceeds maximum signal value of %s,\nre-enter a threshold value.\n",
		add_units(atof(pinput_string),1,"V",value_string[0]),add_units(max_input_signal,3,"V",value_string[1]));
		input_error_flag = 1;
		}
	else
		{
		if (atof(pinput_string) <= min_input_signal)
			{
			printf("Threshold value of %s is less than minimum signal value of %s,\nre-enter a threshold value.\n",
			add_units(atof(pinput_string),1,"V",value_string[0]),add_units(min_input_signal,3,"V",value_string[1]));
			input_error_flag = 1;
			}
		else
			{
			*threshold_value = atof(pinput_string);
			printf("Threshold value is %s.\n",add_units(*threshold_value,3,"V",value_string[0]));
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Read and verify number of moving average samples */

	strcpy(pinput_string,argv[6]);

	if (atoi(pinput_string) < 0)
		{
		printf("Number of moving average samples read is less than zero. Re-enter value of 0 or positive value less than %ld. Read value of of %s.\n",
		*number_of_input_lines,pinput_string);
		input_error_flag = 1;
		}
	else
		{
		if (atol(pinput_string) >= *number_of_input_lines)
			{
			printf("Number of moving average samples read is too large for the number of input samples. Re-enter value of 0 or positive value less than %ld. Read value of %s.\n",
			*number_of_input_lines,pinput_string);
			input_error_flag = 1;
			}
		else
			{
			*num_moving_average_samples = atoi(pinput_string);
			printf("Number of moving average samples = %d.\n",*num_moving_average_samples);
			}
		}
	}

if (input_error_flag != 1)
	{	
	/* Read and verify if slope correction is desired and set flag to proper logic level */

	strcpy(pinput_string,argv[7]);

	if ((tolower(input_string[0]) != 'n') && (tolower(input_string[0]) != 'y'))
		{
		printf("Enter a \"y\" or \"n\" to apply slope correction to computed TIE.\n");
		printf("Both the uncorrected and corrected values of TIE are provided in separate files.\nRead value of %s.\n",pinput_string);
		input_error_flag = 1;
		}
	else
		{
		if ((tolower(input_string[0])) == 'n')
			*correct_slope_flag = 0;
		else
			*correct_slope_flag = 1;
		}
	}

if (input_error_flag != 1)
	{	
	/* Read and verify if using user specified average frequency and average frequency value (if used) */

	strcpy(pinput_string,argv[8]);

	if ((tolower(input_string[0]) != 'n') && (tolower(input_string[0]) != 'y'))
		{
		printf("Enter a \"y\" or \"n\" to use entered frequency as average frequency\n");
		printf("in lieu of computed average frequency Read value of %s.\n",pinput_string);
		input_error_flag = 1;
		}
	else
		{
		if ((tolower(input_string[0])) == 'n')
			{
			if (argc != 10)
				{
				printf("Enter value for average frequency to use. No value detected.\n");
				input_error_flag = 1;
				}
			else
				{
				strncpy(pinput_string,argv[9],LINELENGTH);
				if (atof(pinput_string) <= 0.0)
					{
					printf("Entered average frequency (in MHz) must be greater than zero,\n");
					printf("Re-enter value entered value of %s.\n,",pinput_string);
					input_error_flag = 1;
					}
				else
					{
					*use_ave_freq = 0;
					*ave_freq_MHz = atof(pinput_string);
					printf("Entered average frequency is %s.\n",add_units((*ave_freq_MHz)*1e6,4,"Hz",value_string[0]));
					}
				}
			}
		else
			*use_ave_freq = 1;
		}
	}

if (input_error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}