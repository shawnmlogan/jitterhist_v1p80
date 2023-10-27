#include "jitterhist.h"

int main(int argc,char *argv[])
{
int i, writefile = 1, glitch_flag = 0;
int tokens = 0, use_ave_freq_flag = 0, correct_slope_flag = 0;
int verbose = 0, column_number = 1;
int num_moving_average_samples = 0;
long int number_of_input_lines = 0, number_of_output_lines = 0;

char *ptempfile, tempfile[LINELENGTH + 1];
char fin[LINELENGTH+1],fout[LINELENGTH+1],fout_corrected[2*LINELENGTH + 1],*pfin,*pfout,*pfout_corrected;
char *pbase_filename, base_filename[LINELENGTH+1];
char *pinput_string, input_string[LINELENGTH+1];
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];
char *pgnuplot = "gnuplot",*pgnuplot_path, gnuplot_path[LINELENGTH + 1];
char *pgimp = "gimp",*pgimp_path, gimp_path[LINELENGTH + 1];
char *pgnuplot_command_1, gnuplot_command_1[LINELENGTH + 1];
char *pgnuplot_command_2, gnuplot_command_2[LINELENGTH + 1];
char *ptitle_string, title_string[LINELENGTH + 1];

double fs_GHz, deltat = 0.0;
double ave_freq_MHz = 0.0;
double max_pp_pos_edge_error = 0.0,*pmax_pp_pos_edge_error, max_pp_neg_edge_error = 0.0, *pmax_pp_neg_edge_error;
double duty_cycle_max_percent, duty_cycle_min_percent;
double slope = 0.0, intercept = 0.0;
double threshold = 0.0;
double *pdoubles_array;
double slope_pos_edge, intercept_pos_edge, slope_neg_edge, intercept_neg_edge;
double corrected_neg_edge_tie_max = BIG_NEG_NUM, corrected_neg_edge_tie_min = BIG_POS_NUM, corrected_neg_edge_tie;
double corrected_pos_edge_tie_max = BIG_NEG_NUM, corrected_pos_edge_tie_min = BIG_POS_NUM, corrected_pos_edge_tie;
xy_pair *pxy, *px1y1;
zero_crossing_stats *pzc_stats;

FILE *fpw1,*fpw2;

/*Assign pointers*/

pfin = &fin[0];
pfout = &fout[0];
pfout_corrected = &fout_corrected[0];
pbase_filename = &base_filename[0];
pinput_string = &input_string[0];
pmax_pp_pos_edge_error = &max_pp_pos_edge_error;
pmax_pp_neg_edge_error = &max_pp_neg_edge_error;
ptempfile = &tempfile[0];
pgimp_path = &gimp_path[0];
pgnuplot_path = &gnuplot_path[0];
pgnuplot_command_1 = &gnuplot_command_1[0];
pgnuplot_command_2 = &gnuplot_command_2[0];
ptitle_string = &title_string[0];

if (((pzc_stats = (zero_crossing_stats *) calloc(1,sizeof(zero_crossing_stats))) == NULL) )
	{
	printf("Error allocating memory to pzc_stats array...exiting...\n");
	exit(0);
	}

strcpy(ptempfile,"./.tempfilej");

/*Explain program*/

program_info(verbose);

/*Command line check*/
 
if (argc < 9 || argc > 11)
   {
   printf("Usage: jitterhist <inputfile> <column_number> <outputfile> <Sample frequency (GHz)> ");
   printf("<vthreshold> <num_moving_average_samples> <correct_slope?(y|n)> <Use ave period?(y|n)> <Freq_to_use (MHz) if \"n\">\n");
   exit(0);
   }
else
   {
   
	/* Check inputs and count number of valid input lines in input file column 1 */
	
	if( check_inputs(argv,argc,pfin,&column_number,pfout,&fs_GHz,&threshold,
	&num_moving_average_samples,&use_ave_freq_flag,&ave_freq_MHz,&number_of_input_lines,&correct_slope_flag) != EXIT_SUCCESS)
		{
		exit(0);
		}
   }

/*Set deltat*/

deltat = (1/(fs_GHz*1e9));

