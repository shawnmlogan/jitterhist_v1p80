#include "jitterhist.h"

int check_jitterhist_inputs(char *argv[], int argc, char *plog_string,jitterhist_inputs *pjh_inputs)
{
int input_error_flag = 0, argc_max = 0;
int tokens = 0, sorted_flag = 1, slope_constant_flag = 0;
int data_column_number = 0;
long int i, window_number;

char *plocal_log_string,local_log_string[LOGFILE_LINELENGTH + 1];
char *plocal_log_string_temp,local_log_string_temp[LOGFILE_LINELENGTH + 1];
char *pinput_string, input_string[LINELENGTH + 1];
char *pinput_string_long;
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];
char *pgnuplot = GNUPLOT;

double ave_input_signal = 0.0, max_input_signal = BIG_NEG_NUM, min_input_signal = BIG_POS_NUM;

FILE *fpw1;

plocal_log_string = &local_log_string[0];
plocal_log_string_temp = &local_log_string_temp[0];
pinput_string = &input_string[0];
strncpy(plocal_log_string,"",LOGFILE_LINELENGTH);
strncpy(plocal_log_string_temp,"",LOGFILE_LINELENGTH);

 if ((pjh_inputs->enable_plot_flag == 1) && (check_executable(pgnuplot,pjh_inputs->pgnuplot_path) == 0))
	{
	snprintf(plocal_log_string,LOGFILE_LINELENGTH,"\nWARNING: Did not locate executable for gnuplot in $PATH. ");
	strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
	snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sDisabling all plots.\n",plocal_log_string_temp);
	strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
	snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sgnuplot may be downloaded from: http://www.gnuplot.info/download.html\n\n",plocal_log_string_temp);
	strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
	pjh_inputs->enable_plot_flag = 0;
	}

/* Read and verify input file */

strncpy(pjh_inputs->pinput_filename,argv[1],MAXIMUM_INPUT_FILENAME_LINELENGTH);

if((pinput_string_long = (char *) calloc(strlen(pjh_inputs->pinput_filename) + 2,sizeof(char))) == NULL)
	{
	snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sError allocating memory for pinput_string_long in check_pnoise_inputs().\n",plocal_log_string_temp);
	strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
	input_error_flag = 1;
	}
