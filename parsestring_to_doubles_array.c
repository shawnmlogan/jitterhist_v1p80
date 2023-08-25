
#include "jitterhist.h"

int parsestring_to_doubles_array(char *pinput_string,double *pdoubles_array,int *parray_size,int max_array_size)
{
/*Parses a comma separated variable string for doubles and places each
double in an array that starts at pointer pdoubles_array. It returns up to max_array_size elements it
assigned to the array to the location pointed to by parray_size. If less than max_array_size valid
entires are read in, it returns EXIT_FAILURE*/

char delim = ',';
char string_char,*plocal_string, local_string[LINELENGTH + 1],*pdouble_string, double_string[LINELENGTH + 1];
char *ptemp, temp[LINELENGTH + 1];
int i = 0, j = 0, skip_string_flag = 0;
double *ptemp_pointer, temp_double = 0.0;

plocal_string = &local_string[0];
pdouble_string = &double_string[0];
ptemp = &temp[0];

plocal_string = pinput_string;

*parray_size = 0;

/*Save initial location - location 0 - of array pointed to by pdoubles_array*/

ptemp_pointer = pdoubles_array;

i = 0;
j = 0;
skip_string_flag = 0;
do
	{
	string_char = local_string[i];
	double_string[j] = string_char;
	/*printf("Read character \"%c\".\n",string_char);*/
	if ((string_char == delim) || (i == strlen(pinput_string)))
		{
		double_string[j] = '\0';
		temp_double = strtod(pdouble_string,&ptemp);
		/*printf("j = %d, \"%s\", stopped at \"%s\", strlen(ptemp) = %lu.\n",j,pdouble_string,ptemp,strlen(ptemp));*/
		if (strlen(ptemp) == 0)
			{
			*ptemp_pointer = temp_double;
			ptemp_pointer++;
			*parray_size = *parray_size + 1;
			j = -1;				
			}
		else
			skip_string_flag = 1;
		}
	i++;
	j++;
	}
while ( (i < (strlen(plocal_string) + 1)) && (skip_string_flag != 1) && (*parray_size < max_array_size));


if (skip_string_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}

   
   
