#include "jitterhist.h"

void remove_whitespace_only(char *pstring)
{

/* Removes blanks and retains carriage returns from a string*/

char *ptemp_string_pointer = pstring;

do
	{
	while (*ptemp_string_pointer == ' ')
		{
		ptemp_string_pointer++;
		}
	}
while ((*pstring++ = *ptemp_string_pointer++));
}