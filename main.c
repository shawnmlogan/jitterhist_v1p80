#include "jitterhist.h"

int main(int argc,char *argv[])
{
int i,j, NUMBER, writefile = 1, found_value_flag = 0, number_of_data_columns = 1;
int tokens = 0, use_ave_freq_flag = 0, correct_slope_flag = 0;;
int num_moving_average_samples = 0;
long int number_of_input_lines = 0, number_of_output_lines = 0;

char *ptempfile, tempfile[LINELENGTH + 1];
char fin[LINELENGTH+1],fout[LINELENGTH+1],fout_corrected[2*LINELENGTH + 1],*pfin,*pfout,*pfout_corrected;
char *pinput_string, input_string[LINELENGTH+1];
char value_string[NUMBER_OF_VALUE_STRINGS][LINELENGTH_OF_VALUE_STRING + 1];

double fs_GHz, ts, time = 0.0, deltat =0.0, tstart = 0.0, TSTART = 0.0;
double *pvalue0, value0, *plast_value, last_value;
double t0 = 0.0,ontime = 0.0, period = 0.0, T = 0.0, sum_period = 0.0, sum_ontime = 0.0;
double tneg = 0.0, tpos = 0.0, ave_period = 0.0, ave_ontime = 0.0;
double ave_freq_MHz = 0.0;
double max_period = BIG_NEG_NUM, min_period = BIG_POS_NUM, max_ontime = BIG_NEG_NUM, min_ontime = BIG_POS_NUM;
double max_period_time = 0.0, min_period_time = 0.0, min_ontime_time = 0.0, max_ontime_time = 0.0;
double max_pp_pos_edge_error = 0.0,*pmax_pp_pos_edge_error, max_pp_neg_edge_error = 0.0, *pmax_pp_neg_edge_error;
double slope = 0.0, intercept = 0.0, orig_t0 = 0.0;
double interpolated_value0 = 0.0;
long int num_periods = 0;
double threshold = 0.0;
double *pdoubles_array;
double slope_pos_edge, intercept_pos_edge, slope_neg_edge, intercept_neg_edge;
double corrected_neg_edge_tie_max = BIG_NEG_NUM, corrected_neg_edge_tie_min = BIG_POS_NUM, corrected_neg_edge_tie;
double corrected_pos_edge_tie_max = BIG_NEG_NUM, corrected_pos_edge_tie_min = BIG_POS_NUM, corrected_pos_edge_tie;
xy_pair *pxy, *px1y1;

FILE *fpw1,*fpw2;

/*Assign pointers*/

pfin = &fin[0];
pfout = &fout[0];
pfout_corrected = &fout_corrected[0];
pinput_string = &input_string[0];
pvalue0 = &value0;
plast_value = &last_value;
pmax_pp_pos_edge_error = &max_pp_pos_edge_error;
pmax_pp_neg_edge_error = &max_pp_neg_edge_error;
ptempfile = &tempfile[0];

strcpy(ptempfile,"./.tempfilej");

/*Explain program*/

program_info();

/*Command line check*/
 
if (argc < 8 || argc > 10)
   {
   printf("Usage: jitterhist <inputfile> <outputfile> <Sample frequency (GHz)> ");
   printf("<vthreshold> <num_moving_average_samples> <correct_slope?(y|n)> <Use ave period?(y|n)> <Freq_to_use (MHz) if \"n\">\n");
   exit(0);
   }
else
   {
	/* Check inputs and count number of valid input lines in input file column 1 */
	if( check_inputs(argv,argc,pfin,pfout,&fs_GHz,&threshold,
	&num_moving_average_samples,&use_ave_freq_flag,&ave_freq_MHz,&number_of_input_lines,&correct_slope_flag) != EXIT_SUCCESS)
		{
		exit(0);
		}
   }

/*Set deltat*/

deltat = (1/(fs_GHz*1e9));

if (num_moving_average_samples > 0)
	{
	if (moving_average(pfin,ptempfile,1,num_moving_average_samples,&number_of_output_lines) != EXIT_SUCCESS)
		{
		printf("moving_average() failed...\n");
		exit(0);
		}
	}
else
	strcpy(ptempfile,pfin);

/*Open input file*/

fpw1 = fopen(ptempfile,"r");

/*Look for time of first threshold crossing in data*/

if ((pdoubles_array = (double *) calloc(number_of_data_columns,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in main()...exiting\n");
	exit(0);
	}

while (found_value_flag == 0)
	{
	fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
		{
		for(j = 0; j < number_of_data_columns;j++)
			{
			value0 = pdoubles_array[j];
			found_value_flag = 1;
			}
		}
	}
time = 0.0;

while (value0 > threshold)
   {
   fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
		{
		for(j = 0; j < number_of_data_columns;j++)
			{
			value0 = pdoubles_array[j];
			}
		}
   time += deltat;
   }

/*Now we've found a negative value - look for first threshold crossing*/

while (value0 <= threshold)
   {
   last_value = value0;
   fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
		{
		for(j = 0; j < number_of_data_columns;j++)
			{
			value0 = pdoubles_array[j];
			}
		}
   time += deltat;
   }
/*Save this first threshold crossing with positive slope as tzero_first*/

/*Linearly interpolate to find better estimate of zero crossing*/

slope = (value0 - last_value)/deltat;
intercept = (last_value*time - value0*(time - deltat))/deltat;

t0 = (threshold - intercept)/slope;
interpolated_value0 = slope*t0 + intercept;

printf("First threshold crossing at time %1.8e sec, interpolated_value is %1.8e.\n",t0, interpolated_value0);
  
while (!feof(fpw1))
   {
   while ((value0 > threshold) && !feof(fpw1))
      {
      last_value = value0;
      fgets(pinput_string,LINELENGTH,fpw1);
		remove_carriage_return(pinput_string);
		if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
			{
			for(j = 0; j < number_of_data_columns;j++)
				{
				value0 = pdoubles_array[j];
				time += deltat;
				}
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
		if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,number_of_data_columns)) == EXIT_SUCCESS)
			{
			for(j = 0; j < number_of_data_columns;j++)
				{
				value0 = pdoubles_array[j];
				time += deltat;
				}
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
fclose(fpw1);

printf("Read a total of %ld periods.\n",num_periods);

if( num_periods > 0)
	{
	
	ave_period = sum_period/num_periods;
	ave_ontime = sum_ontime/num_periods;
	
	printf ("Average time period = %2.6e sec, frequency = %4.6f MHz.\n",\
	ave_period, 1.0e-06/ave_period);
	printf ("Minimum  period = %2.6e sec (%4.6f MHz at %1.6e sec).\n",\
	min_period, 1.0e-06/min_period,min_period_time);
	printf ("Maximum  period = %2.6e sec (%4.6f MHz at %1.6e sec, delta = %.4f%%).\n",\
	max_period, 1.0e-06/max_period,max_period_time,100.0*(1.0/min_period - 1.0/max_period)/(1.0/ave_period));
	
	printf ("Average on time = %2.2e sec, Duty cycle = %2.2f %%.\n",\
	ave_ontime, 100.0 * ave_ontime/ave_period);
	printf ("Minimum on time = %2.2e sec (Duty cycle = %2.2f %% at %1.6e sec).\n",\
	min_ontime, 100.0 * min_ontime/ave_period,min_ontime_time);
	printf ("Maximum on time = %2.2e sec (Duty cycle = %2.2f %% at %1.6e sec, delta = %.2f%%).\n",\
	max_ontime, 100.0 * max_ontime/ave_period,max_ontime_time,100.0*(max_ontime - min_ontime)/ave_period);
	
	printf("The calculated average frequency is %4.6f MHz.\n",1.0e-06/ave_period);
	
	if (use_ave_freq_flag != 1)
	   {
	   ave_period = 1.0/(ave_freq_MHz*1.0e6);
	   }
	
	printf("Using an average frequency of %4.6f MHz to compute jitter.\n",1.0e-06/ave_period);
	
	/*Now that we have average frequency, use this frequency to determine maximum distance
	of clock edges from desired frequency to give jitter*/
	
	if (datascan(ptempfile,fout,writefile,ave_period,deltat,threshold,\
	pmax_pp_neg_edge_error,pmax_pp_pos_edge_error) != EXIT_SUCCESS)
	   {
	   printf("Error in datascan(), exiting...\n");
	   exit(0);
	   }
		
	printf("Negative edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	max_pp_neg_edge_error, (max_pp_neg_edge_error)/ave_period, deltat/ave_period);
	
	printf("Positive edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	max_pp_pos_edge_error, (max_pp_pos_edge_error)/ave_period, deltat/ave_period);
	
	if (correct_slope_flag == 1)
		{
		free(pdoubles_array);
		
		if ((pdoubles_array = (double *) calloc(DATA_COLUMNS,sizeof(double))) == NULL)
			{
			printf("Error allocating memory for pdoubles_array in main()...exiting\n");
			exit(0);
			}
		
		if (((pxy = (xy_pair *) calloc(num_periods + 1,sizeof(xy_pair))) == NULL) )
			{
			printf("Error allocating memory to pxy array...exiting...\n");
			exit(0);
			}
		
		if (((px1y1 = (xy_pair *) calloc(num_periods + 1,sizeof(xy_pair))) == NULL) )
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
				if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,DATA_COLUMNS)) == EXIT_SUCCESS)
					{
					pxy[i].x = pdoubles_array[0];
					pxy[i].y = pdoubles_array[1];
					px1y1[i].x = pdoubles_array[0];
					px1y1[i].y = pdoubles_array[2];
					/*printf("i = %ld wrote pxy->x and pxy->y as %1.12e,%1.12e.\n",i,pxy->x,pxy->y);*/
					i++;
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
				printf("Removing effect of residual slope in negative egde TIE:\nslope_neg_edge = %1.6e, intercept_neg_edge = %1.6e\n",slope_neg_edge,intercept_neg_edge);
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
			fclose(fpw1);
			fclose(fpw2);
			free(pxy);
			free(px1y1);
			printf("Corrected negative edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	ave_period*(corrected_neg_edge_tie_max - corrected_neg_edge_tie_min), corrected_neg_edge_tie_max - corrected_neg_edge_tie_min, deltat/ave_period);
			printf("Corrected positive edge location variation = %2.2e sec (%2.4f UIpp +/- %2.4f UI).\n",\
	ave_period*(corrected_pos_edge_tie_max - corrected_pos_edge_tie_min), corrected_pos_edge_tie_max - corrected_pos_edge_tie_min, deltat/ave_period);
			}
			
	if (num_moving_average_samples > 0)
		remove(ptempfile);
	free(pdoubles_array);		
	}
else
	{
	printf("There were no periods found in file \"%s\"\nusing a threshold value of %s.\n",pfin,add_units(threshold,3,"V",value_string[0]));
	printf("No analysis was performed.\n");
	}

}
