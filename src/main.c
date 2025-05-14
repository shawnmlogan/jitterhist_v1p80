
#include "jitterhist.h"

int main(int argc,char *argv[])
{
int i, k, writefile = 1, glitch_flag = 0, error_flag = 0;
int number_moving_ave_iterations = 0;
int entered_num_moving_average_samples = 0;
int tokens = 0;
int verbose = 0;
int zero_mean_rms_flag = 1;
int interpolate_pn_flag = 0, number_of_interpolating_points = NUMBER_OF_INTERPOLATING_POINTS;
int min_sum_square_rel_tie_index;
int file_argc = 11;
int integration_method = INTEGRATION_METHOD;

long int j;
long int psd_error_neg_edge = 0, psd_error_pos_edge = 0;
long int segment_sublength = PSD_SEGMENT_SUBLENGTH, window_number = (long int) PSD_WINDOW_TYPE;
long int num_segments = 0;
long int min_rel_tie_window_number;
long int number_of_output_lines = 0;
long int number_of_psd_parse_lines = 0;
long int psd_length = 0, xnpoints = 0, ynpoints = 0;

char *ptempfile, tempfile[FILENAME_LINELENGTH + 1];
char *plog_string,log_string[LOGFILE_LINELENGTH + 1];
char *plog_string_temp,log_string_temp[LOGFILE_LINELENGTH + 1];
char *plog_filename, log_filename[FILENAME_LINELENGTH + 1];
char *pmoving_ave_filename, moving_ave_filename[FILENAME_LINELENGTH + 1];
char *pfout_corrected, fout_corrected[FILENAME_LINELENGTH + 1];
char *pnom_freq_text,nom_freq_text[LINELENGTH + 1];
char *pbase_filename, base_filename[FILENAME_LINELENGTH - 4 + 1];
char *ptie_filename, tie_filename[FILENAME_LINELENGTH + 1];
char *pfilename_suffix1 = "_corrected", *pfilename_suffix2, filename_suffix2[SUFFIX2_LINELENGTH + 1];
char *ppnoise_filename, pnoise_filename[FILENAME_LINELENGTH + 1];
char *psample_time_sec, sample_time_sec[LINELENGTH + 1];
char *pwindow_type, window_type[LINELENGTH + 1];
char *pinput_string, input_string[LINELENGTH+1];
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];
char *pgimp = "gimp",*pgimp_path, gimp_path[LINELENGTH + 1];
char **ppfile_argv; /* Dynamic memory array to emulate argv[] if input filename only is argument */
char *pgraphic_converter_12 = "graphicconverter 12", *ppreview = "preview";
char *pgnuplot_command_1, gnuplot_command_1[COMMAND_LINELENGTH + 1];
char *pgnuplot_command_2, gnuplot_command_2[COMMAND_LINELENGTH + 1];
char *psys_command_1, sys_command_1[COMMAND_LINELENGTH + 1];
char *ptitle_string, title_string[TITLE_LINELENGTH + 1];
char *ptimestamp,timestamp[LINELENGTH + 1];
char *pcsv = ".csv", *pblank = "", *pquestion_mark = "\?";
char *psample_filename = "jitterhist_input_params.txt";

double frequency_increment_Hz = 0.0, deltat = 0.0;
double max_pp_pos_edge_error = 0.0,*pmax_pp_pos_edge_error, max_pp_neg_edge_error = 0.0, *pmax_pp_neg_edge_error;
double duty_cycle_max_percent, duty_cycle_min_percent;
double slope = 0.0, intercept = 0.0;
double *pdoubles_array;
double slope_pos_edge, intercept_pos_edge, slope_neg_edge, intercept_neg_edge;
double corrected_neg_edge_tie_max = BIG_NEG_NUM, corrected_neg_edge_tie_min = BIG_POS_NUM, corrected_neg_edge_tie;
double corrected_pos_edge_tie_max = BIG_NEG_NUM, corrected_pos_edge_tie_min = BIG_POS_NUM, corrected_pos_edge_tie;
double peak_to_peak_neg_edge_TIE_ui, peak_to_peak_neg_edge_TIE_sec, peak_to_peak_pos_edge_TIE_ui, peak_to_peak_pos_edge_TIE_sec;
double peak_to_peak_corrected_neg_edge_TIE_ui, peak_to_peak_corrected_neg_edge_TIE_sec, peak_to_peak_corrected_pos_edge_TIE_ui, peak_to_peak_corrected_pos_edge_TIE_sec;
double peak_to_peak_TIE_ui, peak_to_peak_corrected_TIE_ui;
double neg_edge_pos_edge_freq_diff_ppm = 0.0;
double *px, *py, *pz, *pw; /* Dynamic memory arrays for power spectral density analysis */
double **ppneg_edge_error_psd_optimum, **pppos_edge_error_psd_optimum; /* Dynamic memory arrays for optimized PSD analysis */
double *pneg_edge_error_pnoise, *ppos_edge_error_pnoise, *pfreq_pnoise; /* Dynamic memory arrays for phase noise data (used in integration routine) */
double neg_edge_error_pnoise_rms_ui = 0.0, pos_edge_error_pnoise_rms_ui = 0.0;
double mean_y_ui = 0.0, mean_z_ui = 0.0, zero_mean_rms_y_ui = 0.0, zero_mean_rms_z_ui = 0.0;
double neg_edge_error_pnoise, pos_edge_error_pnoise;
double freq_max_neg_edge_error_pnoise_Hz, freq_max_pos_edge_error_pnoise_Hz;
double max_neg_edge_error_pnoise = BIG_NEG_NUM, max_pos_edge_error_pnoise = BIG_NEG_NUM, max_phase_noise_dBc_Hz = -20.0;
double *pneg_edge_error_psd, *ppos_edge_error_psd; /* Dynamic memory arrays for power spectral density analysis */
double overlap = PSD_OVERLAP, log10_psd_to_phase_noise_conversion_factor = LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR;
double *pneg_edge_error_rel_tie, *ppos_edge_error_rel_tie, *psum_sq_rel_tie, min_sum_square_rel_tie;
double *pneg_edge_error_tie_ui, *ppos_edge_error_tie_ui;
xy_pair *pxy, *px1y1;
zero_crossing_stats *pzc_stats,zc_stats;
jitterhist_inputs *pjh_inputs,jh_inputs = {.enable_plot_flag = 1,.window_number = (long int) PSD_WINDOW_TYPE, .file_entry_flag = 0};

FILE *fpw1,*fpw2,*fpw3;

/*Assign pointers*/

plog_string = &log_string[0];
plog_string_temp = &log_string_temp[0];
plog_filename = &log_filename[0];
pfout_corrected = &fout_corrected[0];
pnom_freq_text = &nom_freq_text[0];
pbase_filename = &base_filename[0];
ptie_filename = &tie_filename[0];
pfilename_suffix2 = &filename_suffix2[0];
ppnoise_filename = &pnoise_filename[0];
psample_time_sec = &sample_time_sec[0];
pwindow_type = &window_type[0];
pinput_string = &input_string[0];
ptimestamp = &timestamp[0];
pmax_pp_pos_edge_error = &max_pp_pos_edge_error;
pmax_pp_neg_edge_error = &max_pp_neg_edge_error;
ptempfile = &tempfile[0];
pmoving_ave_filename = &moving_ave_filename[0];
pgimp_path = &gimp_path[0];
pgnuplot_command_1 = &gnuplot_command_1[0];
pgnuplot_command_2 = &gnuplot_command_2[0];
psys_command_1 = &sys_command_1[0];
ptitle_string = &title_string[0];
pzc_stats = &zc_stats;
pjh_inputs = &jh_inputs;
pjh_inputs->ppsd_window_numbers_list = &pjh_inputs->psd_window_numbers_list[0];
pjh_inputs->pgnuplot_path = &pjh_inputs->gnuplot_path[0];
pjh_inputs->pinput_filename = &pjh_inputs->input_filename[0];
pjh_inputs->poutput_filename = &pjh_inputs->output_filename[0];
pjh_inputs->ptimestamp = &pjh_inputs->timestamp[0];
pjh_inputs->plog_filename = &pjh_inputs->log_filename[0];
clock_t tic,toc;