else
	{
	strncpy(pinput_string_long,argv[1],strlen(pjh_inputs->pinput_filename) + 1);
	if (strcmp(pjh_inputs->pinput_filename,pinput_string_long) != 0)
		{
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sInput filename contains too many characters. Shorten filename to less than %d characters.\n",
		plocal_log_string_temp,MAXIMUM_INPUT_FILENAME_LINELENGTH + 1);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		input_error_flag = 1;
		}
	else
		{
		if ((fpw1 = fopen(pjh_inputs->pinput_filename,"r")) == NULL)
			{ 
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sFile \"%s\" does not exist!\nCheck and re-enter input file containing phase noise data.\n",plocal_log_string_temp,pjh_inputs->pinput_filename);
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			input_error_flag = 1;
			}  
		 else
			{
			strncpy(pinput_string,argv[2],LINELENGTH);
			if (atoi(pinput_string) < 1)
				{
				snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEnter a column number greater than 0. Read a value of %d.\n",plocal_log_string_temp,atoi(pinput_string));
				strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
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
			   	snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sInput file: %s\n",plocal_log_string_temp,pjh_inputs->pinput_filename);
			   	strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			   	if (find_stats_column_N_of_file(pjh_inputs->pinput_filename,pjh_inputs->column_number,&ave_input_signal,&min_input_signal,
					&max_input_signal,&pjh_inputs->number_of_input_lines,&sorted_flag,&slope_constant_flag) == EXIT_SUCCESS)
						{
						snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sColumn number: %d\n",plocal_log_string_temp,pjh_inputs->column_number);
						strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
						pjh_inputs->number_of_input_lines = pjh_inputs->number_of_input_lines;
						}
					else
						{
						snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sfind_stats_column_N_of_file() failed...exiting...\n",plocal_log_string_temp);
						strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
						exit(0);
						}
					if (input_error_flag != 1)
						{
						
						/* Validate output filename length */
						
						free(pinput_string_long);
						strncpy(pjh_inputs->poutput_filename,argv[3],MAXIMUM_INPUT_FILENAME_LINELENGTH);
						if((pinput_string_long = (char *) calloc(strlen(pjh_inputs->poutput_filename) + 2,sizeof(char))) == NULL)
							{
							snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sError allocating memory for pinput_string_long in check_jitterhist_inputs().\n",plocal_log_string_temp);
							strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
							input_error_flag = 1;
							}
						else
							{
							strncpy(pinput_string_long,argv[3],strlen(pjh_inputs->poutput_filename) + 1);
							if (strcmp(pjh_inputs->poutput_filename,pinput_string_long) != 0)
								{
								snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sOutput filename contains too many characters. Shorten filename to less than %d characters.\n",
								plocal_log_string,MAXIMUM_INPUT_FILENAME_LINELENGTH + 1);
								strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
								input_error_flag = 1;
								}
							else
								{
								snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sOutput file: %s\n",plocal_log_string_temp,pjh_inputs->poutput_filename);
								strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);							
								free(pinput_string_long);
										
								/* Read, assign, and check sample frequency */
								
								strncpy(pinput_string,argv[4],LINELENGTH);
								
								if (atof(pinput_string) <= 0)
									{
									snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sSample frequency (in GHz) must be greater than zero, re-enter value of %s.\n",
									plocal_log_string_temp,pinput_string);
									strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
									input_error_flag = 1;
									}
								else
									{
									if (input_error_flag != 1)
										{
										pjh_inputs->fs_GHz = atof(pinput_string);
										snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sSample frequency is %s.\n",
										plocal_log_string_temp,add_units_2((pjh_inputs->fs_GHz)*1e9,4,0,0,"Hz",value_string[0]));
										strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
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
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sThreshold value of %s exceeds maximum signal value of %s,\nre-enter a threshold value.\n",
		plocal_log_string_temp,add_units_2(atof(pinput_string),1,0,0,"V",value_string[0]),add_units_2(max_input_signal,3,0,0,"V",value_string[1]));
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		input_error_flag = 1;
		}
	else
		{
		if (atof(pinput_string) <= min_input_signal)
			{
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sThreshold value of %s is less than minimum signal value of %s,\nre-enter a threshold value.\n",
			plocal_log_string_temp,add_units_2(atof(pinput_string),1,0,0,"V",value_string[0]),add_units_2(min_input_signal,3,0,0,"V",value_string[1]));
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			input_error_flag = 1;
			}
		else
			{
			pjh_inputs->threshold = atof(pinput_string);
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sThreshold value is %s.\n",plocal_log_string_temp,add_units_2(pjh_inputs->threshold,3,0,0,"V",value_string[0]));
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Read and verify number of moving average samples */

	strncpy(pinput_string,argv[6],LINELENGTH);

	if (atoi(pinput_string) < 0)
		{
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sNumber of moving average samples read is less than zero. Re-enter value of 0 or positive value less than %ld. Read value of of %s.\n",
		plocal_log_string_temp,pjh_inputs->number_of_input_lines,pinput_string);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		input_error_flag = 1;
		}
	else
		{
		if (atol(pinput_string) > (long int) ceil(pjh_inputs->number_of_input_lines/10 - 1))
			{
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sNumber of moving average samples read is too large for the number of input samples. Re-enter value of 0 or positive value less than %.0f. Read value of %s.\n",plocal_log_string_temp,ceil(pjh_inputs->number_of_input_lines/10 - 1),pinput_string);
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			input_error_flag = 1;
			}
		else
			{
			pjh_inputs->num_moving_average_samples = atoi(pinput_string);
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sNumber of moving average samples = %d.\n",plocal_log_string_temp,pjh_inputs->num_moving_average_samples);
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			}
		}
	}

if (input_error_flag != 1)
	{	
	/* Read and verify if slope correction is desired and set flag to proper logic level */

	strncpy(pinput_string,argv[7],LINELENGTH);

	if ((tolower(input_string[0]) != 'n') && (tolower(input_string[0]) != 'y'))
		{
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEnter a \"y\" or \"n\" to apply slope correction to computed TIE.\n",plocal_log_string_temp);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sBoth the uncorrected and corrected values of TIE are provided in separate files.\nRead value of %s.\n",
		plocal_log_string_temp,pinput_string);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
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
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEnter a \"y\" or \"n\" to use entered frequency as average frequency\n",plocal_log_string_temp);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sin lieu of computed average frequency Read value of %s.\n",
		plocal_log_string_temp,pinput_string);
		strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
		input_error_flag = 1;
		}
	else
		{
		if ((tolower(input_string[0])) == 'n')
			{
			argc_max = 10;
			if ((argc != 10) && (argc != 11))
				{
				snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEnter value for average frequency to use. No value detected.\n",plocal_log_string_temp);
				strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
				input_error_flag = 1;
				}
			else
				{
				strncpy(pinput_string,argv[9],LINELENGTH);
				if (atof(pinput_string) <= 0.0)
					{
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEntered average frequency (in MHz) must be greater than zero,\n",plocal_log_string_temp);
					strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sRe-enter value entered value of %s.\n,",plocal_log_string_temp,pinput_string);
					strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
					input_error_flag = 1;
					}
				else
					{
					pjh_inputs->use_ave_freq_flag = 0;
					pjh_inputs->ave_freq_MHz = atof(pinput_string);
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sEntered average frequency is %s.\n",
					plocal_log_string_temp,add_units_2((pjh_inputs->ave_freq_MHz)*1e6,4,0,0,"Hz",value_string[0]));
					strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
					}
				}
			}
		else
			{
			argc_max = 9;
			pjh_inputs->use_ave_freq_flag = 1;
			}
		}
	}

