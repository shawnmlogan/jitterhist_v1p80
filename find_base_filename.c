# include "jitterhist.h"

int find_base_filename(char *pfin, char *pbase_filename, int max_num_characters)
{

int i = 0, error_flag = 0;
char *plocal_fin, local_fin[LINELENGTH + 1];
char *plocal_base_filename;

plocal_fin = &local_fin[0];

if ((plocal_base_filename = (char *) calloc(max_num_characters + 1,sizeof(char))) == NULL)
	{
	printf("Error allocating string for appended filename in append_filename_keep_extension()\n");
	error_flag = 1;
	}
else
	{
	strcpy(plocal_fin,pfin);

	/* Create base file name, remove 3 digit extension */
	
	for (i = 0; i < strlen(plocal_fin);i++)
		{
		if (i < strlen(plocal_fin) - 4)
			{
			plocal_base_filename[i] = local_fin[i];
			}
		else
			{
			plocal_base_filename[i] = '\0';
			}
		}
	strcpy(pbase_filename,plocal_base_filename);
	free(plocal_base_filename);
	}

if (error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
