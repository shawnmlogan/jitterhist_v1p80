#include "jitterhist.h"

int pop_data(char *pfin, char **ppfile_argv,jitterhist_inputs *pjh_inputs)
{
char *ptempstring, tempstring[MAXIMUM_INPUT_FILENAME_LINELENGTH + 1], *pidentifier, identifier[MAXIMUM_INPUT_FILENAME_LINELENGTH+1];
char *pentry, entry[MAXIMUM_INPUT_FILENAME_LINELENGTH + 1], colon =':';
int i = 0, j = 0, N = 0, number_of_entries_detected = 0, error_flag = 0;
FILE *fpw1;

ptempstring = &tempstring[0];
pidentifier = &identifier[0];
pentry = &entry[0];


/*Already verified that file pointed to by pfin exists and filename has */
/* fewer than MAXIMUM_INPUT_FILENAME_LINELENGTH characters */

fpw1 = fopen(pfin,"r");

while (!feof(fpw1))
   {
   fgets(ptempstring,MAXIMUM_INPUT_FILENAME_LINELENGTH,fpw1);
   if (!feof(fpw1))
   	{
	   /*Read line indentifier - skip line if first entry is a newline or a "*" */
	   /* or line does not contain a colon */
	   if (tempstring[0] != '\n' && tempstring[0] != '*' && strchr(ptempstring,colon))
	      {
	      for (i = 0; tempstring[i] != ':'; i++)
	         {
	         identifier[i] = tolower(tempstring[i]);
	         }
	      /*Add string termination*/
	      identifier[i] = '\0';
	
	      /*Put rest of string in tempstring*/
	      
	      N = i;
	      j = 0;
	      for (i = N + 1;tempstring[i] != '\0'; i++)
	         {
	         if ((tempstring[i] != '\n') && (tempstring[i] != ' '))
	            {
	            entry[j] = tempstring[i];
	            j++;
	            }
	          }
	      entry[j] = '\0';
	      if (assign(pidentifier,pentry,ppfile_argv,pjh_inputs) == EXIT_FAILURE)
	         {
	         printf("Identifier \"%s\" in input file is unknown.\nCheck input file \"%s\"\n",
	         pidentifier, pfin);
	         error_flag = 1;
	         }
	      else
	      	number_of_entries_detected++;
	      /* printf("Identifier \"%s\" is: \"%s\"\n",pidentifier,pentry); */
	      }
	   }
   }
fclose(fpw1);

if (number_of_entries_detected == 0)
	{
	printf("Did not detect any valid lines in input file \"%s\".\n",pfin);
	printf("Please check file contents and try again...\n");
	error_flag = 1;
	}

if (strlen(ppfile_argv[1]) == 0)
	{
	printf("Did not detect an input data filename in input file \"%s\".\n",pfin);
	printf("Please check file contents and try again...\n");
	error_flag = 1;
	}

if (error_flag != 0)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
}
      
      
