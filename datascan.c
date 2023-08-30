#include "jitterhist.h"

int datascan(char fin[],char fout[],int writefile,double ave_period,\
double deltat, double vthreshold,\
double *pmax_pp_neg_edge_error,double *pmax_pp_pos_edge_error)
{

int j = 0, tokens, number_of_data_columns = 1;
long int num_periods = 0;

char *pinput_string, input_string[LINELENGTH+1];

double time = 0.0;
double *pvalue0, value0;
double tneg = 0.0, tpos = 0.0;
double tneg_error_max = BIG_NEG_NUM, tneg_error_min = BIG_POS_NUM, tpos_error_max = BIG_NEG_NUM;
double tpos_error_min = BIG_POS_NUM, tneg_error = 0.0, tpos_error = 0.0;
double max_pp_neg_edge_error = 0.0, max_pp_pos_edge_error = 0.0;
double last_value =0.0, slope = 0.0, intercept = 0.0, t0 = 0.0;
double *pdoubles_array,doubles_array[DATA_COLUMNS];

FILE *fpw1,*fpw2;

pinput_string = &input_string[0];

/*Define parameters -  assumes starts at 0*/

time = 0.0;
pvalue0 = &value0;

/*Open input and output file - only open output file if writefile is 1*/

if ((pdoubles_array = (double *) calloc(1,sizeof(double))) == NULL)
	{
	printf("Error allocating memory for pdoubles_array in main()...exiting\n");
	exit(0);
	}

fpw1 = fopen(fin,"r");

if (writefile == 1)
   {
   fpw2 = fopen(fout,"w");
   fprintf(fpw2,"Time(sec),Neg Edge Error(UI),Pos Edge Error(UI)\n");
   }
fscanf(fpw1,"%lf",pvalue0);

while (value0 > vthreshold)
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
/*Now we've found a negative value - look for first zero crossing*/

while (value0 <= vthreshold)
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

/*Save this first zero crossing with positive slope  - reset time to zero and now look for next positive edge*/

/*Linearly interpolate to find better estimate of zero crossing*/

slope = (value0 - last_value)/deltat;
intercept = (last_value*time - value0*(time - deltat))/deltat;

t0 = (vthreshold - intercept)/slope;

/*Zero crossings with positive slope should occur when  freq * time = N where N = 0,2,4,...*/

while (!feof(fpw1))
   {
   while ((value0 > vthreshold) && !feof(fpw1))
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
   /*Save time of negative going zero crossing*/
   if (!feof(fpw1))
      {
      slope = (value0 - last_value)/deltat;
      intercept = (last_value*time - value0*(time - deltat))/deltat;
      tneg = (vthreshold - intercept)/slope;
      }
/*printf("tneg = %1.6e\n",tneg);*/
   
   while ((value0 <= vthreshold) && !feof(fpw1))
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
   /*Save time of positive going zero crossing*/
   if (!feof(fpw1))
      {
      slope = (value0 - last_value)/deltat;
      intercept = (last_value*time - value0*(time - deltat))/deltat;
      tpos = (vthreshold - intercept)/slope;
/*printf("tpos = %1.6e\n",tpos);*/

      num_periods += 1;
      tneg_error = tneg - t0 - (num_periods - 0.50) * ave_period;
      tpos_error = tpos - t0 - (num_periods * ave_period);
/*printf("At time = %1.6e:tneg_error = %1.6e tpos_error = %1.6e\n",time,tneg_error,tpos_error);*/
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