tic = clock();

/* Find timestamp to append to filename and prevent multiple instantiations of
jitterhist in same directory from writing to same file */
	
find_timestamp(ptimestamp,LINELENGTH);
strncpy(pjh_inputs->ptimestamp,ptimestamp,LINELENGTH);
sprintf(ptempfile,"./.tempfilej_%s",ptimestamp);

/*Command line check*/

if ((argc != 2) && (argc != 9) && (argc != 10) && (argc != 11))
   {
   printf("\njitterhist v%.2f %s\n\n",VERSION,VERSION_DATE);
   printf("Case 1 Usage: jitterhist <csv filename with time samples> <column_number> <outputfile> <Sample frequency (GHz)> ");
   printf("<vthreshold> <num_moving_average_samples> <correct_slope?(y|n)> <Use ave frequency?(y|n)> <Freq_to_use (MHz) if \"n\"> <window_function_number (optional input)>\n\n");
   printf("Case 2 Usage: jitterhist <filename with jitterhist inputs>\n\njitterhist ? creates sample input file\n\n");
   exit(0);
   }
else
   {
   if (argc == 2)
	   {
	   if (strcmp(argv[1],pquestion_mark) == 0)
	   	{
	   	create_sample_file(psample_filename);
	   	printf("Created sample file \"%s\".\nModify file with your input parameter values.\n",
	   	psample_filename);
	   	exit(0);
	   	}
	   else
		   {
		   pjh_inputs->file_entry_flag = 1;
		   if (filecheck(argv[1]) != EXIT_SUCCESS)
		      {
		      printf("Exiting...try again with valid filename\n");
		      exit(0);
		      }
		   if ((ppfile_argv = (char **) calloc(file_argc,sizeof(char *))) == NULL )
		   	exit(0);
		   else
		   	{
		   	for (k=0; k < file_argc; k++)
		   		ppfile_argv[k] = (char *) calloc(CSV_FILE_LINELENGTH ,sizeof(char *));
		   	}
		   if (pop_data(argv[1],ppfile_argv,pjh_inputs) != EXIT_SUCCESS)
		   	{
		   	printf("Reading input file failed...creating sample file \"%s\" to use with your parameter values...exiting\n",
		   	psample_filename);
		   	create_sample_file(psample_filename);
				exit(0);
				}
		   if(check_jitterhist_inputs(ppfile_argv,file_argc,plog_string_temp,pjh_inputs) != EXIT_SUCCESS)
		   	{
		   	printf("%s\n",plog_string_temp);
				exit(0);
				}
			for (k=0; k < file_argc; k++)
		   	free(ppfile_argv[k]);
		   }
	   }
	else
		{ 
		
		/* Check inputs and count number of valid input lines in input file column*/
		
		pjh_inputs->file_entry_flag = 0;
		if( check_jitterhist_inputs(argv,argc,plog_string_temp,pjh_inputs) != EXIT_SUCCESS)
			{
			printf("%s\n",plog_string_temp);
			exit(0);
			}
   	}
   }

/* Create log filename */

replace_string(jh_inputs.pinput_filename,plog_filename,pcsv,pblank,FILENAME_LINELENGTH);
snprintf(pjh_inputs->plog_filename,FILENAME_LINELENGTH,"%s_log_%s.log",plog_filename,ptimestamp);
strncpy(plog_filename,pjh_inputs->plog_filename,FILENAME_LINELENGTH);

snprintf(plog_string,LOGFILE_LINELENGTH,"\njitterhist v%.2f %s\n\n%s",VERSION,VERSION_DATE,plog_string_temp);
print_string_to_log(plog_string,pjh_inputs);

/*Set deltat*/

deltat = (1/(pjh_inputs->fs_GHz*1e9));

if (pjh_inputs->num_moving_average_samples > 0)
	{
	i = 0;
	entered_num_moving_average_samples = pjh_inputs->num_moving_average_samples;
	do
		{
		snprintf(pmoving_ave_filename,FILENAME_LINELENGTH,"./.moving_ave_set_to_%d_iteration_%d_%s.csv",
		pjh_inputs->num_moving_average_samples,i,ptimestamp);
		if (moving_average(pjh_inputs->pinput_filename,pmoving_ave_filename,pjh_inputs->column_number,pjh_inputs->num_moving_average_samples,&number_of_output_lines) != EXIT_SUCCESS)
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"moving_average() failed...exiting\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		find_zero_crossings(pmoving_ave_filename,pjh_inputs->threshold,deltat,pjh_inputs->column_number,pzc_stats,pjh_inputs);
		number_moving_ave_iterations++;
		duty_cycle_max_percent = 100.0*pzc_stats->max_ontime/pzc_stats->ave_period;
		duty_cycle_min_percent = 100.0*pzc_stats->min_ontime/pzc_stats->ave_period;
	
		/*printf("duty_cycle_max_percent = %.3f, duty_cycle_min_percent = %.3f\n",duty_cycle_max_percent,duty_cycle_min_percent);*/
	
	if ((duty_cycle_max_percent > (100.0 - DUTY_CYCLE_MIN_PERCENT)) || (duty_cycle_min_percent < DUTY_CYCLE_MIN_PERCENT))
		{
		pjh_inputs->num_moving_average_samples++;
		snprintf(plog_string,LOGFILE_LINELENGTH,"Threshold crossings occur too quickly, increasing number of moving average ");
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"samples to %d.\n",pjh_inputs->num_moving_average_samples);
		print_string_to_log(plog_string,pjh_inputs);
		glitch_flag = 1;
		}
	else
		{
		glitch_flag = 0;
		}
		/*printf("i = %d, glitch_flag was set to %d.\n",i,glitch_flag);*/
		i++;
		}
	while ((glitch_flag == 1) && (i < MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS) && ((double) pjh_inputs->num_moving_average_samples < (ceil(number_of_output_lines/10) - 1)));

if (i == MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS)
	{
	snprintf(plog_string,LOGFILE_LINELENGTH,"Exceeded the allowed number of iterations for the number of\n");
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"moving average samples (%d). ",
	MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS);
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Enter a starting value greater than %d.\n",
	pjh_inputs->num_moving_average_samples - MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS);
	print_string_to_log(plog_string,pjh_inputs);
	exit(0);
	}
	strncpy(ptempfile,pmoving_ave_filename,FILENAME_LINELENGTH);
	}
else
	{
	strncpy(ptempfile,pjh_inputs->pinput_filename,FILENAME_LINELENGTH);
	find_zero_crossings(ptempfile,pjh_inputs->threshold,deltat,pjh_inputs->column_number,pzc_stats,pjh_inputs);
	}

