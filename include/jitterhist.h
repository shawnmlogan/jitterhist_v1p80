#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/resource.h>

#define VERSION 1.72
#define VERSION_DATE "8/9/2024"

#define LINELENGTH 128
#define CSV_FILE_LINELENGTH 180
#define FILENAME_LINELENGTH 255 /* Maximum number of characters allowed in OS X filename */
#define PREFIX_LINELENGTH 0
#define SUFFIX1_LINELENGTH 12 /* used for appending "_corrected" to csv and png filename */
#define SUFFIX2_LINELENGTH 80 /* used for appending psd computation parameters to csv and png filename */
#define MAXIMUM_INPUT_FILENAME_LINELENGTH FILENAME_LINELENGTH - PREFIX_LINELENGTH - (SUFFIX1_LINELENGTH + SUFFIX2_LINELENGTH)
#define TITLE_LINELENGTH 255
#define COMMAND_LINELENGTH 1024
#define OUTPUT_FILE_LINELENGTH 255 /* Maximum OSX filename limit is 255 */
#define BIG_POS_NUM 1.0e12
#define BIG_NEG_NUM -1.0e12
#define EPSILON 1.0e-12

/* Print gnuplot commands to terminal if PRINT_GNUPLOT_COMMAND is defined */

/* #define PRINT_GNUPLOT_COMMAND */

#define GNUPLOT "gnuplot" /* Identifies name of plotting program gnuplot */

/* Used in find_stats_column_N_of_file.c */

/* #define DEBUG_INTEGRATION_TYPE */
/* #define VERBOSE_DEBUG_INTEGRATION_TYPE */

/* Used in parsestring_to_doubles_array.c */

/* #define DEBUG_PARSESTRING_TO_DOUBLES_ARRAY */
/* #define DEBUG_PARSE_3_COLUMN_CSV_FILE */

/* Used to debug find_slope_intercept algorithm */

/* #define DEBUG_FIND_SLOPE_INTERCEPT */

#define MAXIMUM_FIND_SLOPE_ITERATIONS 100
#define MAX_NEG_EDGE_POS_EDGE_FREQ_DIFF_PPM 50

#define MAXIMUM_NUMBER_OF_DATA_COLUMNS 12
#define NUMBER_OF_VALUE_STRINGS 12 /*Used for add_units arguments*/
#define LINELENGTH_OF_VALUE_STRING 20

#define SAVE_MOVING_AVERAGE_FILES 0 /* Set to 1 if want to save files containing moving average results */
#define MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS 20

/* Duty cycle threshold for glitch detection in percent */

#define DUTY_CYCLE_MIN_PERCENT 5.0

/* Choose default window type for phase noise analysis */

#define LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ BIG_POS_NUM
#define PSD_WINDOW_TYPE 7
#define PSD_SEGMENT_SUBLENGTH 1
#define PSD_OVERLAP 0.0

/* Choose to open png files with OPEN_PNG_FILES_FLAG set to 1 */

#define OPEN_PNG_FILES_FLAG 0

/* Macros and constants for PSD analysis */

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))

/* Table of constant values */

static long neg_i1 = -1;
static long pos_i1 = 1;
static long pos_i2 = 2;
static long pos_i4 = 4;
static long pos_i5 = 5;
static double neg_f1 = -1.0;
static double pos_f2 = 2.0;
static double pos_d10 = 10.0;

typedef struct {
	double x,y;
	} xy_pair;

typedef struct {
	long int num_periods;
	double ave_period;
	double ave_period_corrected;
	double ave_period_neg_edge_corrected;
	double ave_period_pos_edge_corrected;
	double ave_ontime;
	double min_period, min_period_time;
	double max_period, max_period_time;
	double min_ontime, min_ontime_time;
	double max_ontime, max_ontime_time;
	} zero_crossing_stats;	

typedef struct {
	char *pinput_filename;
	char input_filename[FILENAME_LINELENGTH + 1];
	long int number_of_input_lines;
	int column_number;
	double fs_GHz;
	double threshold;
	int num_moving_average_samples;
	double ave_freq_MHz;
	int correct_slope_flag;
	int use_ave_freq_flag;
	char *poutput_filename;
	char output_filename[FILENAME_LINELENGTH + 1];
	int enable_plot_flag;
	char *pgnuplot_path;
	char gnuplot_path[LINELENGTH + 1];
	char *ptimestamp;
	char timestamp[LINELENGTH + 1];
	char *plog_filename;
	char log_filename[FILENAME_LINELENGTH + 1];
	} jitterhist_inputs;

typedef struct {
	double r, i;
} complex;

typedef struct {
	double r, i;
} doublecomplex;
	
/*Function prototypes*/

void program_info(int verbose);
void remove_carriage_return(char *pline);
int check_executable(char *pprogram_executable,char *preturn_string);
int check_osx_executable(char *pprogram_executable,char *preturn_string);
int parsestring_to_doubles_array(char *pinput_string,double *pdoubles_array,int *parray_size,int max_array_size);
char * add_units_2(double value,int num_digits,int width,int min_string_length,char *suffix,char *pvalue_string);
int find_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,char *ptimestamp,double *pslope, double *pintercept);
int find_min_pp_slope_intercept_xy(xy_pair *pxy,long int number_of_xy_data_records,char *ptimestamp,double computed_slope, double x_mean, double y_mean, double *pslope, double *pintercept, double *peak_to_peak);

int append_filename_keep_extension(char *pfin, char *pfout_appended, char *pappended_string, int max_num_characters);
int find_base_filename(char *pfin, char *pbase_filename);

int datascan(char *pfin,int column_number, char *pfout,int writefile,zero_crossing_stats *pzc_stats,\
double deltat, double threshold,\
double *pmax_pp_neg_edge_error,double *pmax_pp_pos_edge_error);

int find_stats_column_N_of_file(char *pfin, int column_number, double *ave_input_signal, double *min_input_signal, double *max_input_signal,long int *number_of_input_signal_lines, int *sorted_flag, int *slope_constant_flag);

int check_jitterhist_inputs(char *argv[], int argc, jitterhist_inputs *pjh_inputs);

int validate_input_file(char *pfin, int *column_number, long int *number_of_input_lines);
int moving_average(char *pfilein, char *pfileout,int column_number,int num_moving_average_samples,long int *pnumber_output_lines);

int find_zero_crossings(char *pfin, double threshold, double deltat, int number_of_data_columns, zero_crossing_stats *pzc_stats);

int find_timestamp(char *pdate_string,int max_characters);
int replace_string(char *pinput_string,char *poutput_string,char *porig_string,char *pnew_string,int max_output_string_length);

long int parse_3_column_csv_file(char *pinput_filename,long int start_line,long int stop_line,int x_column_number,int y_column_number,int z_column_number,double *px,double *py,double *pz);

void spfftc(complex *x, long *n, long *isign);
void sppowr(double *x, double *y, double *work, long *lx, 
long *ly, long *iwindo, double *ovrlap, long *nsgmts, long *error);

int find_window_type(long int window_num, char *pwindow_type);
double mean(double *x, long int N);
void print_repeated_char(char c,unsigned int N);
double min(double *px, long int N);
double rms(double *px, long int N,double *paverage,int zero_mean_rms_flag);

#define PLOTTING_ROUTINES_DIR "/Users/sml/cproj/jitterhist/jitterhist_v1p72_080924"
