#include "jitterhist.h"

int validate_input_file(char *pfin, int *column_number, long int *number_of_input_lines)
{
int input_error_flag = 0;
int tokens = 0;

char *pinput_string, input_string[LINELENGTH + 1];
char *pinput_string_long, input_string_long[LINELENGTH + 2];

double *pdoubles_array,doubles_array[MAXIMUM_NUMBER_OF_DATA_COLUMNS];

pinput_string = &input_string[0];
pinput_string_long = &input_string_long[0];

FILE *fpw1;
				
/* Check to verify sufficient columns exist in input file */

if ((pdoubles_array = (double *) calloc(*column_number,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in validate_input_file()...exiting\n");
	exit(0);
	}
else
	{
	fpw1 = fopen(pfin,"r");
	while(!feof(fpw1) && (input_error_flag == 0))
		{
		fgets(pinput_string_long,LINELENGTH + 2,fpw1);
		if ((int) strlen(pinput_string_long) > LINELENGTH)
			{
			input_error_flag = 1;
			printf("Reduce line length of input data file \"%s\" to less than %d characters.\n",pfin,LINELENGTH);
			}
		else
			{
			strncpy(pinput_string,pinput_string_long,LINELENGTH);
			remove_carriage_return(pinput_string);
			if (!feof(fpw1))
				{
				if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,*column_number)) == EXIT_SUCCESS)
					{
					if (tokens != *column_number)
						{
						printf("Insufficient columns in file \"%s\" to support column %d\n",pfin,*column_number);
						printf("Enter a column number less than or equal to %d.\n",tokens);
						input_error_flag = 1;
						}
					else
						(*number_of_input_lines)++;
					}
				}
			}
		}
		fclose(fpw1);
		free(pdoubles_array);
		}
	
if (input_error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}