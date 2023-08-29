#include "jitterhist.h"

int moving_average(char *pfilein, char *pfileout,int number_of_data_columns,int num_moving_average_samples,long int *pnumber_output_lines)
{

char *pinput_string, input_string[LINELENGTH + 1];

int tokens, error_flag = 0;
long int i = 0, j = 0, k = 0, number_of_input_lines = 0;

double *pdoubles_array, sample_value = 0.0;
double **pinput_data_array;

FILE *fpw1, *fpw2;

pinput_string = &input_string[0];

if ((pdoubles_array = (double *) calloc(number_of_data_columns,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in moving_average()...exiting\n");
	error_flag = 1;
	}
else
	{
	/* Count number of valid lines in input file*/
	
	fpw1 = fopen(pfilein,"r");
	while(!feof(fpw1))
		{
		fgets(pinput_string,LINELENGTH,fpw1);
		remove_carriage_return(pinput_string);
		if (!feof(fpw1))
			{

			if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
				number_of_input_lines++;
			}
		}
	fclose(fpw1);
	
	/* printf("In moving_average, found %ld valid lines in file \"%s\".\n",number_of_input_lines,pfilein);*/
	
	/* Allocate memory for data array and place input file data in array */
	
	if ((pinput_data_array = malloc(number_of_input_lines * sizeof(double *))) == NULL)
		{
		printf("Error allocating memory for **pinput_data_array in moving_average()..exiting\n");
		error_flag = 1;
		}
	else
		{
		for(i = 0; i < number_of_input_lines; i++)
			{
			if ((pinput_data_array[i] = malloc(number_of_data_columns * sizeof(double))) == NULL)
				error_flag = 1;
			}
		if (error_flag == 1)
			printf("Error allocating memory for a member of pointer array pinput_data_array[] in moving_average()..exiting\n");
		else
			{
			i = 0;
			j = 0;
			fpw1 = fopen(pfilein,"r");
			
			while(!feof(fpw1))
				{
				fgets(pinput_string,LINELENGTH,fpw1);
				remove_carriage_return(pinput_string);
				if (!feof(fpw1))
					{
					if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
						{ 
						for(j = 0; j < number_of_data_columns;j++)
							{
							pinput_data_array[i][j] = pdoubles_array[j];
							}
						i++;
						}
					}
				}
			fclose(fpw1);

			i = 0;
			j = 0;
			fpw1 = fopen(pfilein,"r");
			fpw2 = fopen(pfileout,"w");
			
			for(i = num_moving_average_samples; i < (number_of_input_lines - num_moving_average_samples);i++)
				{
				for(j = 0; j < number_of_data_columns;j++)
					{
					sample_value = 0.0;
					for (k = (0 - num_moving_average_samples);k < (num_moving_average_samples + 1);k++)
						{
						sample_value = sample_value + pinput_data_array[i + k][j];
						}
					sample_value = sample_value/((double) (1 + 2*num_moving_average_samples));
					if (j != (number_of_data_columns - 1))
						fprintf(fpw2,"%1.12e,",sample_value);
					else
						fprintf(fpw2,"%1.12e\n",sample_value);
					}
				}
			fclose(fpw1);
			fclose(fpw2);
			
			for(i = 0; i < number_of_input_lines; i++)
				free(pinput_data_array[i]);
			}
		}
	}

if (error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}