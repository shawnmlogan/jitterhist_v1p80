# include "jitterhist.h"

int append_filename_keep_extension(char *pfin, char *pfout_appended, char *pappended_string, int max_num_characters)
{

int i = 0, error_flag = 0;
char *plocal_fin, *plocal_fout_appended;
char *pextension_string, extension_string[LINELENGTH + 1];

pextension_string = &extension_string[0];

if ((plocal_fout_appended = (char *) calloc(max_num_characters + 1,sizeof(char))) == NULL)
	{
	printf("Error allocating string for appended filename in append_filename_keep_extension()\n");
	error_flag = 1;
	}
else
	{
	if ((plocal_fin = (char *) calloc(strlen(pfin) + 1,sizeof(char))) == NULL)
		{
		printf("Error allocating string for local filename in append_filename_keep_extension()\n");
		error_flag = 1;
		}
	else
		{
		strcpy(plocal_fin,pfin);

	/* Create appended file name, remove 3 digit extension and append */
	/* "pappended_string" and replace extension */
	
		for (i = 0; i < strlen(plocal_fin);i++)
			{
			if (i < strlen(plocal_fin) - 4)
				{
				plocal_fout_appended[i] = plocal_fin[i];
				}
			else
				{
				plocal_fout_appended[i] = '\0';
				extension_string[i - (strlen(plocal_fin) - 4)] = plocal_fin[i];
				}
			}
		extension_string[i - (strlen(plocal_fin) - 4)] = '\0';
		strncat(plocal_fout_appended,pappended_string,max_num_characters - strlen(pextension_string));
		strncat(plocal_fout_appended,pextension_string,max_num_characters);
		strcpy(pfout_appended,plocal_fout_appended);
		free(plocal_fin);
		free(plocal_fout_appended);
		}
	}

if (strlen(pfin) + strlen(pappended_string) > max_num_characters)
	{
	printf("Filename was truncated as the desired number of characters %lu exceeded %d.\n",
	strlen(pfin) + strlen(pappended_string),max_num_characters);
	printf("Truncated filename is: \"%s\".\n",pfout_appended);
	}

if (error_flag == 1)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
