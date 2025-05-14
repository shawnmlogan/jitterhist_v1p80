#include "jitterhist.h"

int assign(char *pidentifier,char *pentry, char **ppfile_argv, jitterhist_inputs *pjh_inputs)
{
int foundentry = 0, i = 0;

if (strcmp(pidentifier,"filename_with_data_samples_(csv format)") == 0)
   {
   strcpy(ppfile_argv[1],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"column_number") == 0)
   {
   strcpy(ppfile_argv[2],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"output_csv_filename") == 0)
   {
   strcpy(ppfile_argv[3],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"sample_rate_in_ghz") == 0)
   {
    strcpy(ppfile_argv[4],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"signal_threshold_(v)") == 0)
   {
   strcpy(ppfile_argv[5],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"num_moving_average_samples") == 0)
   {
   strcpy(ppfile_argv[6],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"correct_slope?_(y/n)") == 0)
   {
   strcpy(ppfile_argv[7],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"use_ave_frequency?_(y/n)") == 0)
   {
   strcpy(ppfile_argv[8],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"average_frequency_to_use_if_no_(mhz)") == 0)
   {
   strcpy(ppfile_argv[9],pentry);
   foundentry = 1;
   }
if (strcmp(pidentifier,"optimize_window_function?_(y/n)") == 0)
   {
   foundentry = 1;
   if (pentry[0] == 'y')
   	pjh_inputs->optimize_window_flag = 1;
   else
   	{
   	if (pentry[0] == 'n')
   		pjh_inputs->optimize_window_flag = 0;
   	else
   		{
   		printf("Incorrect entry for optimize window function in input file..no optimization performed.\n");
   		pjh_inputs->optimize_window_flag = 0;
   		}
   	}
   }
if (strcmp(pidentifier,"list_of_window_numbers_for_optimization_if_yes") == 0)
   {
   foundentry = 1;
   if (pjh_inputs->optimize_window_flag == 1)
   	{
		if (parsecsv_to_array(pentry,pjh_inputs->ppsd_window_numbers_list,&(pjh_inputs->number_of_psd_windows),
	   MAXIMUM_NUMBER_OF_PSD_WINDOWS,(double) MINIMUM_PSD_WINDOW_NUMBER,(double) MAXIMUM_PSD_WINDOW_NUMBER) != EXIT_SUCCESS)
			printf("Incorrect set of windowing functions in input file. Check list of window numbers.\n");
   	}
   }
if (strcmp(pidentifier,"window_number_if_no") == 0)
   {
   foundentry = 1;
   pjh_inputs->window_number = atol(pentry);
   }
if (strcmp(pidentifier,"plot_outputs?_(y/n)") == 0)
   {
   foundentry = 1;
   if (pentry[0] == 'y')
   	pjh_inputs->enable_plot_flag = 1;
   else
   	{
   	if (pentry[0] == 'n')
   		pjh_inputs->enable_plot_flag = 0;
   	else
   		{
   		printf("Incorrect entry for enable_plot_flag in input file...no plots will be generated.\n");
   		pjh_inputs->enable_plot_flag = 0;
   		}
   	}
   }

if (foundentry == 0)
   return EXIT_FAILURE;
else
   return EXIT_SUCCESS;
}
