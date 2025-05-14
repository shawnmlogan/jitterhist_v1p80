#include "jitterhist.h"

Boolean check_for_cr_only(char *pline)
{
Boolean found_cr_only = TRUE;
char *ptempline, templine[LINELENGTH+1];
int i = 0;

ptempline = &templine[0];

if (strchr(pline,'\n') != NULL) /*Identify if string contains a carriage return*/
   {
   strcpy(ptempline,pline);
   for (i=0;i < strlen(ptempline);i++)
      {
      if (isspace(templine[i]) == 0) /*Identify if any string elements are NOT white space*/
         {
         found_cr_only = FALSE;
         }
      }
   }
else
   found_cr_only = FALSE;

return found_cr_only;
}