if (input_error_flag != 1) 
	{
	/* Add verification of PSD window type if using an optional command line entry for window type */
	/* Examine optimize_window_flag state to determine if need to check window entries */
	if (pjh_inputs->file_entry_flag == 0)
		{
		if (argc == argc_max + 1)
			{
			strncpy(pinput_string,argv[argc - 1],LINELENGTH);
			if ((window_number = atol(pinput_string)) < (long int) MINIMUM_PSD_WINDOW_NUMBER)
				{
				snprintf(plocal_log_string,LOGFILE_LINELENGTH,
				"%sEntered window function number of %ld is less than the minimum value of %d.\n",
				plocal_log_string_temp,window_number,MINIMUM_PSD_WINDOW_NUMBER);
				strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
				input_error_flag = 1;
				}
			else
				{
				if (window_number > (long int) MAXIMUM_PSD_WINDOW_NUMBER)
					{
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,
					"%sEntered window function number of %ld is greater than the maximum value of %d.\n",
					plocal_log_string_temp,window_number,MAXIMUM_PSD_WINDOW_NUMBER);
					strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
					input_error_flag = 1;
					}
				else
					pjh_inputs->window_number = atol(pinput_string);
				}	
			}
		}
	}

if (input_error_flag != 1)
	{
	/* Add verification of PSD window type if using an ASCII input file in lieu of command line entries */
	/* Examine optimize_window_flag state to determine if need to check window entries */
	if (pjh_inputs->optimize_window_flag == 1)
		{
		if (pjh_inputs->number_of_psd_windows == 0)
				{
				snprintf(plocal_log_string,LOGFILE_LINELENGTH,"%sNo window function numbers entered.\n",
				plocal_log_string_temp);
				strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
				input_error_flag = 1;
				}
		if (input_error_flag != 0)
			{
			for (i = 0; i < pjh_inputs->number_of_psd_windows; i++)
				{
				if (pjh_inputs->psd_window_numbers_list[i] < (long int) MINIMUM_PSD_WINDOW_NUMBER)
					{
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,
					"%sEntered window function number of %ld is less than the minimum value of %d.\n",
					plocal_log_string_temp,pjh_inputs->psd_window_numbers_list[i],MINIMUM_PSD_WINDOW_NUMBER);
					strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
					input_error_flag = 1;
					}
				if (pjh_inputs->psd_window_numbers_list[i] > (long int) MAXIMUM_PSD_WINDOW_NUMBER)
					{
					snprintf(plocal_log_string,LOGFILE_LINELENGTH,
					"%sEntered window function number of %ld is greater than the maximum value of %d.\n",
					plocal_log_string_temp,pjh_inputs->psd_window_numbers_list[i],MAXIMUM_PSD_WINDOW_NUMBER);
					input_error_flag = 1;
					}
				if (input_error_flag == 1)
					break;
				}
			}
		}
	else
		{
		if (pjh_inputs->window_number < MINIMUM_PSD_WINDOW_NUMBER)
			{
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,
			"%sEntered window function number of %ld is less than the minimum value of %d.\n",
			plocal_log_string_temp,pjh_inputs->window_number,MINIMUM_PSD_WINDOW_NUMBER);
			strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
			input_error_flag = 1;
			}
		if (pjh_inputs->window_number > MAXIMUM_PSD_WINDOW_NUMBER)
			{
			snprintf(plocal_log_string,LOGFILE_LINELENGTH,
			"%sEntered window function number of %ld is greater than the maximum value of %d.\n",
			plocal_log_string_temp,pjh_inputs->window_number,MAXIMUM_PSD_WINDOW_NUMBER);
			input_error_flag = 1;
			}
		}
	}
			
strncpy(plog_string,plocal_log_string,LOGFILE_LINELENGTH);

if (input_error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}