if( pzc_stats->num_periods > 0)
	{
	if (pzc_stats->num_periods == 1)
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Read a total of %ld period in file \"%s\"\n",pzc_stats->num_periods,pjh_inputs->pinput_filename);
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"using a threshold value of %s.\n",add_units_2(pjh_inputs->threshold,1,0,0,"V",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		}
	else
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Read a total of %ld periods in file \"%s\"\n",pzc_stats->num_periods,pjh_inputs->pinput_filename);
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"using a threshold value of %s.\n",add_units_2(pjh_inputs->threshold,1,0,0,"V",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		}
	snprintf(plog_string,LOGFILE_LINELENGTH,"Average time period = %2.6e sec, frequency = %s.\n",
	pzc_stats->ave_period, add_units_2(1.0/pzc_stats->ave_period,8,0,0,"Hz",value_string[0]));
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Minimum period = %2.6e sec (%s at %s).\n",
	pzc_stats->min_period, add_units_2(1.0/pzc_stats->min_period,8,0,0,"Hz",value_string[0]),
	add_units_2(pzc_stats->min_period_time,6,0,0,"s",value_string[1]));
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Maximum period = %2.6e sec (%s at %s, delta = %.4f%%).\n",
	pzc_stats->max_period,add_units_2(1.0/pzc_stats->max_period,8,0,0,"Hz",value_string[0]),
	add_units_2(pzc_stats->max_period_time,6,0,0,"s",value_string[1]),
	100.0*(1.0/pzc_stats->min_period - 1.0/pzc_stats->max_period)/(1.0/pzc_stats->ave_period));
	print_string_to_log(plog_string,pjh_inputs);
	
	snprintf(plog_string,LOGFILE_LINELENGTH,"Average on time = %2.4e sec, Duty cycle = %2.2f %%.\n",
	pzc_stats->ave_ontime, 100.0 * pzc_stats->ave_ontime/pzc_stats->ave_period);
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Minimum on time = %2.4e sec (Duty cycle = %2.2f %% at %1.6e sec).\n",
	pzc_stats->min_ontime, 100.0 * pzc_stats->min_ontime/pzc_stats->ave_period,pzc_stats->min_ontime_time);
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Maximum on time = %2.4e sec (Duty cycle = %2.2f %% at %1.6e sec, delta = %.2f%%).\n",
	pzc_stats->max_ontime,100.0 * pzc_stats->max_ontime/pzc_stats->ave_period,
	pzc_stats->max_ontime_time,100.0*(pzc_stats->max_ontime - pzc_stats->min_ontime)/pzc_stats->ave_period);
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"The calculated average frequency is %s.\n",
	add_units_2(1.0/pzc_stats->ave_period,8,0,0,"Hz",value_string[0]));
	print_string_to_log(plog_string,pjh_inputs);
	
	if (pjh_inputs->use_ave_freq_flag != 1)
	   {
	   pzc_stats->ave_period = 1.0/(pjh_inputs->ave_freq_MHz*1.0e6);
	   }
	
	snprintf(plog_string,LOGFILE_LINELENGTH,"Using an average frequency of %s to compute jitter.\n",
	add_units_2(1.0/pzc_stats->ave_period,8,0,0,"Hz",value_string[0]));
	print_string_to_log(plog_string,pjh_inputs);
	
	/*Now that we have average frequency, use this frequency to determine maximum distance
	of clock edges from desired frequency to give jitter*/
	
	if (datascan(ptempfile,pjh_inputs->column_number,pjh_inputs->poutput_filename,writefile,pzc_stats,deltat,pjh_inputs->threshold,\
	pmax_pp_neg_edge_error,pmax_pp_pos_edge_error) != EXIT_SUCCESS)
	   {
	   snprintf(plog_string,LOGFILE_LINELENGTH,"Error in datascan(), exiting...\n");
	   print_string_to_log(plog_string,pjh_inputs);
	   exit(0);
	   }

	peak_to_peak_neg_edge_TIE_sec = max_pp_neg_edge_error;
	peak_to_peak_neg_edge_TIE_ui = peak_to_peak_neg_edge_TIE_sec/pzc_stats->ave_period;
	peak_to_peak_pos_edge_TIE_sec = max_pp_pos_edge_error;
	peak_to_peak_pos_edge_TIE_ui = peak_to_peak_pos_edge_TIE_sec/pzc_stats->ave_period;

	snprintf(plog_string,LOGFILE_LINELENGTH,"Negative edge location variation = %s (%s +/- %s).\n",
	add_units_2(peak_to_peak_neg_edge_TIE_sec,3,0,0,"s",value_string[0]),
	add_units_2(peak_to_peak_neg_edge_TIE_ui,3,0,0,"UIpp",value_string[1]),
	add_units_2(deltat/pzc_stats->ave_period,2,0,0,"UI",value_string[2]));
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Positive edge location variation = %s (%s +/- %s).\n",
	add_units_2(peak_to_peak_pos_edge_TIE_sec,3,0,0,"s",value_string[0]),
	add_units_2(peak_to_peak_pos_edge_TIE_ui,3,0,0,"UIpp",value_string[1]),
	add_units_2(deltat/pzc_stats->ave_period,2,0,0,"UI",value_string[2]));
	print_string_to_log(plog_string,pjh_inputs);

	if ((pjh_inputs->correct_slope_flag == 1) && (pzc_stats->num_periods > 1))
		{
		if ((pdoubles_array = (double *) calloc(MAXIMUM_NUMBER_OF_DATA_COLUMNS,sizeof(double))) == NULL)
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory for pdoubles_array in main()...exiting\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		
		if (((pxy = (xy_pair *) calloc(pzc_stats->num_periods + 1,sizeof(xy_pair))) == NULL) )
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to pxy array...exiting...\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		
		if (((px1y1 = (xy_pair *) calloc(pzc_stats->num_periods + 1,sizeof(xy_pair))) == NULL) )
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to px1y1 array...exiting...\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
	
			fpw1 = fopen(pjh_inputs->poutput_filename,"r");
			i = 0;
			while(!feof(fpw1))
				{
				fgets(pinput_string,LINELENGTH,fpw1);
				remove_carriage_return(pinput_string);
				if (!feof(fpw1))
					{
					if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,MAXIMUM_NUMBER_OF_DATA_COLUMNS)) == EXIT_SUCCESS)
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
		
			if (find_slope_intercept_xy(pxy,i,pjh_inputs->ptimestamp,&slope_neg_edge,&intercept_neg_edge) != EXIT_SUCCESS)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Error in find_slope_intercept_xy() for negative edge TIE...exiting\n");
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			else
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Removing effect of residual slope in negative edge TIE:\nslope_neg_edge = %1.6e, intercept_neg_edge = %1.6e\n",
				slope_neg_edge,intercept_neg_edge);
				print_string_to_log(plog_string,pjh_inputs);
				pzc_stats->ave_period_neg_edge_corrected = 1.0/(0.0 - slope_neg_edge + 1.0/pzc_stats->ave_period);
				snprintf(plog_string,LOGFILE_LINELENGTH,"New negative edge based estimated frequency is %s.\n",
				add_units_2(1.0/pzc_stats->ave_period_neg_edge_corrected,8,0,0,"Hz",value_string[0]));
				print_string_to_log(plog_string,pjh_inputs);
				}
		
			if (find_slope_intercept_xy(px1y1,i,pjh_inputs->ptimestamp,&slope_pos_edge,&intercept_pos_edge) != EXIT_SUCCESS)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Error in find_slope_intercept_xy() for positive edge TIE...exiting\n"); 
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			else
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Removing effect of residual slope in positive edge TIE:\nslope_pos_edge = %1.6e, intercept_pos_edge = %1.6e\n",
				slope_pos_edge,intercept_pos_edge);
				print_string_to_log(plog_string,pjh_inputs);
				pzc_stats->ave_period_pos_edge_corrected = 1.0/(0.0 - slope_pos_edge + 1.0/pzc_stats->ave_period);
				snprintf(plog_string,LOGFILE_LINELENGTH,"New positive edge based estimated frequency is %s.\n",
				add_units_2(1.0/pzc_stats->ave_period_pos_edge_corrected,8,0,0,"Hz",value_string[0]));
				print_string_to_log(plog_string,pjh_inputs);
				
				neg_edge_pos_edge_freq_diff_ppm = 1e6*(2.0*(1.0/pzc_stats->ave_period_pos_edge_corrected - 1.0/pzc_stats->ave_period_neg_edge_corrected)/(1.0/pzc_stats->ave_period_pos_edge_corrected + 1.0/pzc_stats->ave_period_neg_edge_corrected));
				
				if(fabs(neg_edge_pos_edge_freq_diff_ppm) > MAX_NEG_EDGE_POS_EDGE_FREQ_DIFF_PPM)
					{
					snprintf(plog_string,LOGFILE_LINELENGTH,"\nCheck analysis! Corrected positive and negative frequencies differ by more than %d ppm (%.1f ppm)!.\n",
					MAX_NEG_EDGE_POS_EDGE_FREQ_DIFF_PPM,neg_edge_pos_edge_freq_diff_ppm);
					print_string_to_log(plog_string,pjh_inputs);
					snprintf(plog_string,LOGFILE_LINELENGTH,"\tThis often indicates the data sample does not include enough\n");
					print_string_to_log(plog_string,pjh_inputs);
					snprintf(plog_string,LOGFILE_LINELENGTH,"\tperiods of a significant modulation frequency to provide an accurate\n");
					print_string_to_log(plog_string,pjh_inputs);
					snprintf(plog_string,LOGFILE_LINELENGTH,"\testimate of its long term frequency and resulting TIE computation.\n\n");
					print_string_to_log(plog_string,pjh_inputs);
					pzc_stats->ave_period_corrected = 0.50*(pzc_stats->ave_period_pos_edge_corrected + pzc_stats->ave_period_pos_edge_corrected);
					print_string_to_log(plog_string,pjh_inputs);
					}
				else
					{
					snprintf(plog_string,LOGFILE_LINELENGTH,"Corrected positive and negative frequencies differ by %.1f ppm (considered acceptable).\n",
					neg_edge_pos_edge_freq_diff_ppm);
					print_string_to_log(plog_string,pjh_inputs);
					pzc_stats->ave_period_corrected = 0.50*(pzc_stats->ave_period_pos_edge_corrected + pzc_stats->ave_period_pos_edge_corrected);
					}
				}
			
			/* Create corrected file name, remove 3 digit extension and */
			/* append "_corrected" and replace extension */
			
			if (append_filename_keep_extension(pjh_inputs->poutput_filename,pfout_corrected,pfilename_suffix1,FILENAME_LINELENGTH) != EXIT_SUCCESS)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Error in created corrected filename from function append_filename_keep_extension()\n");
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			else
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Slope corrected filename is \"%s\".\n",pfout_corrected);
				print_string_to_log(plog_string,pjh_inputs);
				}
			
			fpw1 = fopen(pjh_inputs->poutput_filename,"r");				
			fpw2 = fopen(pfout_corrected,"w");
			fprintf(fpw2,"Time(sec),Slope corrected Neg Edge Error(UI),Slope corrected Pos Edge Error(UI)\n");
		
			i = 0;
			while(!feof(fpw1))
				{
				fgets(pinput_string,LINELENGTH,fpw1);
				remove_carriage_return(pinput_string);
				if (!feof(fpw1))
					{
				if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,MAXIMUM_NUMBER_OF_DATA_COLUMNS)) == EXIT_SUCCESS)
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
			
			peak_to_peak_corrected_neg_edge_TIE_ui = corrected_neg_edge_tie_max - corrected_neg_edge_tie_min;
			peak_to_peak_corrected_neg_edge_TIE_sec = pzc_stats->ave_period_corrected*peak_to_peak_corrected_neg_edge_TIE_ui;
			peak_to_peak_corrected_pos_edge_TIE_ui = corrected_pos_edge_tie_max - corrected_pos_edge_tie_min;
			peak_to_peak_corrected_pos_edge_TIE_sec = pzc_stats->ave_period_corrected*peak_to_peak_corrected_pos_edge_TIE_ui;

			snprintf(plog_string,LOGFILE_LINELENGTH,"Corrected negative edge location variation = %s (%s +/- %s).\n",
			add_units_2(peak_to_peak_corrected_neg_edge_TIE_sec,3,0,0,"s",value_string[0]),
			add_units_2(peak_to_peak_corrected_neg_edge_TIE_ui,3,0,0,"UIpp",value_string[1]),
			add_units_2(deltat/pzc_stats->ave_period_corrected,2,0,0,"UI",value_string[2]));
			print_string_to_log(plog_string,pjh_inputs);
			snprintf(plog_string,LOGFILE_LINELENGTH,"Corrected positive edge location variation = %s (%s +/- %s).\n",
			add_units_2(peak_to_peak_corrected_pos_edge_TIE_sec,3,0,0,"s",value_string[0]),
			add_units_2(peak_to_peak_corrected_pos_edge_TIE_ui,3,0,0,"UIpp",value_string[1]),
			add_units_2(deltat/pzc_stats->ave_period_corrected,2,0,0,"UI",value_string[2]));
			print_string_to_log(plog_string,pjh_inputs);

		}
			
	if (pjh_inputs->num_moving_average_samples > 0)
		{			
		if (SAVE_MOVING_AVERAGE_FILES != 1)
			{
			remove(ptempfile);
			for(i = 0; i < number_moving_ave_iterations - 1; i++)
				{
				snprintf(pmoving_ave_filename,FILENAME_LINELENGTH,"./.moving_ave_set_to_%d_iteration_%d_%s.csv",
				i + entered_num_moving_average_samples,i,ptimestamp);
				remove(pmoving_ave_filename);
				}
			}
		}
		
