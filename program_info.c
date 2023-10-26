#include "jitterhist.h"

void program_info(int verbose)
{
printf("Version %.3f %s\n",VERSION,VERSION_DATE);

if (verbose == 1)
	{
	printf("This program reads samples of a waveform and computes the output time interval error.\n");
	printf("\ta. Updated for arbitrary threshold 10/11/99.\n");
	printf("\tb. Updated to allow user to enter alternate frequency to compute jitter 12/12/99.\n");
	printf("\tc. Updated to eliminate prompts and allow for command line entries 8/23/04.\n");
	printf("\td. Updated to add linear interpolation to increase accuracy of zero-crossings 4/6/11.\n");
	printf("\te. Updated to add routine to remove slope of output jitter characteristic\n\t\
	and re-compute jitter and jitter statistics. 8/21/2023\n"); 
	printf("\tf. Updated to add optional moving average feature to improve\n\t\
	accuracy of zero-crossings 8/23/2023.\n");
	printf("\tg. Updated to include input to optionally compute slope and intercept of computed\n\t\
	TIE edges and remove the slope from the computed TIE.\n");
	printf("\th. Updated to include feature to include linear search for the number of\n\t");
	printf("moving average samples that eliminate closely spaced multiple threshold crossings\n\t");
	printf("which result in incorrect average duty cycle and average frequency 8/27/2023.\n");
	printf("\ti. Updated to include check that there are sufficient input data samples to\n\t");
	printf("accommodate the number of requested moving average samples\n\n");
	printf("\tj. Updated to include columm number of input data and to plot and\n\t");
	printf("annotate jitter or corrected jitter if requested. Fixed numerous bugs\n\t");
	printf("in parsestring_to_doubles_array.c, find_zero_crossings, add_units.c, and datascan.c.\n\t");
	printf("Added new functions check_executable(),find_base_filename(),\n\t);
	printf("and find_stats_column_N_of_file()\n\n");
	}
}
