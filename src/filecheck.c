#include "jitterhist.h"

int filecheck(char *pfilename)
{
extern char exitchars[];
extern int num_exitchars;
int i;
char *pinput_string_long, input_string_long[MAXIMUM_INPUT_FILENAME_LINELENGTH + 2];
char *pline, line[MAXIMUM_INPUT_FILENAME_LINELENGTH + 1];
char *plocal_log_string,local_log_string[LOGFILE_LINELENGTH + 1];
char *plocal_log_string_temp,local_log_string_temp[LOGFILE_LINELENGTH + 1];
Boolean flag_to_quit = FALSE, found_CR = FALSE, found_valid_filename = FALSE, return_on_CR = FALSE;
struct stat file_info;
FILE *fp;

pline = &line[0];
pinput_string_long = &input_string_long[0];
plocal_log_string = &local_log_string[0];
plocal_log_string_temp = &local_log_string_temp[0];

strncpy(pline,pfilename,MAXIMUM_INPUT_FILENAME_LINELENGTH); /* copy file pointer to local pointer*/
strncpy(pinput_string_long,pfilename,MAXIMUM_INPUT_FILENAME_LINELENGTH + 1); 

if (strcmp(pline,pinput_string_long) != 0)
	{
	printf("Input filename contains too many characters. Shorten filename to less than %d characters.\n",
	MAXIMUM_INPUT_FILENAME_LINELENGTH);
	found_valid_filename = FALSE;
	}
else
	{
	do
	   {
		if (check_for_cr_only(pline) == TRUE)
	      {
			found_CR = TRUE;
			printf("Re-enter filename or ");
			for (i = 0;i < num_exitchars;i++)
				{
				if(i != (num_exitchars - 1))
	      		printf("\"%c\" or ",exitchars[i]);
	      	else
	      		printf("\"%c\" to exit.\n",exitchars[i]);
	      	}
			}
		else
			{
			remove_carriage_return(pline);
		   if (check_for_quit_characters(pline) == TRUE)
	      	flag_to_quit = TRUE;
			else
				{
				if ((stat(pline,&file_info) == 0) && S_ISDIR(file_info.st_mode))
					{
					printf("\"%s\" is a directory!\nRe-enter a filename or ",pline);
					for (i = 0;i < num_exitchars;i++)
		      		{
		      		if(i != (num_exitchars - 1))
		      			printf("\"%c\" or ",exitchars[i]);
		      		else
		      			printf("\"%c\" to exit.\n",exitchars[i]);
		      		}
					}
				else
					{
			   	if ((fp = fopen(pline,"r")) == NULL)
			      	{
			      	printf("File \"%s\" does not exist!\nRe-enter filename or ",pline);
			      	for (i = 0;i < num_exitchars;i++)
			      		{
			      		if(i != (num_exitchars - 1))
			      			printf("\"%c\" or ",exitchars[i]);
			      		else
			      			printf("\"%c\" to exit.\n",exitchars[i]);
			      		}
			      	}
			   	else
			      	{
			      	found_valid_filename = TRUE;
			      	/*Remove carriage return and white space since no longer need to detect CR only*/
			      	remove_whitespace(pline);
			      	}
			      }
			   }
		   }
		if ((flag_to_quit != TRUE) && (found_valid_filename != TRUE))
			{
			fflush(stdin);
	   	fgets(pinput_string_long,MAXIMUM_INPUT_FILENAME_LINELENGTH + 1,stdin);
	   	strncpy(pline,pinput_string_long,MAXIMUM_INPUT_FILENAME_LINELENGTH);
			if (strcmp(pline,pinput_string_long) != 0)
				{
				snprintf(plocal_log_string,LOGFILE_LINELENGTH,
				"%sInput filename contains too many characters. Shorten filename to less than %d characters.\n",
				plocal_log_string_temp,MAXIMUM_INPUT_FILENAME_LINELENGTH + 1);
				strncpy(plocal_log_string_temp,plocal_log_string,LOGFILE_LINELENGTH);
				found_valid_filename = FALSE;
				}
			}
	   	remove_whitespace_only(pline);
	   	}
		while ((found_valid_filename == FALSE) && (flag_to_quit == FALSE));
		}

if ((flag_to_quit == FALSE) && (found_valid_filename = TRUE))
	{
   return EXIT_SUCCESS;
   }
else
	return EXIT_FAILURE;

}