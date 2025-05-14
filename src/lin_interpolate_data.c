
#include "jitterhist.h"

int lin_interpolate_data(double *px,double *py,long int number_of_data_points,double *px_out, double *py_out,long int number_of_output_data_points,jitterhist_inputs *pjitterhist_inputs)
{
int error_flag = 0;
long int i;
char *pstring,string[LINELENGTH + 1];
char *plog_string,log_string[LOGFILE_LINELENGTH + 1];
double slope, intercept;

xy_pair *pxy;

pstring = &string[0];
plog_string = &log_string[0];

if ((number_of_data_points > 0) && (number_of_output_data_points > 0))
	{
	if ((pxy = (xy_pair *) calloc(number_of_data_points,sizeof(xy_pair))) == NULL)
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Error allocating memory to pxy in lin_interpolate_data().\n");
		print_string_to_log(plog_string,pjitterhist_inputs);
		error_flag = 1;
		}
	else
		{
		for (i = 0; i < number_of_data_points; i++)
			{
			pxy[i].x = px[i];
			pxy[i].y = py[i];
			}
	   if (find_slope_intercept_xy(pxy,number_of_data_points,pjitterhist_inputs->ptimestamp,&slope,&intercept) != EXIT_SUCCESS)
	   	{
	   	snprintf(plog_string,LOGFILE_LINELENGTH,"Error occurred in find_slope_intercept_xy()...\n");
	   	print_string_to_log(plog_string,pjitterhist_inputs);
			error_flag = 1;
			}
		else
			{
			#ifdef DEBUG_INTERPOLATE_DATA
				snprintf(plog_string,LOGFILE_LINELENGTH,"Slope = %.16e, intercept = %1.6e\n",slope,intercept);
				print_string_to_log(plog_string,pjitterhist_inputs);
			#endif
			if (number_of_output_data_points == 1)
				*py_out = intercept + slope*(*px_out);
			else
				{
				for(i = 0; i < number_of_output_data_points; i++)
			   	py_out[i] = intercept + slope*(px_out[i]);
			   }
			}
		#ifdef DEBUG_INTERPOLATE_DATA
			snprintf(plog_string,LOGFILE_LINELENGTH,"x,Interpolated result\n");
			print_string_to_log(plog_string,pjitterhist_inputs);
			if (number_of_output_data_points == 1)
				{
				snprintf(plog_string,LOGFILE_LINELENGTH,"%1.12e,%1.12e\n",*px_out,*py_out);
				print_string_to_log(plog_string,pjitterhist_inputs);
				}
			else
				{
				for (i = 0; i < number_of_output_data_points; i++)
					{
					snprintf(plog_string,LOGFILE_LINELENGTH,"%1.12e,%1.12e\n",px_out[i],py_out[i]);
					print_string_to_log(plog_string,pjitterhist_inputs);
					}
				}
		#endif
		free(pxy);
		}
	}
else
	{
	snprintf(plog_string,LOGFILE_LINELENGTH,"Enter a non-zero number of input and output data points to function lin_interpolate_data().\n");
	print_string_to_log(plog_string,pjitterhist_inputs);
	error_flag = 1;
	}

if (error_flag != 0)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;
	
}