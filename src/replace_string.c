
#include "jitterhist.h"

int replace_string(char *pinput_string,char *poutput_string,char *porig_string,char *pnew_string,int max_output_string_length)
{
int i, j, k, l, output_string_length = 0, error_flag = 0;
int number_occurences_of_orig_string = 0, orig_string_start_location[100];

char *ptemp_inputstring,temp_inputstring[max_output_string_length + 1];
char *plocal_input_string,local_input_string[max_output_string_length + 1];
char *plocal_new_string,local_new_string[max_output_string_length + 1];
char *ptemp_outputstring,temp_outputstring[max_output_string_length + 1];

ptemp_inputstring = &temp_inputstring[0];
plocal_input_string = &local_input_string[0];
ptemp_outputstring = &temp_outputstring[0];
plocal_new_string = &local_new_string[0];

strcpy(plocal_input_string,pinput_string);
strcpy(plocal_new_string,pnew_string);

/* Locate character positions of string to replace within input string */
/* Store locations in orig_string_start_location[] */
	
strncpy(ptemp_inputstring,pinput_string,max_output_string_length);
j = 0;

if (strstr(ptemp_inputstring,porig_string) != NULL)
	{
	for (i = 0; strstr(ptemp_inputstring,porig_string) != NULL; i++)
	{
	if (strcmp(ptemp_inputstring,strstr(ptemp_inputstring,porig_string)) == 0)
		{
		orig_string_start_location[j] = i;
		/* printf("orig_string_start_location[%d] = %d. strlen(porig_string) = %lu.\n",j,orig_string_start_location[j],strlen(porig_string)); */
		j++;
		}
	ptemp_inputstring++;
	}
	
	number_occurences_of_orig_string = j;
	output_string_length = strlen(pinput_string) + number_occurences_of_orig_string * (strlen(pnew_string) - strlen(porig_string));
	
	if (output_string_length > max_output_string_length)
		{
		printf("Output string length of %d will be greater than max_output_string_length of %d in repstring().\n",output_string_length,max_output_string_length);
		printf("String replacement not performed\n");
		error_flag = 1;
		}
	else
		{
		j = 0;
		k = 0;
		for (i = 0; i < strlen(pinput_string); i++)
			{
			if ((i == orig_string_start_location[j]) && (j <number_occurences_of_orig_string))
				{
				for (l = 0; l < strlen(pnew_string); l++)
					{
					temp_outputstring[k] = local_new_string[l];
					k++;
					}
					i = i + strlen(porig_string) - 1; /* Skip over remaining parts of original string */
				j++;
				}
			else
				{
				temp_outputstring[k] = local_input_string[i];
				/* printf("Assigning temp_outputstring[%d] as %c.\n",k,temp_outputstring[k]); */
				k++;
				}
			}
		
		/* Add null character to end of string */
		
		temp_outputstring[k] = '\0';
		
		strcpy(poutput_string,ptemp_outputstring);
		}
	}
else
	strcpy(poutput_string,pinput_string); /* There were no incidences of porig_string in pinput_string */

if (error_flag != 0)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}