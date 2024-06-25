# include "jitterhist.h"

int find_base_filename(char *pfin, char *pbase_filename)
{

int error_flag = 0;
char *plocal_base_filename;

if ((plocal_base_filename = (char *) calloc(strlen(pfin) + 1,sizeof(char))) == NULL)
	{
	printf("Error allocating string for appended filename in find_base_filename()\n");
	error_flag = 1;
	}
else
	{
	strncpy(plocal_base_filename,pfin,strlen(pfin));
	plocal_base_filename[strlen(pfin) - 4] = '\0';
	strncpy(pbase_filename,plocal_base_filename,strlen(pfin));
	free(plocal_base_filename);
	}

if (error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
