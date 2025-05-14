#include "jitterhist.h"

/*  Non-trapezoidal integration functions added 12/3/2022 */
/*  Optimized 12/5/2022 */
/*  Corrected 1/8/2024 to allow for exact exponent of -1 (1/f slope exactly) */
/*  Without added exception formula is not valid as integral of 1/x is ln(x) and */
/*  does not conform with [(1/f)^(z + 1)]/(z + 1) since z = -1! */
/*  Corrected and reversed subtraction order of X values in expression for case where z = -1. */
/*  As a result of error, the sign was reversed for integration in prior version leading to */
/*  a negative integrated value */
/*  Changed formula to be consistent with Signal Integrity Journal article and corrected formula for */
/*  case z = 1 as it was missing fo multiplicand 1/10/2024 */

int integrate_pn_ui(double *pfreq_Hz,double *pn_dBc,long int number_of_data_points,int integration_type,jitterhist_inputs *pjitterhist_inputs,double min_integration_freq_Hz,double max_integration_freq_Hz,int interpolate_pn_flag, int number_of_interpolating_points, double *pintegral)
{

int error_flag = 0, interpolate_start_freq_flag = 0, interpolate_stop_freq_flag = 0;
long int i, start_index = -1, stop_index = -1, integration_index = 0, number_of_integration_points = 0;

char *pstring,string[LINELENGTH + 1];
char *plog_string,log_string[LOGFILE_LINELENGTH + 1];

double *px, *py, *py_lin, z = 0.0, gamma = 0.0, local_integral = 0.0;
double *pinterpolate_x,*pinterpolate_y;
double minimum_input_frequency_Hz = 0.0, maximum_input_frequency_Hz = 0.0;
double epsilon = FREQUENCY_DIFF_TO_SKIP_INTERPOLATION;

pstring = &string[0];
plog_string = &log_string[0];

if (2.0*(max_integration_freq_Hz - min_integration_freq_Hz)/(max_integration_freq_Hz + min_integration_freq_Hz) > 0.0 - EPSILON)
	{
	if (((px = (double *) calloc(number_of_data_points,sizeof(double))) == NULL) || ((py = (double *)  calloc(number_of_data_points,sizeof(double))) == NULL) || ((py_lin = (double *)  calloc(number_of_data_points,sizeof(double))) == NULL) || ((pinterpolate_x = (double *) calloc(number_of_interpolating_points,sizeof(double))) == NULL) || ((pinterpolate_y = (double *) calloc(number_of_interpolating_points,sizeof(double))) == NULL))
		{
		snprintf(plog_string,LOGFILE_LINELENGTH,"Memory allocation failed for px or py in integrate_pn_ui().\n");
		print_string_to_log(plog_string,pjitterhist_inputs);
		error_flag = 1;
		}
	else
		{
		minimum_input_frequency_Hz = min(pfreq_Hz,number_of_data_points);
		maximum_input_frequency_Hz = max(pfreq_Hz,number_of_data_points);
		integration_index = 0;
		for(i = 1; i < number_of_data_points;i++)
			{
			if(((pfreq_Hz[i - 1] - min_integration_freq_Hz)/(min_integration_freq_Hz) < (0.0 - epsilon)) && ((pfreq_Hz[i] - min_integration_freq_Hz)/(min_integration_freq_Hz) > epsilon))
				{
				start_index = i - 1;
				interpolate_start_freq_flag = 1; /* Will interpolate phase noise index of 0 using min_integration_freq_Hz */
				}
			if(((pfreq_Hz[i - 1] - min_integration_freq_Hz)/(min_integration_freq_Hz) < (0.0 - epsilon)) && fabs((pfreq_Hz[i] - min_integration_freq_Hz)/(min_integration_freq_Hz)) < epsilon)
				{
				start_index = i;
				interpolate_start_freq_flag = 0; /* No need to interpolate lower frequency */
				}		
			if(fabs(pfreq_Hz[i - 1] - min_integration_freq_Hz)/(min_integration_freq_Hz) < epsilon)
				{
				start_index = i - 1;
				interpolate_start_freq_flag = 0; /* No need to interpolate lower frequency */
				}		
			if(((pfreq_Hz[i - 1] - max_integration_freq_Hz)/(max_integration_freq_Hz) < (0.0 - epsilon)) && ((pfreq_Hz[i] - max_integration_freq_Hz)/(max_integration_freq_Hz) > epsilon))
				{
				stop_index = i;
				interpolate_stop_freq_flag = 1; /* Will interpolate final index of phase noise using max_integration_freq_Hz */
				}
			if(((pfreq_Hz[i - 1] - max_integration_freq_Hz)/(max_integration_freq_Hz) < (0.0 - epsilon)) && fabs((pfreq_Hz[i] - max_integration_freq_Hz)/(max_integration_freq_Hz)) < epsilon)
				{
				stop_index = i;
				interpolate_stop_freq_flag = 0; /* No need to interpolate phase noise at upper frequency */
				}
			if(fabs(pfreq_Hz[i - 1] - max_integration_freq_Hz)/(max_integration_freq_Hz) < epsilon)
				{
				stop_index = i - 1;
				interpolate_start_freq_flag = 0; /* No need to interpolate phase noise at upper frequency */
				}		
			}
		if (start_index == -1) /* Allow for case where lower integration frequency is less than supplied data frequencies*/
			start_index = 0;

		if (stop_index == -1) /* Allow for case where upper integration frequency is greater than supplied data frequencies*/
			stop_index = (number_of_data_points - 1);
			
		#ifdef DEBUG_INTEGRATE_PN_UI			
			snprintf(plog_string,LOGFILE_LINELENGTH,"start_index = %ld, stop_index = %ld\n",start_index,stop_index);
			print_string_to_log(plog_string,pjitterhist_inputs);
		#endif
		
		number_of_integration_points = stop_index - start_index + 1;
		
		#ifdef DEBUG_INTEGRATE_PN_UI
			snprintf(plog_string,LOGFILE_LINELENGTH,"number_of_data_points = %ld, number_of_integration_points = %ld\n",
			number_of_data_points,number_of_integration_points);
			print_string_to_log(plog_string,pjitterhist_inputs);
		#endif
		
		for (i = start_index; i < (stop_index + 1); i++)
			{
			px[i-start_index] = pfreq_Hz[i];
			py[i-start_index] = pn_dBc[i];
			}			
			
			if ((interpolate_start_freq_flag == 1) && (number_of_integration_points > 2) && (interpolate_pn_flag == 1))
				{
				#ifdef DEBUG_INTEGRATE_PN_UI
					snprintf(plog_string,LOGFILE_LINELENGTH,"Interpolating phase noise for start frequency of %1.6e...\n",min_integration_freq_Hz);
					print_string_to_log(plog_string,pjitterhist_inputs);
				#endif
				
				for (i = 0; i < number_of_interpolating_points; i++)
					{
					pinterpolate_x[i] = px[i];
					pinterpolate_y[i] = py[i];
					#ifdef DEBUG_INTEGRATE_PN_UI
						snprintf(plog_string,LOGFILE_LINELENGTH,"pinterpolate_x[%ld] = %1.6e, interpolate_y[%ld] = %1.6e\n",
						i,pinterpolate_x[i],i,pinterpolate_y[i]);
						print_string_to_log(plog_string,pjitterhist_inputs);
					#endif
					pinterpolate_x[i] = log10(pinterpolate_x[i]);
					}
				px[0] = log10(min_integration_freq_Hz);
				lin_interpolate_data(pinterpolate_x,pinterpolate_y,number_of_interpolating_points,&px[0],&py[0],1,pjitterhist_inputs);
				px[0] = pow(10.0,px[0]);
				#ifdef DEBUG_INTEGRATE_PN_UI
					for (i = 0; i < number_of_interpolating_points; i++)
						{
						if (i == 0)
							{
							snprintf(plog_string,LOGFILE_LINELENGTH,"---- Interpolation results ------\ni,px[i],py[i]\n");
							print_string_to_log(plog_string,pjitterhist_inputs);
							snprintf(plog_string,LOGFILE_LINELENGTH,"%ld,%1.12e,%1.12e\n",i,px[i],py[i]);
							print_string_to_log(plog_string,pjitterhist_inputs);
							}
						else
							{
							snprintf(plog_string,LOGFILE_LINELENGTH,"%ld,%1.12e,%1.12e\n",i,px[i],py[i]);
							print_string_to_log(plog_string,pjitterhist_inputs);
							}
						snprintf(plog_string,LOGFILE_LINELENGTH,"-----End of interpolation results -------\n");
						print_string_to_log(plog_string,pjitterhist_inputs);
						}
				#endif
				}
			if ((interpolate_stop_freq_flag == 1) && (number_of_integration_points > 2) && (interpolate_pn_flag == 1))
				{
				#ifdef DEBUG_INTEGRATE_PN_UI
					snprintf(plog_string,LOGFILE_LINELENGTH,"Interpolating phase noise for stop frequency of %1.6e...\n",max_integration_freq_Hz);
					print_string_to_log(plog_string,pjitterhist_inputs);
				#endif
				for (i = 0; i < number_of_interpolating_points; i++)
					{
					pinterpolate_x[i] = px[(number_of_integration_points - 1) - (number_of_interpolating_points - 1) + i];
					pinterpolate_y[i] = py[(number_of_integration_points - 1) - (number_of_interpolating_points - 1) + i];
					#ifdef DEBUG_INTEGRATE_PN_UI
						snprintf(plog_string,LOGFILE_LINELENGTH,"pinterpolate_x[%ld] = %1.3e, pinterpolate_y[%ld] = %1.3e\n",
						i,pinterpolate_x[i],i,pinterpolate_y[i]);
						print_string_to_log(plog_string,pjitterhist_inputs);
					#endif
					pinterpolate_x[i] = log10(pinterpolate_x[i]);
					}
				px[(number_of_integration_points - 1)] = log10(max_integration_freq_Hz);
				lin_interpolate_data(pinterpolate_x,pinterpolate_y,number_of_interpolating_points,&px[(number_of_integration_points - 1)],&py[(number_of_integration_points - 1)],1,pjitterhist_inputs);
				px[(number_of_integration_points - 1)] = pow(10.0,px[(number_of_integration_points - 1)]);
				#ifdef DEBUG_INTEGRATE_PN_UI
					for (i = 0; i < number_of_interpolating_points ; i++)
						{
						snprintf(plog_string,LOGFILE_LINELENGTH,"Interpolation result: px[%ld] = %1.6e, py[%ld] = %1.3e\n",
						((number_of_integration_points - 1) + i),
						px[(number_of_integration_points - 1) - (number_of_interpolating_points - 1) + i],
						((number_of_integration_points - 1) + i),
						py[(number_of_integration_points - 1) - (number_of_interpolating_points - 1) + i]);
						print_string_to_log(plog_string,pjitterhist_inputs);
						}
				#endif
				}
			if(number_of_integration_points > 0)
				{
				#ifdef DEBUG_INTEGRATE_PN_UI
					snprintf(plog_string,LOGFILE_LINELENGTH,"Integrating between %1.4e and %1.4e in integrate_pn_ui.c\n",
					px[0],px[number_of_integration_points - 1]);
					print_string_to_log(plog_string,pjitterhist_inputs);
				#endif
				for( i = 0; i < number_of_integration_points; i++)
					{
					if isnan(py[i])
						py_lin[i] = 0.0;
					else
						py_lin[i] = pow(10.0,py[i]/10);
					#ifdef DEBUG_INTEGRATE_PN_UI
						snprintf(plog_string,LOGFILE_LINELENGTH,"i = %ld, py[%ld] = %1.12e, py_lin[%ld] = %1.12e\n",
						i,i,py[i],i,py_lin[i]);
						print_string_to_log(plog_string,pjitterhist_inputs);
					#endif
					}
				switch (integration_type)
					{
					case POWER:
						for( i = 0; i < number_of_integration_points; i++)
							{
							if (i > 1)
								{
								gamma = px[i - 1]/px[i];
								z = 0.0 - log10(py_lin[i - 1]/py_lin[i])/log10(gamma);
								if (z == 1)
									{
									local_integral = local_integral + py_lin[i - 1]*px[i - 1]*(log(px[i] - log(px[i - 1])));
									}
								else
									local_integral = local_integral + py_lin[i - 1]*px[i - 1]*(pow(1.0/gamma,(1 - z)) - 1.0)/(1 - z);
								#ifdef DEBUG_INTEGRATE_PN_UI
									snprintf(plog_string,LOGFILE_LINELENGTH,
									"%ld,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e\n",
									i,px[i],px[i] - px[i - 1],py_lin[i - 1],local_integral,pow(2.0*local_integral,0.50)/(2.0*M_PI));
									print_string_to_log(plog_string,pjitterhist_inputs);
								#endif		
								}
							else
								{
								if (i == 0)
									{
									local_integral = 0.0;
									#ifdef DEBUG_INTEGRATE_PN_UI
										snprintf(plog_string,LOGFILE_LINELENGTH,
										"---- POWER Integration results ------\ni,px[i],delta_x[i],py_lin[i],local integral,rms\n");
										print_string_to_log(plog_string,pjitterhist_inputs);
									#endif
									}
								else
									{
									gamma = px[i]/px[i + 1];
									z = 0.0 - log10(py_lin[i]/py_lin[i + 1])/log10(gamma);
									if (z == 1)
											{
											local_integral = local_integral + py_lin[i]*px[i]*(log(px[i + 1] - log(px[i])));
											}
									else
										local_integral = local_integral + py_lin[i]*px[i]*(pow(1.0/gamma,(1 - z)) - 1.0)/(1 - z);
									#ifdef DEBUG_INTEGRATE_PN_UI
										snprintf(plog_string,LOGFILE_LINELENGTH,
										"%ld,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e\n",
										i,px[i],px[i] - px[i - 1],py_lin[i - 1],local_integral,pow(2.0*local_integral,0.50)/(2.0*M_PI));
										print_string_to_log(plog_string,pjitterhist_inputs);
									#endif
									}
								}					
							}
					#ifdef DEBUG_INTEGRATE_PN_UI
						snprintf(plog_string,LOGFILE_LINELENGTH,"-----End of POWER integration results -------\n");
						print_string_to_log(plog_string,pjitterhist_inputs);
					#endif
						break;
				case BOXCAR:
					for( i = 0; i < number_of_integration_points; i++)
						{
						if (i == 0)
							{
							local_integral = 0.0;
							#ifdef DEBUG_INTEGRATE_PN_UI
								snprintf(plog_string,LOGFILE_LINELENGTH,
								"---- BOXCAR Integration results ------\ni,px[i],delta_x[i],py_lin[i],local integral,rms\n");
								print_string_to_log(plog_string,pjitterhist_inputs);
							#endif							
							}
						else
							{
							local_integral = local_integral + py_lin[i]*(px[i] - px[i - 1]);
							#ifdef DEBUG_INTEGRATE_PN_UI
								snprintf(plog_string,LOGFILE_LINELENGTH,
								"%ld,%1.12e,%1.12e,%1.12e,%1.12e,%1.12e\n",
								i,px[i],px[i] - px[i - 1],py_lin[i - 1],local_integral,pow(2.0*local_integral,0.50)/(2.0*M_PI));
								print_string_to_log(plog_string,pjitterhist_inputs);
							#endif		
							}
						}
					#ifdef DEBUG_INTEGRATE_PN_UI
						snprintf(plog_string,LOGFILE_LINELENGTH,"-----End of BOXCAR integration results -------\n");
						print_string_to_log(plog_string,pjitterhist_inputs);
					#endif
					break;
				default:
					snprintf(plog_string,LOGFILE_LINELENGTH,"Did not recognize integration type in integrate_pn_ui.c...\n");
					print_string_to_log(plog_string,pjitterhist_inputs);
					error_flag = 1;
					break;
				}

				#ifdef DEBUG_INTEGRATE_PN_UI
					snprintf(plog_string,LOGFILE_LINELENGTH,
					"Completed integrating between %1.4e and %1.4e in integrate_pn_ui, integral is %1.6e (%1.6e UI)\n",			
					px[0],px[number_of_integration_points - 1],local_integral,
					pow(2.0*local_integral,0.50)/(2.0*M_PI));
					print_string_to_log(plog_string,pjitterhist_inputs);
				#endif
				*pintegral = pow(2.0*local_integral,0.50)/(2.0*M_PI);
				}
			else
				*pintegral = NAN;
   	free(px);
   	free(py);
   	free(py_lin);
   	free(pinterpolate_x);
   	free(pinterpolate_y);
   	}
   }
	else
		{
		if ((max_integration_freq_Hz - min_integration_freq_Hz)/(max_integration_freq_Hz) < EPSILON)
			*pintegral = 0.0;
		else
			{
			snprintf(plog_string,LOGFILE_LINELENGTH,"Maximum integration frequency must be greater than minimum integration frequency.\n");
			print_string_to_log(plog_string,pjitterhist_inputs);
			error_flag = 1;
			}
		}
		#ifdef DEBUG_INTEGRATE_PN_UI
			snprintf(plog_string,LOGFILE_LINELENGTH,"Exiting integrate_pn_ui.c with *pintegral set to %1.8e.\n",*pintegral);
			print_string_to_log(plog_string,pjitterhist_inputs);
		#endif

if (error_flag != 0)
	return EXIT_FAILURE;
else
	return EXIT_SUCCESS;

}
