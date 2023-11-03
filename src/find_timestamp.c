
#include "jitterhist.h"

int find_timestamp(char *pdate_string,int max_characters)
{
time_t timestamp,*ptimestamp;
struct tm timeptr,*ptimeptr;

ptimestamp = &timestamp;
ptimeptr = &timeptr;

time(ptimestamp);
localtime_r(ptimestamp,ptimeptr);
strftime(pdate_string,max_characters,"%m%d%y_%H_%M_%S", ptimeptr);

return EXIT_SUCCESS;
}
