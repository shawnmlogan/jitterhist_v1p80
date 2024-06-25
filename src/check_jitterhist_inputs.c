#include "jitterhist.h"

int check_jitterhist_inputs(char *argv[], int argc, jitterhist_inputs *pjh_inputs)
{
int input_error_flag = 0;
int tokens = 0, sorted_flag = 1, slope_constant_flag = 0;
int data_column_number = 0;

char *pinput_string, input_string[LINELENGTH + 1];
char *pinput_string_long;
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];
char *pgnuplot = GNUPLOT;

double ave_input_signal = 0.0, max_input_signal = BIG_NEG_NUM, min_input_signal = BIG_POS_NUM;

FILE *fpw1;

pinput_string = &input_string[0];

 if (check_executable(pgnuplot,pjh_inputs->pgnuplot_path) == 0)
	{
	printf("\nWARNING: Did not locate executable for gnuplot in $PATH. ");
	printf("Disabling all plots.\n");
	printf("gnuplot may be downloaded from: http://www.gnuplot.info/download.html\n\n");
	pjh_inputs->enable_plot_flag = 0;
	}
else
	pjh_inputs->enable_plot_flag = 1;

/* Read and verify input file */

strncpy(pjh_inputs->pinput_filename,argv[1],MAXIMUM_INPUT_FILENAME_LINELENGTH);

if((pinput_string_long = (char *) calloc(strlen(pjh_inputs->pinput_filename) + 2,sizeof(char))) == NULL)
	{
	printf("Error allocating memory for pinput_string_long in check_pnoise_inputs().\n");
	input_error_flag = 1;
	}
