#define EXIT_CHARS /* Include definition of EXIT_CHARS to declare exitchars[] and num_exitchars */
#include "jitterhist.h"

Boolean check_for_quit_characters(char *pline)
{
extern char exitchars[];
extern int num_exitchars;

Boolean flag_found_exitchar = FALSE;
char *ptempline;
int i = 0;

if ((ptempline = (char *) calloc(strlen(pline) + 1,sizeof(char))) == NULL)
	{
	printf("Error allocating memory for ptempfile in check_for_quit_characters()..exiting\n");
	exit(0);
	}

strcpy(ptempline,pline);
remove_whitespace(ptempline);

for(i=0;i<num_exitchars;i++)
	{
	if (ptempline[0] == exitchars[i])
		flag_found_exitchar = TRUE;
	}
	
free(ptempline);

return flag_found_exitchar;
}
