#include "jitterhist.h"

char * add_units(double value,int num_digits,char *suffix,char *pvalue_string)
{

char *pvalue_string_local, value_string_local[LINELENGTH + 1];
pvalue_string_local = &value_string_local[0];

if (fabs(value) < 1e-9)
	{
	if (fabs(value) == 0)
		sprintf(pvalue_string_local,"%.*f %s",num_digits,value/1e-12,suffix);
	else
   	sprintf(pvalue_string_local,"%.*f p%s",num_digits,value/1e-12,suffix);
   }
else
   if (fabs(value) < 1e-6)
      sprintf(pvalue_string_local,"%.*f n%s",num_digits,value/1e-09,suffix);
   else
      {
      if (fabs(value) < 1e-3)
         sprintf(pvalue_string_local,"%.*f u%s",num_digits,value/1e-06,suffix);
      else
         {
         if(fabs(value) < 1.0)
            sprintf(pvalue_string_local,"%.*f m%s",num_digits,value/1e-03,suffix);
         else
            {
            if (fabs(value) < 1e3)
               sprintf(pvalue_string_local,"%.*f %s",num_digits,value,suffix);
            else
               {  
               if (fabs(value) < 1e6)
                  sprintf(pvalue_string_local,"%.*f k%s",num_digits,value/1e3,suffix);
               else
                  {
                  if (fabs(value) < 1e9)
                      sprintf(pvalue_string_local,"%.*f M%s",num_digits,value/1e6,suffix);
                  else
                     {
                      sprintf(pvalue_string_local,"%.*f G%s",num_digits,value/1e9,suffix);
                  }
               }
            }
         }
      }
   }

strncpy(pvalue_string,pvalue_string_local,LINELENGTH_OF_VALUE_STRING);
return pvalue_string;
}