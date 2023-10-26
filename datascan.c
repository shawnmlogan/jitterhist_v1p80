#include "jitterhist.h"

int datascan(char *pfin,int column_number, char *pfout,int writefile,double ave_period,\
double deltat, double threshold,\
double *pmax_pp_neg_edge_error,double *pmax_pp_pos_edge_error)
{

int j = 0, tokens, found_value_flag = 0;
long int num_periods = 0;

char *pinput_string, input_string[LINELENGTH+1];

double time = 0.0;
double *pvalue0, value0;
double tneg = 0.0, tpos = 0.0;
double tneg_error_max = BIG_NEG_NUM, tneg_error_min = BIG_POS_NUM, tpos_error_max = BIG_NEG_NUM;
double tpos_error_min = BIG_POS_NUM, tneg_error = 0.0, tpos_error = 0.0;
double max_pp_neg_edge_error = 0.0, max_pp_pos_edge_error = 0.0;
double last_value =0.0, slope = 0.0, intercept = 0.0, t0 = 0.0;
double *pdoubles_array;

FILE *fpw1,*fpw2;

pinput_string = &input_string[0];

/*Define parameters -  assumes starts at 0*/

time = 0.0;
pvalue0 = &value0;

/*Open input and output file - only open output file if writefile is 1*/

if ((pdoubles_array = (double *) calloc(column_number,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in main()...exiting\n");
	exit(0);
	}

if (writefile == 1)
   {
   fpw2 = fopen(pfout,"w");
   fprintf(fpw2,"Time(sec),Neg Edge Error(UI),Pos Edge Error(UI)\n");
   }
found_value_flag = 0;
fpw1 = fopen(pfin,"r");

/*Look for time of first threshold crossing in data*/

while (found_value_flag == 0)
	{
	fgets(pinput_string,LINELENGTH,fpw1);
	remove_carriage_return(pinput_string);
	if ((parsestring_to_doubles_array(pinput_string,pdoubles_array,&tokens,column_number)) == EXIT_SUCCESS)
		{
		value0 = pdoubles_array[column_number-1];
		/* printf("Initial value0 = %1.6e.\n",value0); */
		found_value_flag = 1;
		}
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
		/* printf("while loop 1, time = %1.6e, value0 = %1.6e.\n",time,value0); */
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
		/* printf("while loop 2, time = %1.6e, value0 = %1.6e.\n",time,value0); */
		}
   }

/*Save this first zero crossing with positive slope  - reset time to zero and now look for next positive edge*/

/*Linearly interpolate to find better estimate of zero crossing*/

slope = (value0 - last_value)/deltat;
intercept = (last_value*time - value0*(time - deltat))/deltat;
/* printf("value0 = %1.6e, last_value = %1.6e, ",value0,last_value);
printf("time = %1.6e, time-delta = %1.6e.\n",time,time - deltat); */
t0 = (threshold - intercept)/slope;

/*Zero crossings with positive slope should occur when  freq * time = N where N = 0,2,4,...*/

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
      
   /*Save time of negative going zero crossing*/
   
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
      
   /*Save time of positive going zero crossing*/
   
   if (!feof(fpw1))
      {
      slope = (value0 - last_value)/deltat;
      intercept = (last_value*time - value0*(time - deltat))/deltat;
      tpos = (threshold - intercept)/slope;

      num_periods += 1;
      tneg_error = tneg - t0 - (num_periods - 0.50) * ave_period;
      tpos_error = tpos - t0 - (num_periods * ave_period);
      
      /*printf("time = %1.8e, num_periods = %ld, ave_period = %1.6e, TIE neg = %1.6e,TIE pos = %1.6e\n",time,num_periods,ave_period,tneg_error/ave_period,tpos_error/ave_period);
		printf("At time = %1.6e:tneg_error = %1.6e tpos_error = %1.6e\n",time,tneg_error,tpos_error); */
		
      if (writefile == 1)
         fprintf(fpw2, "%1.8e,%1.6e,%1.6e\n",time,tneg_error/ave_period,tpos_error/ave_period);
            
      if (tneg_error > tneg_error_max)
         tneg_error_max = tneg_error;
      if (tneg_error < tneg_error_min)
         tneg_error_min = tneg_error;

      if (tpos_error > tpos_error_max)
         tpos_error_max = tpos_error;
      if (tpos_error < tpos_error_min)
         tpos_error_min = tpos_error;
      }
   }
fclose(fpw1);
if (writefile == 1)
   fclose(fpw2);

/*Return values*/

max_pp_neg_edge_error = tneg_error_max - tneg_error_min;
max_pp_pos_edge_error = tpos_error_max - tpos_error_min;
*pmax_pp_neg_edge_error = max_pp_neg_edge_error;
*pmax_pp_pos_edge_error = max_pp_pos_edge_error;

return EXIT_SUCCESS;
}