/* Determine if phase noise analysis is accurate by comparing rms phase variation */
/*  to MAX_rms_TIE_FOR_PHASE_NOISE_UI */

if (peak_to_peak_corrected_neg_edge_TIE_ui > peak_to_peak_corrected_pos_edge_TIE_ui)
	peak_to_peak_corrected_TIE_ui = peak_to_peak_corrected_neg_edge_TIE_ui;
else
	peak_to_peak_corrected_TIE_ui = peak_to_peak_corrected_pos_edge_TIE_ui;

if (peak_to_peak_neg_edge_TIE_ui > peak_to_peak_pos_edge_TIE_ui)
	peak_to_peak_TIE_ui = peak_to_peak_neg_edge_TIE_ui;
else
	peak_to_peak_TIE_ui = peak_to_peak_pos_edge_TIE_ui;


/* Read in data from uncorrected or corrected TIE file if pjh_inputs->correct_slope_flag == 1 */
/* the number of periods is greater than 1 into arrays px, py, and pz */

	psd_length = (long int) pow(2,floor(log10(pzc_stats->num_periods)/log10(2.0)));

	if ((pjh_inputs->correct_slope_flag == 1) && (pzc_stats->num_periods > 1))
		strncpy(ptie_filename,pfout_corrected,FILENAME_LINELENGTH);
	else
		strncpy(ptie_filename,pjh_inputs->poutput_filename,FILENAME_LINELENGTH);

	if (((px = calloc((psd_length + 1),sizeof(double))) == NULL) || ((py = calloc((psd_length + 1),sizeof(double))) == NULL) || ((pz = calloc((psd_length + 1),sizeof(double))) == NULL))
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to psd data array (x),\npsd data array (y) or psd array (w)...exiting...\n");
		print_string_to_log(plog_string,pjh_inputs);
		exit(0);
		}
	number_of_psd_parse_lines = parse_3_column_csv_file(ptie_filename,(pzc_stats->num_periods - psd_length),pzc_stats->num_periods,1,2,3,px,py,pz);
	#ifdef DEBUG_PARSE_3_COLUMN_CSV_FILE
		snprintf(plog_string,LOGFILE_LINELENGTH,"Read a total of %ld lines using parse_3_column_csv_file() from file \"%s\".\n",
		number_of_psd_parse_lines,tie_filename);
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"pzc_stats->num_periods = %ld, psd_length = %ld.\n",
		pzc_stats->num_periods,psd_length);
		print_string_to_log(plog_string,pjh_inputs);
	#endif

	/* Compute rms and mean values of TIE */
			
	zero_mean_rms_y_ui = rms(py,psd_length,&mean_y_ui,zero_mean_rms_flag);
	zero_mean_rms_z_ui = rms(pz,psd_length,&mean_z_ui,zero_mean_rms_flag);

	/* Subtracting mean as may be using only a segment of corrected tie results since PSD
	will use last psd_length data points of tie */
	
	for (i = 0; i< psd_length; i++)
		{
		py[i] = py[i] - mean_y_ui;
		pz[i] = pz[i] - mean_z_ui;
		}

	zero_mean_rms_y_ui = rms(py,psd_length,&mean_y_ui,zero_mean_rms_flag);
	zero_mean_rms_z_ui = rms(pz,psd_length,&mean_z_ui,zero_mean_rms_flag);

	if (((pneg_edge_error_psd = calloc((psd_length/2 + 1),sizeof(double))) == NULL) || ((ppos_edge_error_psd = calloc((psd_length/2 + 1),sizeof(double))) == NULL) || ((pw = calloc((2*(psd_length/2 + 1) + 2),sizeof(double))) == NULL) || ((pneg_edge_error_pnoise = calloc((psd_length/2 + 1),sizeof(double))) == NULL) || ((ppos_edge_error_pnoise = calloc((psd_length/2 + 1),sizeof(double))) == NULL) || ((pfreq_pnoise = calloc((psd_length/2 + 1),sizeof(double))) == NULL))
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to output psd data arrays (pneg_edge_error_psd, ppos_edge_error_psd )\nor work array (pw)...exiting...\n");
		print_string_to_log(plog_string,pjh_inputs);
		exit(0);
		}
		
	/* Find psd of negative and positive edge errors and place in py and pz arrays*/

	overlap = PSD_OVERLAP;
	segment_sublength = PSD_SEGMENT_SUBLENGTH;
	xnpoints = psd_length;
	ynpoints = psd_length/(2*segment_sublength);
	if ((pjh_inputs->correct_slope_flag == 1) && (pzc_stats->num_periods > 1))
		frequency_increment_Hz = (0.50/pzc_stats->ave_period_corrected)/((double) ynpoints);
	else
		frequency_increment_Hz = (0.50/pzc_stats->ave_period)/((double) ynpoints);
	
	if (pjh_inputs->optimize_window_flag == 1)
		{
		printf("Optimizing window function...please wait...\n");
		
		/*Create 2D arrays to contain the PSD analysis results for negative (y) and positive (z) TIE */
		
		if (((ppneg_edge_error_psd_optimum = (double **) calloc(pjh_inputs->number_of_psd_windows,sizeof(double *))) == NULL) || ((pppos_edge_error_psd_optimum = (double **) calloc(pjh_inputs->number_of_psd_windows,sizeof(double *))) == NULL))
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to 2-D arrays holding neg and pos optimizing PSD results inmain.c...exiting...\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		for (k = 0; k < pjh_inputs->number_of_psd_windows; k++)
			{
			ppneg_edge_error_psd_optimum[k] = (double *) calloc((psd_length/2 + 1),sizeof(double));
			pppos_edge_error_psd_optimum[k] = (double *) calloc((psd_length/2 + 1),sizeof(double));
			if ((ppneg_edge_error_psd_optimum[k] == NULL) || (pppos_edge_error_psd_optimum[k] == NULL))
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to 1-D arrays holding neg and pos optimizing PSD results in main.c...exiting...\n");
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			}
		
		if (((ppos_edge_error_rel_tie = (double *) calloc(pjh_inputs->number_of_psd_windows,sizeof(double))) == NULL) || ((pneg_edge_error_rel_tie = (double *) calloc(pjh_inputs->number_of_psd_windows,sizeof(double))) == NULL) || ((pneg_edge_error_tie_ui = (double *) calloc(pjh_inputs->number_of_psd_windows,sizeof(double))) == NULL) || ((ppos_edge_error_tie_ui = (double *) calloc(pjh_inputs->number_of_psd_windows,sizeof(double))) == NULL) || ((psum_sq_rel_tie = (double *) calloc(pjh_inputs->number_of_psd_windows,sizeof(double))) == NULL))
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to arrays holding window number, TIE, and TIE errors in main.c...exiting...\n");
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
			{
			window_number = pjh_inputs->psd_window_numbers_list[j];		
			if (find_window_type(window_number,pwindow_type) != EXIT_SUCCESS)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"Did not choose a valid window function!\n");
				print_string_to_log(plog_string,pjh_inputs);
				snprintf(plog_string,LOGFILE_LINELENGTH,"Please check value of PSD_WINDOW_TYPE. Read value as %ld.\n",
				window_number);
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			else
				printf("Analyzing phase noise using a %s window...\n",pwindow_type);

			if (find_psd_and_integrate(py,ppneg_edge_error_psd_optimum[j],pw,&xnpoints,&ynpoints,&window_number,&overlap,&num_segments,pfreq_pnoise,pneg_edge_error_pnoise,integration_method,interpolate_pn_flag,frequency_increment_Hz,zero_mean_rms_flag,&neg_edge_error_pnoise_rms_ui,pjh_inputs,&psd_error_neg_edge) != EXIT_SUCCESS)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"find_psd_and_integrate() failed for negative edges in main.c...exiting...\n");
				print_string_to_log(plog_string,pjh_inputs);
				exit(0);
				}
			else
				{
				pneg_edge_error_tie_ui[j] = neg_edge_error_pnoise_rms_ui;
				pneg_edge_error_rel_tie[j] = (neg_edge_error_pnoise_rms_ui - zero_mean_rms_y_ui)/zero_mean_rms_y_ui;
				#ifdef DEBUG_OPTIMIZE_PSD_WINDOW
					snprintf(plog_string,LOGFILE_LINELENGTH,"Window number: %ld (%s), TIE rms (last %ld points) = %1.12e,",
					window_number,pwindow_type,psd_length,zero_mean_rms_y_ui);
					print_string_to_log(plog_string,pjh_inputs);
					snprintf(plog_string,LOGFILE_LINELENGTH," integrated neg edge phase noise = %1.12e, difference = %.6f%%.\n",
					neg_edge_error_pnoise_rms_ui,100.0*pneg_edge_error_rel_tie[j]);
					print_string_to_log(plog_string,pjh_inputs);
				#endif
				}
			}
			for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
				{
				window_number = pjh_inputs->psd_window_numbers_list[j];
				pjh_inputs->psd_window_numbers_list[j] = pjh_inputs->psd_window_numbers_list[j];
				find_window_type(window_number,pwindow_type);
				if (find_psd_and_integrate(pz,pppos_edge_error_psd_optimum[j],pw,&xnpoints,&ynpoints,&window_number,&overlap,&num_segments,pfreq_pnoise,ppos_edge_error_pnoise,integration_method,interpolate_pn_flag,frequency_increment_Hz,zero_mean_rms_flag,&pos_edge_error_pnoise_rms_ui,pjh_inputs,&psd_error_pos_edge) != EXIT_SUCCESS)
					{
					snprintf(plog_string,LOGFILE_LINELENGTH,"find_psd_and_integrate() failed for positive edges in main.c...exiting...\n");
					print_string_to_log(plog_string,pjh_inputs);
					exit(0);
					}
				else
					{
					ppos_edge_error_tie_ui[j] = pos_edge_error_pnoise_rms_ui;
					ppos_edge_error_rel_tie[j] = (pos_edge_error_pnoise_rms_ui - zero_mean_rms_z_ui)/zero_mean_rms_z_ui;
					psum_sq_rel_tie[j] = pow(pneg_edge_error_rel_tie[j],2.0) + pow(ppos_edge_error_rel_tie[j],2.0);
					#ifdef DEBUG_OPTIMIZE_PSD_WINDOW
						snprintf(plog_string,LOGFILE_LINELENGTH,"Window number: %ld (%s), TIE rms (last %ld points) = %1.12e,",
						window_number,pwindow_type,psd_length,zero_mean_rms_z_ui);
						print_string_to_log(plog_string,pjh_inputs);
						snprintf(plog_string,LOGFILE_LINELENGTH," integrated pos edge phase noise = %1.12e, difference = %.6f%%.\n",
						pos_edge_error_pnoise_rms_ui,100.0*ppos_edge_error_rel_tie[j]);
						print_string_to_log(plog_string,pjh_inputs);
						snprintf(plog_string,LOGFILE_LINELENGTH,"Sum of squares of negative and positive relative errors = %1.12e.\n",
						psum_sq_rel_tie[j]);
						print_string_to_log(plog_string,pjh_inputs);
					#endif
					}
				}
		for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
			{
			if (j == 0)
				{
				min_sum_square_rel_tie = psum_sq_rel_tie[j];
				min_sum_square_rel_tie_index = j;
				}				
			else
				{
				if (psum_sq_rel_tie[j] < min_sum_square_rel_tie)
					{
					min_sum_square_rel_tie = psum_sq_rel_tie[j];
					min_sum_square_rel_tie_index = j;
					}
				}
			}
			#ifdef DEBUG_OPTIMIZE_PSD_WINDOW
				snprintf(plog_string,LOGFILE_LINELENGTH,"\nWindow number,Window name,neg edge rms TIE (last %ld points),neg edge integrated phase noise,difference (%%),pos edge rms TIE (last %ld points),pos edge integrated phase noise,difference (%%),sum of squares of neg and pos edge differences\n",psd_length,psd_length);
				print_string_to_log(plog_string,pjh_inputs);
				for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
					{
					find_window_type(pjh_inputs->psd_window_numbers_list[j],pwindow_type);
					snprintf(plog_string,LOGFILE_LINELENGTH,"%ld,%s,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e\n",
					pjh_inputs->psd_window_numbers_list[j],pwindow_type,zero_mean_rms_y_ui,pneg_edge_error_tie_ui[j],pneg_edge_error_rel_tie[j],
					zero_mean_rms_z_ui,ppos_edge_error_tie_ui[j],ppos_edge_error_rel_tie[j],psum_sq_rel_tie[j]);
					print_string_to_log(plog_string,pjh_inputs);
					}
				find_window_type(pjh_inputs->psd_window_numbers_list[min_sum_square_rel_tie_index],pwindow_type);
				snprintf(plog_string,LOGFILE_LINELENGTH,"%ld,%s,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e\n\n",
				pjh_inputs->psd_window_numbers_list[min_sum_square_rel_tie_index],pwindow_type,zero_mean_rms_y_ui,
				pneg_edge_error_tie_ui[min_sum_square_rel_tie_index],
				pneg_edge_error_rel_tie[min_sum_square_rel_tie_index],
				zero_mean_rms_z_ui,ppos_edge_error_tie_ui[min_sum_square_rel_tie_index],
				ppos_edge_error_rel_tie[min_sum_square_rel_tie_index],psum_sq_rel_tie[min_sum_square_rel_tie_index]);
				print_string_to_log(plog_string,pjh_inputs);
			#endif
		min_rel_tie_window_number = pjh_inputs->psd_window_numbers_list[min_sum_square_rel_tie_index];
		window_number = pjh_inputs->psd_window_numbers_list[min_sum_square_rel_tie_index];

	/* #ifdef DEBUG_OPTIMIZE_PSD_WINDOW */
		find_window_type(min_rel_tie_window_number,pwindow_type);
		snprintf(plog_string,LOGFILE_LINELENGTH,"%s window selected to minimize error between temporal rms TIE and integrated phase noise rms TIE.\n",pwindow_type);
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"\tNegative edge temporal rms TIE = %s\n",
		add_units_2(zero_mean_rms_y_ui,3,0,0,"UI",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"\tIntegrated negative edge rms TIE = %s ",
		add_units_2(pneg_edge_error_tie_ui[min_sum_square_rel_tie_index],3,0,0,"UI",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"(Difference = %s).\n",add_units_2(100.0*pneg_edge_error_rel_tie[min_sum_square_rel_tie_index],2,0,0,"%",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"\tPositive edge temporal rms TIE = %s\n",
		add_units_2(zero_mean_rms_z_ui,3,0,0,"UI",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"\tIntegrated positive edge rms TIE = %s ",
		add_units_2(ppos_edge_error_tie_ui[min_sum_square_rel_tie_index],3,0,0,"UI",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"(Difference = %s).\n",add_units_2(100.0*ppos_edge_error_rel_tie[min_sum_square_rel_tie_index],2,0,0,"%",value_string[0]));
		print_string_to_log(plog_string,pjh_inputs);
	/* #endif */
	
	#ifdef DEBUG_OPTIMIZE_PSD_WINDOW
		
		fpw1 = fopen("ppneg_edge_error_psd_optimum.csv","w");
		fpw2 = fopen("pppos_edge_error_psd_optimum.csv","w");
		
		for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
		{
		if (j != (pjh_inputs->number_of_psd_windows - 1))
			{
			fprintf(fpw1,"Window %ld,",pjh_inputs->psd_window_numbers_list[j]);
			fprintf(fpw2,"Window %ld,",pjh_inputs->psd_window_numbers_list[j]);
			}
		else
			{
			fprintf(fpw1,"Window %ld\n",pjh_inputs->psd_window_numbers_list[j]);
			fprintf(fpw2,"Window %ld\n",pjh_inputs->psd_window_numbers_list[j]);
			}		
		}
	
		for (i = 0; i < psd_length/2; i++)
			{
			for (j = 0; j < pjh_inputs->number_of_psd_windows; j++)
				{
				if (j != (pjh_inputs->number_of_psd_windows - 1))
					{
					fprintf(fpw1,"%1.12e,",
					10.0*log10(ppneg_edge_error_psd_optimum[j][i]/(frequency_increment_Hz * (double) ynpoints))+ LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR);
					fprintf(fpw2,"%1.12e,",
					10.0*log10(pppos_edge_error_psd_optimum[j][i]/(frequency_increment_Hz * (double) ynpoints))+ LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR);
					}
				else
					{
					fprintf(fpw1,"%1.12e\n",
					10.0*log10(ppneg_edge_error_psd_optimum[j][i]/(frequency_increment_Hz * (double) ynpoints))+ LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR);
					fprintf(fpw2,"%1.12e\n",
					10.0*log10(pppos_edge_error_psd_optimum[j][i]/(frequency_increment_Hz * (double) ynpoints))+ LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR);
					}
				}
			}
		fclose(fpw1);
		fclose(fpw2);
				
	#endif
	}
else
	{
	window_number = pjh_inputs->window_number;
	if (find_window_type(window_number,pwindow_type) != EXIT_SUCCESS)
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Did not choose a valid window function!\n");
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"Please check value of PSD_WINDOW_TYPE. Read value as %ld.\n",
		window_number);
		print_string_to_log(plog_string,pjh_inputs);
		exit(0);
		}
	}

if (pjh_inputs->optimize_window_flag == 1)
	{
	for (i = 0;i < ynpoints;i++)
		{
		pneg_edge_error_psd[i] = ppneg_edge_error_psd_optimum[min_sum_square_rel_tie_index][i];
		ppos_edge_error_psd[i] = pppos_edge_error_psd_optimum[min_sum_square_rel_tie_index][i];
		}
	psd_error_neg_edge = 0;
	psd_error_pos_edge = 0;
	}
else
	{
		
	/* Compute psd using specified window type for positive and negative edges */
	
	sppowr(py,pneg_edge_error_psd,pw,&xnpoints,&ynpoints,&window_number,&overlap,&num_segments,&psd_error_neg_edge);
	if (psd_error_neg_edge == 0)
		{
		for (i = 0;i < ynpoints;i++)
			{
			if (i == 0)
				{
				pneg_edge_error_psd[i]=NAN; /* Assign each to NAN since means of positive and negative tie*/
				pneg_edge_error_pnoise[i] = NAN;
				pfreq_pnoise[i] = 0.0;      /*  are zero and L(f) will be negative infinity */						
				}
			else
				{
				pfreq_pnoise[i] = ((double) i)*frequency_increment_Hz;
				pneg_edge_error_pnoise[i] = 10.0*log10(pneg_edge_error_psd[i]/(frequency_increment_Hz * (double) ynpoints))
				 + log10_psd_to_phase_noise_conversion_factor;
				}
			}
		}
					
	sppowr(pz,ppos_edge_error_psd,pw,&xnpoints,&ynpoints,&window_number,&overlap,&num_segments,&psd_error_pos_edge);
	if (psd_error_pos_edge == 0)
		{
	for (i = 0;i < ynpoints;i++)
			{
			if (i == 0)
				{
				ppos_edge_error_psd[i]=NAN; /* Assign each to NAN since means of positive and negative tie*/
				ppos_edge_error_pnoise[i] = NAN;
				pfreq_pnoise[i] = 0.0;      /*  are zero and L(f) will be negative infinity */						
				}
			else
				{
				pfreq_pnoise[i] = ((double) i)*frequency_increment_Hz;
				ppos_edge_error_pnoise[i] = 10.0*log10(ppos_edge_error_psd[i]/(frequency_increment_Hz * (double) ynpoints))
				 + log10_psd_to_phase_noise_conversion_factor;
				}
			}
		}
	}
								
if((psd_error_neg_edge == 0) && (psd_error_pos_edge == 0))
	{
	if (num_segments > 1)
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Phase noise analyses complete and used ");
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"%ld segments of size %ld and each contains %ld output data points.\n",
		num_segments,2*ynpoints,ynpoints);
		print_string_to_log(plog_string,pjh_inputs);
		}
	else
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Phase noise analyses complete and used ");
		print_string_to_log(plog_string,pjh_inputs);
		snprintf(plog_string,LOGFILE_LINELENGTH,"%ld segment of size %ld and each contains %ld output data points.\n",
		num_segments,2*ynpoints,ynpoints);
		print_string_to_log(plog_string,pjh_inputs);
		}
	if (pjh_inputs->optimize_window_flag == 1)
		snprintf(pfilename_suffix2,SUFFIX2_LINELENGTH,
		"_seg_sublength_%ld_num_segments_%ld_ovrlap_percent_%.0f_window_%ld_pnoise",
		segment_sublength,num_segments,100.0*overlap,min_rel_tie_window_number);
	else
		snprintf(pfilename_suffix2,SUFFIX2_LINELENGTH,
		"_seg_sublength_%ld_num_segments_%ld_ovrlap_percent_%.0f_window_%ld_pnoise",
		segment_sublength,num_segments,100.0*overlap,window_number);

	if (append_filename_keep_extension(ptie_filename,ppnoise_filename,pfilename_suffix2,FILENAME_LINELENGTH) != EXIT_SUCCESS)
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Error in created corrected pnoise filename from function append_filename_keep_extension()\n");
		print_string_to_log(plog_string,pjh_inputs);
		exit(0);
		}
	else
		{
		if ((fpw3 = fopen(ppnoise_filename,"w")) == NULL)
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Error opening phase noise output file \"%s\" ",ppnoise_filename);
			print_string_to_log(plog_string,pjh_inputs);
			snprintf(plog_string,LOGFILE_LINELENGTH,"with length %ld characters...exiting...\n",strlen(ppnoise_filename));
			print_string_to_log(plog_string,pjh_inputs);
			exit(0);
			}
		if (pjh_inputs->correct_slope_flag == 1)
			fprintf(fpw3,"Frequency (Hz),Corrected Negative Edge Phase Noise (dBc/Hz),Corrected Positive Edge Phase Noise (dBc/Hz)\n");
		else
			fprintf(fpw3,"Frequency (Hz),Negative Edge Phase Noise (dBc/Hz),Positive Edge Phase Noise (dBc/Hz)\n");
							
		for (i = 0;i < ynpoints;i++)
			{
			if (i == 0)
				{
				pneg_edge_error_pnoise[i]=NAN; /* Assign each to NAN since means of positive and negative tie*/
				ppos_edge_error_pnoise[i]=NAN; /*  are zero and L(f) will be negative infinity */
				fprintf(fpw3,"%1.12e,%1.12e,%1.12e\n",
				((double) i)*frequency_increment_Hz,
				pneg_edge_error_pnoise[i],ppos_edge_error_pnoise[i]);
				}
			else
				{
				fprintf(fpw3,"%1.12e,%1.12e,%1.12e\n",
				((double) i)*frequency_increment_Hz,
				pneg_edge_error_pnoise[i],ppos_edge_error_pnoise[i]);
				if (pneg_edge_error_pnoise[i] > max_neg_edge_error_pnoise)
					{
					max_neg_edge_error_pnoise = pneg_edge_error_pnoise[i];
					freq_max_neg_edge_error_pnoise_Hz = ((double) i)*frequency_increment_Hz;
					}
				if (ppos_edge_error_pnoise[i] > max_pos_edge_error_pnoise)
					{
					max_pos_edge_error_pnoise = ppos_edge_error_pnoise[i];
					freq_max_pos_edge_error_pnoise_Hz = ((double) i)*frequency_increment_Hz;
					}
				}
			}
		}
	fclose(fpw3);
	snprintf(plog_string,LOGFILE_LINELENGTH,"Completed writing %d data points to phase noise output file \"%s\".\n",i,ppnoise_filename);
	print_string_to_log(plog_string,pjh_inputs);
	
	if (max_neg_edge_error_pnoise > LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ)
		{
		snprintf(pinput_string,LINELENGTH,"Warning! Negative edge based phase noise of %.1f dBc/Hz at %s exceeds %.0f dBc/Hz.\n",
		max_neg_edge_error_pnoise,add_units_2(freq_max_neg_edge_error_pnoise_Hz,3,0,0,"Hz",value_string[0]),
		LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ);
		print_repeated_char('-',strlen(pinput_string));
		printf("%s",pinput_string);
		printf("The accuracy of the negative edge based phase noise analysis is compromised.\n");
		print_repeated_char('-',strlen(pinput_string));
		}
	if (max_pos_edge_error_pnoise > LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ)
		{
		snprintf(pinput_string,LINELENGTH,"Warning! Positive edge based phase noise of %.1f dBc/Hz at %s exceeds %.0f dBc/Hz.\n",
		max_pos_edge_error_pnoise,add_units_2(freq_max_pos_edge_error_pnoise_Hz,3,0,0,"Hz",value_string[0]),
		LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ);
		print_repeated_char('-',strlen(pinput_string));
		printf("%s",pinput_string);
		printf("The accuracy of the positive edge based phase noise analysis is compromised.\n");
		print_repeated_char('-',strlen(pinput_string));
		}
	
	if (max_neg_edge_error_pnoise > max_pos_edge_error_pnoise)
		max_phase_noise_dBc_Hz = max_neg_edge_error_pnoise;
	else
		max_phase_noise_dBc_Hz = max_pos_edge_error_pnoise;								
    }
	else
	   {
	   switch(psd_error_neg_edge)
	     {
	      case 1:
	         /*printf("window_num out of range, program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Did not recognize window type, program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      case 2:
	         /*printf("LX is too small for one segment...program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Length of input data is too small for analysis of 1 segment program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      case 3:
	         /*printf("LY is not a power of 2...program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Segment sublength must be a factor of 2 (i.e.,1,2,4,...) program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      }
	   switch(psd_error_pos_edge)
	     {
	      case 1:
	         /*printf("window_num out of range, program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Did not recognize window type, program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      case 2:
	         /*printf("LX is too small for one segment...program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Length of input data is too small for analysis of 1 segment program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      case 3:
	         /*printf("LY is not a power of 2...program exits...\n");*/
	         snprintf(plog_string,LOGFILE_LINELENGTH,"Segment sublength must be a factor of 2 (i.e.,1,2,4,...) program exits...\n");
	         print_string_to_log(plog_string,pjh_inputs);
	         exit(0);
	      }
	   }
		
	/* Create command lines for plotting waveform in gnuplot and plot if gnuplot is available */
	
	if (pjh_inputs->enable_plot_flag == 1)
		{
		
		if (pjh_inputs->correct_slope_flag == 1)
			{
			find_base_filename(pfout_corrected,pbase_filename);
			strncpy(pnom_freq_text,add_units_2(1.0/pzc_stats->ave_period_corrected,6,0,0,"Hz",
		value_string[0]),LINELENGTH);
			}
		else
			{
			find_base_filename(pjh_inputs->poutput_filename,pbase_filename);
			strncpy(pnom_freq_text,add_units_2(1.0/pzc_stats->ave_period,6,0,0,"Hz",
		value_string[0]),LINELENGTH);
			}
		snprintf(ptitle_string,TITLE_LINELENGTH,"{/:Bold Sample frequency = %s, number of moving average samples = %d}",add_units_2(1e9*(pjh_inputs->fs_GHz),4,0,0,"Hz",value_string[0]),pjh_inputs->num_moving_average_samples);			
		snprintf(pgnuplot_command_1,COMMAND_LINELENGTH,"gnuplot -e 'base_filename = \"%s\"; plot_title = \"%s\"; nom_freq_text = \"%s\"; plotting_routines_dir = \"%s\";' %s/plotting_routines/gnuplot/plot_jitter.gnu \n",
		pbase_filename,ptitle_string,pnom_freq_text,PLOTTING_ROUTINES_DIR,PLOTTING_ROUTINES_DIR);
		system(pgnuplot_command_1);
		#ifdef PRINT_GNUPLOT_COMMAND
			snprintf(plog_string,LOGFILE_LINELENGTH,"\n%s\n",pgnuplot_command_1);
			print_string_to_log(plog_string,pjh_inputs);
		#endif

		if (OPEN_PNG_FILES_FLAG == 1)
			{		
			if ((check_executable(pgimp,pgimp_path)) != 0)
				{
				sprintf(pgnuplot_command_2,"gimp %s.png&\n",pbase_filename);
				system(pgnuplot_command_2);
				}
			else
				{
				if ((check_osx_executable(pgraphic_converter_12,pinput_string)) != 0)
					{
					sprintf(pgnuplot_command_2,"open -a \"%s\" %s.png&\n",
					pgraphic_converter_12,pbase_filename);
					system(pgnuplot_command_2);
					}
				}
			}

		if (pjh_inputs->correct_slope_flag == 1)
			strncpy(psample_time_sec,add_units_2((double) psd_length * pzc_stats->ave_period_corrected,3,0,0,"s",value_string[0]),LINELENGTH);
		else
			strncpy(psample_time_sec,add_units_2((double) psd_length * pzc_stats->ave_period,3,0,0,"s",value_string[0]),LINELENGTH);

		find_base_filename(ppnoise_filename,pbase_filename);
		snprintf(pgnuplot_command_1,COMMAND_LINELENGTH,"gnuplot -e 'base_filename = \"%s\"; plot_title = \"%s\"; nom_freq_text = \"%s\"; fft_length = %ld; sample_time_text = \"%s\"; window_type = \"%s\"; plotting_routines_dir = \"%s\"; limit_max_phase_noise_dbc_Hz = %.1f;' %s/plotting_routines/gnuplot/plot_phasenoise.gnu \n",
		pbase_filename,ptitle_string,pnom_freq_text,psd_length,psample_time_sec,
		pwindow_type,PLOTTING_ROUTINES_DIR,LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ,PLOTTING_ROUTINES_DIR);

		system(pgnuplot_command_1);
		#ifdef PRINT_GNUPLOT_COMMAND
			snprintf(plog_string,LOGFILE_LINELENGTH,"\n%s\n",pgnuplot_command_1);
			print_string_to_log(plog_string,pjh_inputs);
		#endif
		
		if (OPEN_PNG_FILES_FLAG == 1)
			{		
			if ((check_executable(pgimp,pgimp_path)) != 0)
				{
				sprintf(pgnuplot_command_2,"gimp %s.png&\n",pbase_filename);
				system(pgnuplot_command_2);
				}
			else
				{
				if ((check_osx_executable(pgraphic_converter_12,pinput_string)) != 0)
					{
					sprintf(pgnuplot_command_2,"open -a \"%s\" %s.png&\n",
					pgraphic_converter_12,pbase_filename);
					system(pgnuplot_command_2);
					}
				}
			}
		}
	free(px);
	free(py);
	free(pz);
	free(pw);
	free(pneg_edge_error_psd);
	free(ppos_edge_error_psd);
	free(pneg_edge_error_pnoise);
	free(ppos_edge_error_pnoise);
	free(pfreq_pnoise);
	if (pjh_inputs->optimize_window_flag == 1)
		{
		free(pneg_edge_error_rel_tie);
		free(ppos_edge_error_rel_tie);
		free(pneg_edge_error_tie_ui);
		free(ppos_edge_error_tie_ui);
		free(psum_sq_rel_tie);
		for (k = 0; k < pjh_inputs->number_of_psd_windows; k++)
			{
			free(ppneg_edge_error_psd_optimum[k]);
			free(pppos_edge_error_psd_optimum[k]);
			}
		}
	}
else
	{
	snprintf(plog_string,LOGFILE_LINELENGTH,"There were no periods found in file \"%s\"\nusing a threshold value of %s ",
	pjh_inputs->pinput_filename,add_units_2(pjh_inputs->threshold,1,0,0,"V",value_string[0]));
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"and a moving average sample size of %d. ",pjh_inputs->num_moving_average_samples);
	print_string_to_log(plog_string,pjh_inputs);
	snprintf(plog_string,LOGFILE_LINELENGTH,"No analysis was performed.\n");
	print_string_to_log(plog_string,pjh_inputs);
	}

toc = clock();
snprintf(pinput_string,LINELENGTH,"Elapsed time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
snprintf(plog_string,LOGFILE_LINELENGTH,"%s",pinput_string);
print_string_to_log(plog_string,pjh_inputs);

}