if (num_moving_average_samples > 0)
	{
	i = 0;
	do
		{
		if (moving_average(pfin,ptempfile,column_number,num_moving_average_samples,&number_of_output_lines) != EXIT_SUCCESS)
			{
			printf("moving_average() failed...exiting\n");
			exit(0);
			}
		find_zero_crossings(ptempfile,threshold,deltat,column_number,pzc_stats);
		duty_cycle_max_percent = 100.0*pzc_stats->max_ontime/pzc_stats->ave_period;
		duty_cycle_min_percent = 100.0*pzc_stats->min_ontime/pzc_stats->ave_period;
	
		/*printf("duty_cycle_max_percent = %.3f, duty_cycle_min_percent = %.3f\n",duty_cycle_max_percent,duty_cycle_min_percent);*/
	
		if ((duty_cycle_max_percent > 99.0) || (duty_cycle_min_percent < 1.0))
			{
			num_moving_average_samples++;
			printf("Threshold crossings occur too quickly, increasing number of moving average ");
			printf("samples to %d.\n",num_moving_average_samples);
			glitch_flag = 1;
			}
		else
			{
			glitch_flag = 0;
			}
		/*printf("i = %d, glitch_flag was set to %d.\n",i,glitch_flag);*/
		i++;
		}
		while ((glitch_flag == 1) && (i < MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS) && ((double) num_moving_average_samples < (ceil(number_of_output_lines/2) - 1)));

	if (i == MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS)
		{
		printf("Exceeded the allowed number of iterations for the number of\n");
		printf("moving average samples (%d). ",MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS);
		printf("Enter a starting value greater than %d.\n",num_moving_average_samples - MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS);
		exit(0);
		}
	}
else
	{
	strcpy(ptempfile,pfin);
	find_zero_crossings(ptempfile,threshold,deltat,column_number,pzc_stats);
	}

