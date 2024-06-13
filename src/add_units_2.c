#include "jitterhist.h"

char * add_units_2(double value,int num_digits,int width,int min_string_length,char *suffix,char *pvalue_string)
{
int i;
char *pvalue_string_local, value_string_local[LINELENGTH + 1];
pvalue_string_local = &value_string_local[0];


if (fabs(value) < 1e-15)
	{
	if (fabs(value) == 0)
		snprintf(pvalue_string_local,LINELENGTH,"%*.*f %s",width,num_digits,value/1e-12,suffix);
	else
   	snprintf(pvalue_string_local,LINELENGTH,"%*.*f a%s",width,num_digits,value/1e-12,suffix);
   }
else
   if (fabs(value) < 1e-12)
      snprintf(pvalue_string_local,LINELENGTH,"%*.*f f%s",width,num_digits,value/1e-15,suffix);
   else
   	{
   	if (fabs(value) < 1e-9)
      	snprintf(pvalue_string_local,LINELENGTH,"%*.*f p%s",width,num_digits,value/1e-12,suffix);
      else
      	{
		   if (fabs(value) < 1e-6)
		      snprintf(pvalue_string_local,LINELENGTH,"%*.*f n%s",width,num_digits,value/1e-09,suffix);
		   else
		      {
		      if (fabs(value) < 1e-3)
		         snprintf(pvalue_string_local,LINELENGTH,"%*.*f u%s",width,num_digits,value/1e-06,suffix);
		      else
		         {
		         if(fabs(value) < 1.0)
		            snprintf(pvalue_string_local,LINELENGTH,"%*.*f m%s",width,num_digits,value/1e-03,suffix);
		         else
		            {
		            if (fabs(value) < 1e3)
		               snprintf(pvalue_string_local,LINELENGTH,"%*.*f %s",width,num_digits,value,suffix);
		            else
		               {  
		               if (fabs(value) < 1e6)
		                  snprintf(pvalue_string_local,LINELENGTH,"%*.*f k%s",width,num_digits,value/1e3,suffix);
		               else
		                  {
		                  if (fabs(value) < 1e9)
		                      snprintf(pvalue_string_local,LINELENGTH,"%*.*f M%s",width,num_digits,value/1e6,suffix);
		                  else
		                     {
		                      snprintf(pvalue_string_local,LINELENGTH,"%*.*f G%s",width,num_digits,value/1e9,suffix);
		                  }
		               }
		            }
		         }
		      }
		   }
		}
	}

if (strlen(pvalue_string_local) < min_string_length)
	{
	for( i = strlen(pvalue_string_local); i < min_string_length; i++)
		{
		value_string_local[i] = ' ';
		}
	value_string_local[i] = '\0';
	}

strncpy(pvalue_string,pvalue_string_local,LINELENGTH_OF_VALUE_STRING);
return pvalue_string;
}