else
	{
	strncpy(pinput_string_long,argv[1],strlen(pjh_inputs->pinput_filename) + 1);
	if (strcmp(pjh_inputs->pinput_filename,pinput_string_long) != 0)
		{
		printf("Input filename contains too many characters. Shorten filename to less than %d characters.\n",
		MAXIMUM_INPUT_FILENAME_LINELENGTH + 1);
		input_error_flag = 1;
		}
	else
		{
		if ((fpw1 = fopen(pjh_inputs->pinput_filename,"r")) == NULL)
			{ 
			printf("File \"%s\" does not exist!\nCheck and re-enter input file containing phase noise data.\n",pjh_inputs->pinput_filename);
			input_error_flag = 1;
			}  
		 else
			{
			strncpy(pinput_string,argv[2],LINELENGTH);
			if (atoi(pinput_string) < 1)
				{
				printf("Enter a column number greater than 0. Read a value of %d.\n",atoi(pinput_string));
				input_error_flag = 1;
				}
			else
				{
				pjh_inputs->column_number = atoi(pinput_string);
		
				/* Check to verify sufficient columns exist in input file */
				
				if (validate_input_file(pjh_inputs->pinput_filename,&pjh_inputs->column_number,&pjh_inputs->number_of_input_lines) != EXIT_SUCCESS)
			   	input_error_flag = 1;
			   else
			   	{
			   	printf("Input file: %s\n",pjh_inputs->pinput_filename);
			   	if (find_stats_column_N_of_file(pjh_inputs->pinput_filename,pjh_inputs->column_number,&ave_input_signal,&min_input_signal,
					&max_input_signal,&pjh_inputs->number_of_input_lines,&sorted_flag,&slope_constant_flag) == EXIT_SUCCESS)
						{
						printf("Column number: %d\n",pjh_inputs->column_number);
						pjh_inputs->number_of_input_lines = pjh_inputs->number_of_input_lines;
						}
					else
						{
						printf("find_stats_column_N_of_file() failed...exiting...\n");
						exit(0);
						}
					if (input_error_flag != 1)
						{
						
						/* Validate output filename length */
						
						free(pinput_string_long);
						strncpy(pjh_inputs->poutput_filename,argv[3],MAXIMUM_INPUT_FILENAME_LINELENGTH);
						if((pinput_string_long = (char *) calloc(strlen(pjh_inputs->poutput_filename) + 2,sizeof(char))) == NULL)
							{
							printf("Error allocating memory for pinput_string_long in check_jitterhist_inputs().\n");
							input_error_flag = 1;
							}
						else
							{
							strncpy(pinput_string_long,argv[3],strlen(pjh_inputs->poutput_filename) + 1);
							if (strcmp(pjh_inputs->poutput_filename,pinput_string_long) != 0)
								{
								printf("Output filename contains too many characters. Shorten filename to less than %d characters.\n",
								MAXIMUM_INPUT_FILENAME_LINELENGTH + 1);
								input_error_flag = 1;
								}
							else
								{
								printf("Output file: %s\n",pjh_inputs->poutput_filename);
								
								free(pinput_string_long);
										
								/* Read, assign, and check sample frequency */
								
								strncpy(pinput_string,argv[4],LINELENGTH);
								
								if (atof(pinput_string) <= 0)
									{
									printf("Sample frequency (in GHz) must be greater than zero, re-enter value of %s.\n",pinput_string);
									input_error_flag = 1;
									}
								else
									{
									if (input_error_flag != 1)
										{
										pjh_inputs->fs_GHz = atof(pinput_string);
										printf("Sample frequency is %s.\n",add_units_2((pjh_inputs->fs_GHz)*1e9,4,0,0,"Hz",value_string[0]));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Read, assign, and verify threshold value */
	strncpy(pinput_string,argv[5],LINELENGTH);
	if (atof(pinput_string) >= max_input_signal)
		{
		printf("Threshold value of %s exceeds maximum signal value of %s,\nre-enter a threshold value.\n",
		add_units_2(atof(pinput_string),1,0,0,"V",value_string[0]),add_units_2(max_input_signal,3,0,0,"V",value_string[1]));
		input_error_flag = 1;
		}
	else
		{
		if (atof(pinput_string) <= min_input_signal)
			{
			printf("Threshold value of %s is less than minimum signal value of %s,\nre-enter a threshold value.\n",
			add_units_2(atof(pinput_string),1,0,0,"V",value_string[0]),add_units_2(min_input_signal,3,0,0,"V",value_string[1]));
			input_error_flag = 1;
			}
		else
			{
			pjh_inputs->threshold = atof(pinput_string);
			printf("Threshold value is %s.\n",add_units_2(pjh_inputs->threshold,3,0,0,"V",value_string[0]));
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Read and verify number of moving average samples */

	strncpy(pinput_string,argv[6],LINELENGTH);

	if (atoi(pinput_string) < 0)
		{
		printf("Number of moving average samples read is less than zero. Re-enter value of 0 or positive value less than %ld. Read value of of %s.\n",
		pjh_inputs->number_of_input_lines,pinput_string);
		input_error_flag = 1;
		}
	else
		{
		if (atol(pinput_string) > (long int) ceil(pjh_inputs->number_of_input_lines/10 - 1))
			{
			printf("Number of moving average samples read is too large for the number of input samples. Re-enter value of 0 or positive value less than %.0f. Read value of %s.\n",
			ceil(pjh_inputs->number_of_input_lines/10 - 1),pinput_string);
			input_error_flag = 1;
			}
		else
			{
			pjh_inputs->num_moving_average_samples = atoi(pinput_string);
			printf("Number of moving average samples = %d.\n",pjh_inputs->num_moving_average_samples);
			}
		}
	}

if (input_error_flag != 1)
	{	
	/* Read and verify if slope correction is desired and set flag to proper logic level */

	strncpy(pinput_string,argv[7],LINELENGTH);

	if ((tolower(input_string[0]) != 'n') && (tolower(input_string[0]) != 'y'))
		{
		printf("Enter a \"y\" or \"n\" to apply slope correction to computed TIE.\n");
		printf("Both the uncorrected and corrected values of TIE are provided in separate files.\nRead value of %s.\n",pinput_string);
		input_error_flag = 1;
		}
	else
		{
		if ((tolower(input_string[0])) == 'n')
			pjh_inputs->correct_slope_flag = 0;
		else
			pjh_inputs->correct_slope_flag = 1;
		}
	}

if (input_error_flag != 1)
	{	
	/* Read and verify if using user specified average frequency and average frequency value (if used) */

	strncpy(pinput_string,argv[8],LINELENGTH);

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
					pjh_inputs->use_ave_freq_flag = 0;
					pjh_inputs->ave_freq_MHz = atof(pinput_string);
					printf("Entered average frequency is %s.\n",add_units_2((pjh_inputs->ave_freq_MHz)*1e6,4,0,0,"Hz",value_string[0]));
					}
				}
			}
		else
			pjh_inputs->use_ave_freq_flag = 1;
		}
	}

if (input_error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}