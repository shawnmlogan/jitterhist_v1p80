#include "jitterhist.h"

long int parse_3_column_csv_file(char *pinput_filename,long int start_line,long int stop_line,int x_column_number,int y_column_number,int z_column_number,double *px,double *py,double *pz)

{
int tokens = 0, max_column_number;
long int i, j, number_of_input_lines ;

char *pinput_string,input_string[LINELENGTH + 1];

double *pdoubles_array,doubles_array[MAXIMUM_NUMBER_OF_DATA_COLUMNS];
FILE *fpw1;

pinput_string = &input_string[0];
pdoubles_array = &doubles_array[0];

if (x_column_number > y_column_number)
	{
	if (z_column_number > x_column_number)
		max_column_number = z_column_number;
	else
		max_column_number = x_column_number;
	}
else
	{
	if (z_column_number > y_column_number)
		max_column_number = z_column_number;
	else
		max_column_number = y_column_number;
	}
if (max_column_number > MAXIMUM_NUMBER_OF_DATA_COLUMNS)
	{
	printf("There are too many columns in input file \"%s\" to parse.\n",pinput_filename);
	printf("Enter a file with fewer than %d columns, exiting without parsing file.\n",
	MAXIMUM_NUMBER_OF_DATA_COLUMNS + 1);
	number_of_input_lines = 0;
	}
else
	{
	i = 0;
	fpw1 = fopen(pinput_filename,"r");
	do
		{
		fgets(pinput_string,LINELENGTH,fpw1);
		remove_carriage_return(pinput_string);
		if (!feof(fpw1))
			{
			if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,max_column_number)) == EXIT_SUCCESS)
				{
				if ((i > start_line - 1) && (i < stop_line + 1))
					{
					#ifdef DEBUG_PARSE_3_COLUMN_CSV_FILE
						if (i == start_line)
							printf("Starting to read data into arrays at line i = %ld.\n",i);
					#endif	
					j = i - start_line;
					px[j] = doubles_array[x_column_number - 1];
					py[j] = doubles_array[y_column_number - 1];
					pz[j] = doubles_array[z_column_number - 1];
					#ifdef DEBUG_PARSE_3_COLUMN_CSV_FILE
						printf("j = %ld, px[%ld] = %1.6e py[%ld] = %1.6e pz[%ld] = %1.6e.\n",
						j,j,px[j],j,py[j],j,pz[j]);
					#endif
					if (!isnan(px[j]) && !isnan(py[j]) && !isnan(pz[j]))
						j++;
					}
				i++;
				}
			}
		}
	while(!feof(fpw1));
	
	fclose(fpw1);
	
	number_of_input_lines = j;
	}

return number_of_input_lines;

}