if( pzc_stats->num_periods > 0)
	{
	if (pzc_stats->num_periods == 1)
		{
		printf("Read a total of %ld period in file \"%s\"\n",pzc_stats->num_periods,pfin);
		printf("using a threshold value of %s.\n",add_units(threshold,1,"V",value_string[0]));
		}
	else
		{
		printf("Read a total of %ld periods in file \"%s\"\n",pzc_stats->num_periods,pfin);
		printf("using a threshold value of %s.\n",add_units(threshold,1,"V",value_string[0]));
		}
	printf ("Average time period = %2.6e sec, frequency = %4.6f MHz.\n",\
	pzc_stats->ave_period, 1.0e-06/pzc_stats->ave_period);
	printf ("Minimum period = %2.6e sec (%4.6f MHz at %1.6e sec).\n",\
	pzc_stats->min_period, 1.0e-06/pzc_stats->min_period,pzc_stats->min_period_time);
	printf ("Maximum period = %2.6e sec (%4.6f MHz at %1.6e sec, delta = %.4f%%).\n",\
	pzc_stats->max_period, 1.0e-06/pzc_stats->max_period,pzc_stats->max_period_time,100.0*(1.0/pzc_stats->min_period - 1.0/pzc_stats->max_period)/(1.0/pzc_stats->ave_period));
	
	printf ("Average on time = %2.2e sec, Duty cycle = %2.2f %%.\n",\
	pzc_stats->ave_ontime, 100.0 * pzc_stats->ave_ontime/pzc_stats->ave_period);
	printf ("Minimum on time = %2.2e sec (Duty cycle = %2.2f %% at %1.6e sec).\n",\
	pzc_stats->min_ontime, 100.0 * pzc_stats->min_ontime/pzc_stats->ave_period,pzc_stats->min_ontime_time);
	printf ("Maximum on time = %2.2e sec (Duty cycle = %2.2f %% at %1.6e sec, delta = %.2f%%).\n",\
	pzc_stats->max_ontime, 100.0 * pzc_stats->max_ontime/pzc_stats->ave_period,pzc_stats->max_ontime_time,100.0*(pzc_stats->max_ontime - pzc_stats->min_ontime)/pzc_stats->ave_period);
	
	printf("The calculated average frequency is %4.6f MHz.\n",1.0e-06/pzc_stats->ave_period);
	
	if (use_ave_freq_flag != 1)
	   {
	   pzc_stats->ave_period = 1.0/(ave_freq_MHz*1.0e6);
	   }
	
	printf("Using an average frequency of %4.6f MHz to compute jitter.\n",1.0e-06/pzc_stats->ave_period);
	
	/*Now that we have average frequency, use this frequency to determine maximum distance
	of clock edges from desired frequency to give jitter*/
	
	if (datascan(ptempfile,column_number,pfout,writefile,pzc_stats->ave_period,deltat,threshold,\
	pmax_pp_neg_edge_error,pmax_pp_pos_edge_error) != EXIT_SUCCESS)
	   {
	   printf("Error in datascan(), exiting...\n");
	   exit(0);
	   }
		
	printf("Negative edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	max_pp_neg_edge_error, (max_pp_neg_edge_error)/pzc_stats->ave_period, deltat/pzc_stats->ave_period);
	
	printf("Positive edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	max_pp_pos_edge_error, (max_pp_pos_edge_error)/pzc_stats->ave_period, deltat/pzc_stats->ave_period);
	
	if ((correct_slope_flag == 1) && (pzc_stats->num_periods > 1))
		{
		if ((pdoubles_array = (double *) calloc(DATA_COLUMNS,sizeof(double))) == NULL)
			{
			printf("Error allocating memory for pdoubles_array in main()...exiting\n");
			exit(0);
			}
		
		if (((pxy = (xy_pair *) calloc(pzc_stats->num_periods + 1,sizeof(xy_pair))) == NULL) )
			{
			printf("Error allocating memory to pxy array...exiting...\n");
			exit(0);
			}
		
		if (((px1y1 = (xy_pair *) calloc(pzc_stats->num_periods + 1,sizeof(xy_pair))) == NULL) )
			{
			printf("Error allocating memory to px1y1 array...exiting...\n");
			exit(0);
			}
	
			fpw1 = fopen(pfout,"r");
			i = 0;
			while(!feof(fpw1))
				{
				fgets(pinput_string,LINELENGTH,fpw1);
				remove_carriage_return(pinput_string);
				if (!feof(fpw1))
					{
					if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,DATA_COLUMNS)) == EXIT_SUCCESS)
						{
						pxy[i].x = pdoubles_array[0];
						pxy[i].y = pdoubles_array[1];
						px1y1[i].x = pdoubles_array[0];
						px1y1[i].y = pdoubles_array[2];
						/* printf("i = %d wrote pxy->x and pxy->y as %1.12e,%1.12e.\n",i,pxy->x,pxy->y); */
						i++;
						}
					}
				}
			fclose(fpw1);
		
			if (find_slope_intercept_xy(pxy,i,&slope_neg_edge,&intercept_neg_edge) != EXIT_SUCCESS)
				{
				printf("Error in find_slope_intercept_xy() for negative edge TIE...exiting\n"); 
				exit(0);
				}
			else
				{
				printf("Removing effect of residual slope in negative edge TIE:\nslope_neg_edge = %1.6e, intercept_neg_edge = %1.6e\n",slope_neg_edge,intercept_neg_edge);
				pzc_stats->ave_period_corrected = 1.0/(0.0 - slope_neg_edge + 1.0/pzc_stats->ave_period);
				printf("New estimated frequency is %s.\n", add_units(1.0/pzc_stats->ave_period_corrected,6,"Hz",value_string[0]));
				}
		
			if (find_slope_intercept_xy(px1y1,i,&slope_pos_edge,&intercept_pos_edge) != EXIT_SUCCESS)
				{
				printf("Error in find_slope_intercept_xy() for positive edge TIE...exiting\n"); 
				exit(0);
				}
			else
				{
				printf("Removing effect of residual slope in positive edge TIE:\nslope_pos_edge = %1.6e, intercept_pos_edge = %1.6e\n",slope_pos_edge,intercept_pos_edge);
				}
		
			fpw1 = fopen(pfout,"r");
			
			/* Create corrected file name, remove 3 digit extension and */
			/* append "_corrected" and replace extension */
			
			if (append_filename_keep_extension(pfout,pfout_corrected,"_corrected",2*LINELENGTH + 1) != EXIT_SUCCESS)
				{
				printf("Error in created corrected filename from function append_filename_keep_extension()\n");
				exit(0);
				}
			else
				printf("Slope corrected filename is \"%s\".\n",pfout_corrected);
				
			fpw2 = fopen(pfout_corrected,"w");
			fprintf(fpw2,"Time(sec),Slope corrected Neg Edge Error(UI),Slope corrected Pos Edge Error(UI)\n");
		
			i = 0;
			while(!feof(fpw1))
				{
				fgets(pinput_string,LINELENGTH,fpw1);
				remove_carriage_return(pinput_string);
				if (!feof(fpw1))
					{
				if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,DATA_COLUMNS)) == EXIT_SUCCESS)
					{
						corrected_neg_edge_tie = pdoubles_array[1] - (pdoubles_array[0]*slope_neg_edge + intercept_neg_edge);
						if (corrected_neg_edge_tie > corrected_neg_edge_tie_max)
							corrected_neg_edge_tie_max = corrected_neg_edge_tie;
						if (corrected_neg_edge_tie < corrected_neg_edge_tie_min)
							corrected_neg_edge_tie_min = corrected_neg_edge_tie;
			
						corrected_pos_edge_tie = pdoubles_array[2] - (pdoubles_array[0]*slope_pos_edge + intercept_pos_edge);
						if (corrected_pos_edge_tie > corrected_pos_edge_tie_max)
							corrected_pos_edge_tie_max = corrected_pos_edge_tie;
						if (corrected_pos_edge_tie < corrected_pos_edge_tie_min)
							corrected_pos_edge_tie_min = corrected_pos_edge_tie;
						
						fprintf(fpw2,"%1.12e,%1.12e,%1.12e\n",pdoubles_array[0],corrected_neg_edge_tie,
						corrected_pos_edge_tie);
						i++;
						}
					}
				}
			fclose(fpw1);
			fclose(fpw2);
			free(pxy);
			free(px1y1);
			free(pdoubles_array);
			printf("Corrected negative edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	pzc_stats->ave_period*(corrected_neg_edge_tie_max - corrected_neg_edge_tie_min), corrected_neg_edge_tie_max - corrected_neg_edge_tie_min, deltat/pzc_stats->ave_period);
			printf("Corrected positive edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	pzc_stats->ave_period*(corrected_pos_edge_tie_max - corrected_pos_edge_tie_min), corrected_pos_edge_tie_max - corrected_pos_edge_tie_min, deltat/pzc_stats->ave_period);
	
			}
			
	if (num_moving_average_samples > 0)
		remove(ptempfile);
	free(pzc_stats);	
	
	/* Create command lines for plotting waveform in gnuplot and plot if gnuplot is available */
	
	if ((check_executable(pgnuplot,pgnuplot_path)) == 0)
		{
		printf("Did not locate executable for gnuplot.\n");
		printf("Disabling all plots as gnuplot was not found in path.\n");
		}
	else
		{
		
		/* Generate text file with gnuplot output command */
		
		if (correct_slope_flag == 1)
			find_base_filename(pfout_corrected,pbase_filename,LINELENGTH);
		else
			find_base_filename(pfout,pbase_filename,LINELENGTH);
		
		sprintf(ptitle_string,"{/:Bold Sample frequency = %s, number of moving average samples = %d}",add_units(1e9*fs_GHz,1,"Hz",value_string[0]),num_moving_average_samples);
		
		sprintf(pgnuplot_command_1,"gnuplot -e 'base_filename = \"%s\"; plot_title = \"%s\"; nom_freq_text = \"%s\";' /Users/sml/cproj/jitterhist/jitterhistv160/plotting_routines/gnuplot/plot_jitter.gnu \n",pbase_filename,ptitle_string,add_units(1.0/pzc_stats->ave_period_corrected,6,"Hz",value_string[0]));

		system(pgnuplot_command_1);
		
		if ((check_executable(pgimp,pgimp_path)) != 0)
			{
			sprintf(pgnuplot_command_2,"gimp %s.png&\n",pbase_filename);
			system(pgnuplot_command_2);
			}		
		}
	}
else
	{
	printf("There were no periods found in file \"%s\"\nusing a threshold value of %s ",pfin,add_units(threshold,1,"V",value_string[0]));
	printf("and a moving average sample size of %d. ",num_moving_average_samples);
	printf("No analysis was performed.\n");
	}

}
