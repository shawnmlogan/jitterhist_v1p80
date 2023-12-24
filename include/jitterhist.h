#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#define LINELENGTH 180
#define DEBUG
#define BIG_POS_NUM 1.0e12
#define BIG_NEG_NUM -1.0e12
#define EPSILON 1.0e-12

#define MAXIMUM_NUMBER_OF_DATA_COLUMNS 12
#define NUMBER_OF_VALUE_STRINGS 12 /*Used for add_units arguments*/
#define LINELENGTH_OF_VALUE_STRING 20
#define MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS 20

#define VERSION 1.163
#define VERSION_DATE "12/24/2023"

typedef struct {
	double x,y;
	} xy_pair;

typedef struct {
	long int num_periods;
	double ave_period;
	double ave_period_corrected;
	double ave_ontime;
	double min_period, min_period_time;
	double max_period, max_period_time;
	double min_ontime, min_ontime_time;
	double max_ontime, max_ontime_time;
	} zero_crossing_stats;	
	
/*Function prototypes*/

void program_info(int verbose);
void remove_carriage_return(char *pline);
int check_executable(char *pprogram_executable,char *preturn_string);
int parsestring_to_doubles_array(char *pinput_string,double *pdoubles_array,int *parray_size,int max_array_size);
char * add_units(double value,int num_digits,char *suffix,char *pvalue_string);
int find_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,double *pslope, double *pintercept);

int append_filename_keep_extension(char *pfin, char *pfout_appended, char *pappended_string, int max_num_characters);
int find_base_filename(char *pfin, char *pbase_filename, int max_num_characters);

int datascan(char *pfin,int column_number, char *pfout,int writefile,double ave_period,\
double deltat, double vthreshold,\
double *pmax_pp_neg_edge_error,double *pmax_pp_pos_edge_error);

int find_stats_column_one_of_file(char *pfin, double *ave_input_signal, double *min_input_signal, double *max_input_signal,long int *number_of_input_signal_lines);
int find_stats_column_N_of_file(char *pfin, int column_number, double *ave_input_signal, double *min_input_signal, double *max_input_signal,long int *number_of_input_signal_lines, int *sorted_flag);

int check_jitterhist_inputs(char *argv[], int argc, char *pfin, int *column_number, char *pfout, double *fs_GHz, double *threshold_value,
int *num_moving_average_samples, int *use_ave_freq, double *ave_freq_MHz,long int *number_of_input_lines,int *correct_slope_flag);
int validate_input_file(char *pfin, int *column_number, long int *number_of_input_lines);

int moving_average(char *pfilein, char *pfileout,int column_number,int num_moving_average_samples,long int *pnumber_output_lines);

int find_zero_crossings(char *pfin, double threshold, double deltat, int number_of_data_columns, zero_crossing_stats *pzc_stats);

int find_timestamp(char *pdate_string,int max_characters);
#define PLOTTING_ROUTINES_DIR "/Users/sml/cproj/jitterhist/jitterhistv163"
