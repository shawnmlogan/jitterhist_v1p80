#include "jitterhist.h"

void remove_carriage_return(char *pline)
{

char *ptempline, templine[LINELENGTH+1];
int i = 0;

ptempline = &templine[0];

if (strchr(pline,'\n') != NULL) /*Identify if string contains a carriage return*/
   {
   strcpy(ptempline,pline);
   for (i=0;i < strlen(ptempline);i++)
      {
      if (templine[i] == '\n') /*Identify if string element is a newline*/
         templine[i] = '\0';
      }
   /*printf("Removed carriage return of input \"%s\" to form \"%s\".\n",pline,ptempline);*/
   strcpy(pline, ptempline);
   }

}
