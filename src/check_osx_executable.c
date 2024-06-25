#include "jitterhist.h"

int check_osx_executable(char *pprogram_executable,char *preturn_string)
{
int found_executable_flag = 0;
char *pstring, string[LINELENGTH + 1],*pnot_found = "1";


FILE *fpw1;

pstring = &string[0];

#if defined(__APPLE__) && defined(__MACH__)
	/* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
	/* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
	/* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
	snprintf(pstring,LINELENGTH,"if [ -x \"/Applications/%s.app\" ]; then echo \"0\"; else echo \"1\"; fi || echo \"%s\";\n",pprogram_executable,pnot_found);
	fpw1 = popen(pstring,"r");
	fgets(preturn_string,LINELENGTH,fpw1);
	remove_carriage_return(preturn_string);
	pclose(fpw1);
	
	if (strcmp(preturn_string,pnot_found) == 0)
	   {
	   /*printf("Program executable \"%s\" is not found.\n",pprogram_executable);*/
	   found_executable_flag = 0;
	   }
	else
	   {
	   /*printf("Program executable \"%s\" is located at \"%s\".\n",pprogram_executable,preturn_string);*/
	   found_executable_flag = 1;
	   }
	remove(pnot_found);
#endif
#endif




return found_executable_flag;

}
