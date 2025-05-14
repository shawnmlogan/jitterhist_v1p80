#include "jitterhist.h"

int parsecsv_to_array(char *pdummy,long int *ppsd_windows_list,int *parray_size,int max_array_size, double range_min, double range_max)
{
/*Parses a comma separated variable string for long integers and places each
in an array that starts at pointer ppsd_windows_list. It returns the number of elements it
assigned to the array to the location pointed to by parray_size.
If more than  max_array_size are read in, it returns
EXIT_FAILURE, assigns array_size to max_array_size  and an error message*/

char *pdelim = ",";
char *ptoken;
char *plocal_input_string, local_input_string[MAXIMUM_INPUT_FILENAME_LINELENGTH + 1];
int i = 0, j = 0, duplicate_flag = 0, error_flag = 0;
long int *ptemp_pointer;
long int *ppartial_list, partial_list[MAXIMUM_NUMBER_OF_PSD_WINDOWS + 1]={0}; /* Allow one extra entry in list */

plocal_input_string = &local_input_string[0];
ppartial_list = &partial_list[0];

/* copy input string */

strncpy(plocal_input_string,pdummy,MAXIMUM_INPUT_FILENAME_LINELENGTH);

/*Save initial location - location 0 - of array pointed to by ppsd_windows_list*/
ptemp_pointer = ppsd_windows_list;

ptoken = strtok(pdummy,pdelim);

while ((ptoken != NULL) && (error_flag == 0))
   {
   *ppartial_list = atol(ptoken);
   if ((double) partial_list[i] < range_min)
   	{
   	printf("Ignoring window value of %ld which is less than the allowed minimum of %.0f. Check entry.\n",partial_list[i],range_min);
		ppartial_list--;
	   i--;
      }
   if ((double) partial_list[i] > range_max)
   	{
   	printf("Ignoring window value of %ld which is greater than the allowed maximum of %.0f. Check entry.\n",partial_list[i],range_max);
		ppartial_list--;
	   i--;
      }
   if (error_flag != 1)
   	{
	   duplicate_flag = 0;
		if (i > 0)
			{
			for (j = 0; j < i; j++)
				{
	   		if (partial_list[i] == partial_list[j])
	   			{
	   			duplicate_flag = 1;
	   			break;
	   			}
	   		}
	   	if (duplicate_flag != 1)
	   		{
	   		ppartial_list++;
	   		i++;
	   		}
	   	}
	   else
	   	{
	   	ppartial_list++;
	   	i++;
	   	}
	   if (((ptoken = strtok(NULL,pdelim)) == NULL) && (duplicate_flag == 1))
	   	partial_list[i] = 0;
	   if (i > max_array_size)
	      {
	      printf("Too many non-unique elements (> %d) on line \"%s\"!\n",
	      max_array_size,plocal_input_string);
	      error_flag = 1;
	      break;
	      }
	   }
   }

/* for (j = 0; j < i; j++)
	{
	if (j == 0)
		{
		printf("Detected %d valid window numbers.\n",i);
		printf("Valid window list:");
		}
	if (j != i - 1)
		printf(" %ld",partial_list[j]);
	else
		printf(" %ld\n",partial_list[j]);
	}
printf("Restoring pointer...\n"); */

/* ppartial_list = &partial_list[0];

for (j = 0; j < i; j++)
	{
	if (j == 0)
		{
		printf("Detected %d valid window numbers.\n",i);
		printf("Valid window list using pointers:");
		}
	if (j != i - 1)
		printf(" %ld",*ppartial_list++);
	else
		printf(" %ld\n",*ppartial_list++);
	} */

ppartial_list = &partial_list[0];
*parray_size = i;

for (j = 0; j < i; j++)
	*ppsd_windows_list++ = *ppartial_list++;

/* Restore pointer to point to first element in array */

ppsd_windows_list = ptemp_pointer;

if (error_flag != 0)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}

   
   
