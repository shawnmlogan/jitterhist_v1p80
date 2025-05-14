#include "jitterhist.h"

void print_string_to_log(char *pstring, jitterhist_inputs *pjitterhist_inputs)
{

FILE *fpw1;

printf("%s",pstring);

fpw1 = fopen(pjitterhist_inputs->plog_filename,"a");
fprintf(fpw1,"%s",pstring);
fclose(fpw1);

}