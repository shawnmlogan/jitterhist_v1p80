# include "jitterhist.h"

int find_zero_crossings(char *pfin, double threshold, double deltat, int column_number, zero_crossing_stats *pzc_stats,jitterhist_inputs *pjh_inputs)
{
int j, found_value_flag = 0;
int tokens = 0;

char *plog_string,log_string[LOGFILE_LINELENGTH + 1];
char *pinput_string, input_string[LINELENGTH+1];

double time = 0.0;
double *pvalue0, value0, *plast_value, last_value;
double t0 = 0.0,ontime = 0.0, period = 0.0, T = 0.0, sum_period = 0.0, sum_ontime = 0.0;
double tneg = 0.0, tpos = 0.0, ave_period = 0.0, ave_ontime = 0.0;
double ave_freq_MHz = 0.0;
double max_period = BIG_NEG_NUM, min_period = BIG_POS_NUM, max_ontime = BIG_NEG_NUM, min_ontime = BIG_POS_NUM;
double max_period_time = 0.0, min_period_time = 0.0, min_ontime_time = 0.0, max_ontime_time = 0.0;
double slope = 0.0, intercept = 0.0, orig_t0 = 0.0;
double interpolated_value0 = 0.0;
long int num_periods = 0;

double *pdoubles_array;

xy_pair *pxy, *px1y1;

FILE *fpw1;

/*Assign pointers*/

plog_string = &log_string[0];
pinput_string = &input_string[0];
pvalue0 = &value0;
plast_value = &last_value;

if ((pdoubles_array = (double *) calloc(column_number,sizeof(double))) == NULL)
	{
	snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory for pdoubles_array in main()...exiting\n");
	print_string_to_log(plog_string,pjh_inputs);
	exit(0);
	}

/*Open input file*/

fpw1 = fopen(pfin,"r");

/*Look for time of first threshold crossing in data*/

while ((found_value_flag == 0) && !feof(fpw1))
	{
	fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
		{
		value0 = pdoubles_array[column_number-1];
		found_value_flag = 1;
		}
	}

if (found_value_flag == 0)
	{
	snprintf(plog_string,LOGFILE_LINELENGTH,"Reached end of input data file and did not find an initial valid value.\n");
	print_string_to_log(plog_string,pjh_inputs);
	exit(0);
	}
	
time = 0.0;

while ((value0 > threshold) && !feof(fpw1))
   {
   fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
		{
		value0 = pdoubles_array[column_number-1];
		time += deltat;
		}
   }

/*Now we've found a negative value - look for first threshold crossing*/

while ((value0 <= threshold) && !feof(fpw1))
   {
   last_value = value0;
   fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
		{
		value0 = pdoubles_array[column_number-1];
		time += deltat;
		}
   }
   
if(feof(fpw1))
	{
	num_periods = 0;
	ave_period = 0.0;
	ave_ontime = 0.0;
	
	min_period = 0.0;
	min_period_time = 0.0;
	max_period = 0.0;
	max_period_time = 0.0;
	
	min_ontime = 0.0;
	min_ontime_time = 0.0;
	max_ontime = 0.0;
	max_ontime_time = 0.0;
	}
else
	{
	
	/*Save this first threshold crossing with positive slope as tzero_first*/
	
	/*Linearly interpolate to find better estimate of zero crossing*/
	
	slope = (value0 - last_value)/deltat;
	intercept = (last_value*time - value0*(time - deltat))/deltat;
	
	t0 = (threshold - intercept)/slope;
	interpolated_value0 = slope*t0 + intercept;
	
	snprintf(plog_string,LOGFILE_LINELENGTH,"First threshold crossing at time %1.8e sec, interpolated_value is %1.8e.\n",t0, interpolated_value0);
	print_string_to_log(plog_string,pjh_inputs);
	
	while (!feof(fpw1))
	   {
	   while ((value0 > threshold) && !feof(fpw1))
	      {
	      last_value = value0;
	      fgets(pinput_string,LINELENGTH,fpw1);
			remove_carriage_return(pinput_string);
			if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
				{
				value0 = pdoubles_array[column_number-1];
				time += deltat;
				}
	      }
	   /*Save time of negative going threshold crossing*/
	
	   if (!feof(fpw1))
	      {
	      slope = (value0 - last_value)/deltat;
	      intercept = (last_value*time - value0*(time - deltat))/deltat;
	      tneg = (threshold - intercept)/slope;
	      }

	   while ((value0 <= threshold) && !feof(fpw1))
	      {
	      last_value = value0;
	      fgets(pinput_string,LINELENGTH,fpw1);
			remove_carriage_return(pinput_string);
			if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
				{
				value0 = pdoubles_array[column_number-1];
				time += deltat;
				}
	      }
	   /*Save time of positive going threshold crossing*/
	
	   if (!feof(fpw1))
	      {
	      slope = (value0 - last_value)/deltat;
	      intercept = (last_value*time - value0*(time - deltat))/deltat;
	      tpos = (threshold - intercept)/slope;
	      num_periods += 1;
	      ontime = tneg - t0;
	      period = tpos - t0;
	      
	      if (ontime > max_ontime)
	       	{
	         max_ontime = ontime;
	         max_ontime_time = time;
	         }
	      if (ontime < min_ontime)
	      	{
	         min_ontime = ontime;
	         min_ontime_time = time;
	         }
	      if (period > max_period)
	      	{
	         max_period = period;
	         max_period_time = time;
	         }         
	      if (period < min_period)
	      	{
	         min_period = period;
	         min_period_time = time;
	         }         
	
	      sum_period += period;
	      sum_ontime += ontime;
	      /*Save new offset time as last positive going threshold crossing*/
	      t0 = tpos;
	      }
	   }
   }
free(pdoubles_array);
fclose(fpw1);

if( num_periods > 0)
	{
	ave_period = sum_period/num_periods;
	ave_ontime = sum_ontime/num_periods;

	pzc_stats->num_periods = num_periods;
	pzc_stats->ave_period = ave_period;
	pzc_stats->ave_ontime = ave_ontime;
	
	pzc_stats->min_period = min_period;
	pzc_stats->min_period_time = min_period_time;
	pzc_stats->max_period = max_period;
	pzc_stats->max_period_time = max_period_time;
	
	pzc_stats->min_ontime = min_ontime;
	pzc_stats->min_ontime_time = min_ontime_time;
	pzc_stats->max_ontime = max_ontime;
	pzc_stats->max_ontime_time = max_ontime_time;
	}
else
	{
	pzc_stats->num_periods = num_periods;
	pzc_stats->ave_period = ave_period;
	pzc_stats->ave_ontime = ave_ontime;
	
	pzc_stats->min_period = min_period;
	pzc_stats->min_period_time = min_period_time;
	pzc_stats->max_period = max_period;
	pzc_stats->max_period_time = max_period_time;
	
	pzc_stats->min_ontime = min_ontime;
	pzc_stats->min_ontime_time = min_ontime_time;
	pzc_stats->max_ontime = max_ontime;
	pzc_stats->max_ontime_time = max_ontime_time;
	}
return EXIT_SUCCESS;
}