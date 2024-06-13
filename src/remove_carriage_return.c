#include "jitterhist.h"

void remove_carriage_return(char *pline)
{

char *ptempline;
int i = 0;

if ( (ptempline = (char *) calloc(strlen(pline) + 1,sizeof(char))) == NULL)
	{
	printf("Memory allocation error for string in remove_carriage_return().\n");
	exit(0);
	}
else
	{
	if (strchr(pline,'\n') != NULL) /*Identify if string contains a newline character*/
	   {
	   strcpy(ptempline,pline);
	   for (i=0;i < strlen(ptempline);i++)
	      {
	      if (ptempline[i] == '\n') /*Identify if string element is a newline character*/
	         ptempline[i] = '\0';
	      }
	   /*printf("Removed newline character of input \"%s\" to form \"%s\".\n",pline,ptempline);*/
	   strcpy(pline, ptempline);
	   
	   if (strchr(pline,'\r') != NULL) /*Identify if string contains a carriage return*/
		   {
		   strcpy(ptempline,pline);
		   for (i=0;i < strlen(ptempline);i++)
		      {
		      if (ptempline[i] == '\r') /*Identify if string element is a carriage return*/
		         ptempline[i] = '\0';
		      }
		   /*printf("Removed a carriage return from input \"%s\" to form \"%s\".\n",pline,ptempline);*/
		   strcpy(pline, ptempline);
		   }
	   }
	free(ptempline);
	}

}
