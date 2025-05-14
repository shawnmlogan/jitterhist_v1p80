#include "jitterhist.h"

int find_psd_and_integrate(double *py,double *pedge_error_psd,double *pw,long int *pxnpoints,long int *pynpoints,long int *pwindow_number,double *poverlap,long int *pnum_segments,double *pfreq_pnoise,double *pedge_error_pnoise,int integration_method,int interpolate_pn_flag, double frequency_increment_Hz,int zero_mean_rms_flag,double *pedge_error_pnoise_rms_ui,jitterhist_inputs *pjh_inputs,long int *ppsd_error_edge)
{
int error_flag = 0;
long int i;
char *plog_string,log_string[LOGFILE_LINELENGTH + 1];
double edge_error_pnoise_rms_ui = 0.0, mean_y_ui = 0.0;

plog_string = &log_string[0];

sppowr(py,pedge_error_psd,pw,pxnpoints,pynpoints,pwindow_number,poverlap,pnum_segments,ppsd_error_edge);

if (*ppsd_error_edge == 0)
	{
	for (i = 0;i < *pynpoints;i++)
		{
		
		if (i == 0)
			{
			pedge_error_psd[i]=NAN; /* Assign each to NAN since means of positive and negative tie*/
			pedge_error_pnoise[i] = NAN;
			pfreq_pnoise[i] = 0.0;      /*  are zero and L(f) will be negative infinity */						
			}
		else
			{
			pfreq_pnoise[i] = ((double) i)*frequency_increment_Hz;
			pedge_error_pnoise[i] = 10.0*log10(pedge_error_psd[i]/(frequency_increment_Hz * (double) *pynpoints))
			 + LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR;
			}
		}

if(integrate_pn_ui(pfreq_pnoise,pedge_error_pnoise,*pynpoints+1,integration_method,pjh_inputs,pfreq_pnoise[0],pfreq_pnoise[*pynpoints - 1], interpolate_pn_flag,1,&edge_error_pnoise_rms_ui) != EXIT_SUCCESS)
	 	{
	 	snprintf(plog_string,LOGFILE_LINELENGTH,"integrate_pn_ui() failed in find_psd_and_integrate()...\n");
	 	print_string_to_log(plog_string,pjh_inputs);
	 	error_flag = 1;
	 	}
	if (error_flag != 1)
		{
		*pedge_error_pnoise_rms_ui = edge_error_pnoise_rms_ui;
		/*
			printf("In find_psd_and_integrate: Window number: %ld, TIE rms (last %ld points) = %1.12e, integrated edge phase noise = %1.12e, difference = %.3f%%.\n",
			*pwindow_number,*pxnpoints,rms(py,*pxnpoints,&mean_y_ui,zero_mean_rms_flag),
			edge_error_pnoise_rms_ui,
			100.0*(edge_error_pnoise_rms_ui - rms(py,*pxnpoints,&mean_y_ui,zero_mean_rms_flag))/rms(py,*pxnpoints,&mean_y_ui,zero_mean_rms_flag));
		*/
		}
	}

if (error_flag == 0)
	return EXIT_SUCCESS;
else
	return EXIT_FAILURE;
}