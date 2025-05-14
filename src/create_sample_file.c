# include "jitterhist.h"


void create_sample_file(char *pfname)
{

FILE *fpw1;

if ((fpw1 = fopen(pfname,"w")) == NULL)
	{
	printf("Unable to open sample file! Check directory permissions and file space.\n");
	}
else
	{
	fprintf(fpw1,"* Comment lines begin with * or do not contain a colon\n");
	fprintf(fpw1,"* Enter a value for each input after the colon delimiter\n");
	fprintf(fpw1,"filename_with_data_samples_(csv format): <Enter your csv input file with data samples>\n");
	fprintf(fpw1,"column_number: <Enter column number of your csv input file with data samples>\n");
	fprintf(fpw1,"output_csv_filename: <Enter your csv output filename with TIE results>\n");
	fprintf(fpw1,"sample_rate_in_GHz: <Enter the sampling rate of your data samples in GHz>\n");
	fprintf(fpw1,"signal_threshold_(V): <Enter the logic threshold in Volts>\n");
	fprintf(fpw1,"num_moving_average_samples: <Enter the number of moving average samples to use for TIE computation (set to 0 for none)>\n");
	fprintf(fpw1,"correct_slope?_(y/n): <Enter yes to correct slope of computed TIE or no to leave uncorrected>\n");
	fprintf(fpw1,"Use_ave_frequency?_(y/n): <Enter yes to compute TIE using the computed average frequency or no to enter a frequency in MHz>\n");
	fprintf(fpw1,"Average_frequency_to_use_if_no_(MHz): <If above entry is a no, enter the frequency in MHz to compute TIE>\n");
	fprintf(fpw1,"Optimize_window_function?_(y/n): <Enter yes to optimize window used to compute phase noise or no to use a single window>\n");
	fprintf(fpw1,"*\n");
	fprintf(fpw1,"* Window numbers for available window types:\n");
	fprintf(fpw1,"*\n");
	fprintf(fpw1,"* Rectangular 1\n");
	fprintf(fpw1,"* Tapered rectangular 2\n");
	fprintf(fpw1,"* Triangular 3\n");
	fprintf(fpw1,"* Hanning 4\n");
	fprintf(fpw1,"* Hamming 5\n");
	fprintf(fpw1,"* Blackman 6\n");
	fprintf(fpw1,"* Blackman-hanning 7\n");
	fprintf(fpw1,"*\n");
	fprintf(fpw1,"List_of_window_numbers_for_optimization_if_yes: <Enter comma separated list of window numbers to use for optimization if optimization entry is a yes>\n");
	fprintf(fpw1,"Window_number_if_no: <Enter window number if optimization entry is a no>\n");
	fprintf(fpw1,"plot_outputs?_(y/n): <Enter yes to enable plots or no to disable plots>\n");
	fclose(fpw1);
	}

return;
}


