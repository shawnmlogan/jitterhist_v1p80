#include "jitterhist.h"

void remove_whitespace(char *pstring)
{

/* Removes both blanks an carriage returns from a string*/

char *ptemp_string_pointer = pstring;

do
	{
	while ((*ptemp_string_pointer == ' ') || (*ptemp_string_pointer == '\n'))
		{
		ptemp_string_pointer++;
		}
	}
while ((*pstring++ = *ptemp_string_pointer++));
}