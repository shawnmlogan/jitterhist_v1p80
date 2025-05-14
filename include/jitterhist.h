#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/resource.h>

#define VERSION 1.80
#define VERSION_DATE "5/11/2025"

#define LINELENGTH 128
#define LOGFILE_LINELENGTH 1024
#define CSV_FILE_LINELENGTH 180
#define FILENAME_LINELENGTH 255 /* Maximum number of characters allowed in OS X filename */
#define PREFIX_LINELENGTH 0
#define SUFFIX1_LINELENGTH 12 /* Used for appending "_corrected" to csv and png filename */
#define SUFFIX2_LINELENGTH 80 /* Used for appending psd computation parameters to csv and png filename */
#define MAXIMUM_INPUT_FILENAME_LINELENGTH FILENAME_LINELENGTH - PREFIX_LINELENGTH - (SUFFIX1_LINELENGTH + SUFFIX2_LINELENGTH)
#define TITLE_LINELENGTH 255
#define COMMAND_LINELENGTH 1024
#define OUTPUT_FILE_LINELENGTH 255 /* Maximum OSX filename limit is 255 */
#define BIG_POS_NUM 1.0e12
#define BIG_NEG_NUM -1.0e12
#define EPSILON 1.0e-12

#ifdef EXIT_CHARS
	char exitchars[] = {'q','Q'};
	int num_exitchars = sizeof(exitchars)/sizeof(exitchars[0]);
#endif

typedef enum { FALSE, TRUE } Boolean;

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
#define NUMBER_OF_VALUE_STRINGS 12 /* Used for add_units arguments */
#define LINELENGTH_OF_VALUE_STRING 20

#define SAVE_MOVING_AVERAGE_FILES 0 /* Set to 1 if want to save files containing moving average results */
#define MAXIMUM_NUMBER_OF_MOVING_AVERAGE_ITERATIONS 20

/* Duty cycle threshold for glitch detection in percent */

#define DUTY_CYCLE_MIN_PERCENT 5.0

/* Choose default window type for phase noise analysis */

#define LIMIT_MAX_PHASE_NOISE_DBC_PER_HZ BIG_POS_NUM

/* With OPTIMIZE_PSD_WINDOW set to 1, an exhaustive search of all selected windows is performed
to determine window that minimizes rms TIE between phase noise and temporal rms TIE */
/* With OPTIMIZE_PSD_WINDOW set to 0, PSD_WINDOW_TYPE is chosen for PSD analysis */

/* #define OPTIMIZE_PSD_WINDOW 1 */
#define OPTIMIZE_PSD_WINDOW 0
#define PSD_WINDOW_TYPE 1
/* #define PSD_WINDOW_TYPE 7 */
#define MAXIMUM_NUMBER_OF_PSD_WINDOWS 7
#define MINIMUM_PSD_WINDOW_NUMBER 1
#define MAXIMUM_PSD_WINDOW_NUMBER MAXIMUM_NUMBER_OF_PSD_WINDOWS
#define PSD_SEGMENT_SUBLENGTH 1
#define PSD_OVERLAP 0.0
/* #define DEBUG_OPTIMIZE_PSD_WINDOW */

#define FREQUENCY_DIFF_TO_SKIP_INTERPOLATION 0.001 /* Constant for phase noise integration function */
#define NUMBER_OF_INTERPOLATING_POINTS 3 /* Constant for phase noise integration function */
/* #define DEBUG_INTEGRATE_PN_UI */

enum integration_type {BOXCAR,POWER};
#define INTEGRATION_METHOD BOXCAR /* Most accurate for integrating discrete phase noise data */
#define LOG10_PSD_TO_PHASE_NOISE_CONVERSION_FACTOR 10.0*log10(2.0*pow(M_PI,2))

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
	int file_entry_flag; /* Flag to indicate if entry source is a file (1) or command line (0) */
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
	long int psd_window_numbers_list[MAXIMUM_NUMBER_OF_PSD_WINDOWS];
	long int *ppsd_window_numbers_list;
	int number_of_psd_windows;
	int optimize_window_flag;
	long int window_number;
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
	
/* Function prototypes */

int filecheck(char *pfilename);
Boolean check_for_cr_only(char *pline);
Boolean check_for_quit_characters(char *pline);
void remove_whitespace(char *pstring);
void remove_whitespace_only(char *pstring);
void remove_carriage_return(char *pline);
int pop_data(char *pfin, char **ppfile_argv,jitterhist_inputs *pjh_inputs);
int assign(char *pidentifier,char *pentry, char **ppfile_argv, jitterhist_inputs *pjh_inputs);
void create_sample_file(char *pfname);
int parsecsv_to_array(char *pdummy,long int *ppsd_windows_list,int *parray_size,int max_array_size, double range_min, double range_max);
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

int check_jitterhist_inputs(char *argv[], int argc, char *plog_string,jitterhist_inputs *pjh_inputs);

int validate_input_file(char *pfin, int *column_number, long int *number_of_input_lines);
int moving_average(char *pfilein, char *pfileout,int column_number,int num_moving_average_samples,long int *pnumber_output_lines);

int find_zero_crossings(char *pfin, double threshold, double deltat, int number_of_data_columns, zero_crossing_stats *pzc_stats,jitterhist_inputs *pjh_inputs);

int find_timestamp(char *pdate_string,int max_characters);
int replace_string(char *pinput_string,char *poutput_string,char *porig_string,char *pnew_string,int max_output_string_length);

long int parse_3_column_csv_file(char *pinput_filename,long int start_line,long int stop_line,int x_column_number,int y_column_number,int z_column_number,double *px,double *py,double *pz);

void spfftc(complex *x, long int *n, long int *isign);
void sppowr(double *x, double *y, double *work, long int *lx, 
long int *ly, long int *iwindo, double *ovrlap, long int *nsgmts, long int *error);
double spwndo(long int *type, long int *n, long int *k);
void spmask(double *x, long int *lx, long int *type, double *tsv, long int *error);

long pow_ii(long *ap, long *bp);

void complex_div(complex *quotient, complex *numerator, complex *denominator);;
void complex_exp(complex *r, complex *z);;
void pow_ci(complex *result, complex *a, long *b);
void pow_zi(doublecomplex *p, doublecomplex *a, long *b); 	/* p = a**b  */
void r_cnjg(complex *r, complex *z);
void z_div(doublecomplex *quotient, doublecomplex *numerator, doublecomplex *denominator);

double complex_abs(complex *z);
double pow_ri(double *ap, long *bp);
double r_mod(double *x, double *y);
double r_sign(double *a, double *b);

int find_window_type(long int window_num, char *pwindow_type);
double mean(double *x, long int N);
void print_repeated_char(char c,unsigned int N);
double min(double *px, long int N);
double rms(double *px, long int N,double *paverage,int zero_mean_rms_flag);

int find_psd_and_integrate(double *py,double *pedge_error_psd,double *pw,long int *pxnpoints,long int *pynpoints,long int *pwindow_number,double *poverlap,long int *pnum_segments,double *pfreq_pnoise,double *pedge_error_pnoise,int integration_method,int interpolate_pn_flag, double frequency_increment_Hz,int zero_mean_rms_flag,double *pedge_error_pnoise_rms_ui,jitterhist_inputs *pjh_inputs,long int *ppsd_error_edge);

int integrate_pn_ui(double *pfreq_Hz,double *pn_dBc,long int number_of_data_points,int integration_type,jitterhist_inputs *pn_input,double min_integration_freq_Hz,double max_integration_freq_Hz,int interpolate_pn_flag, int number_of_interpolating_points, double *pintegral);
int lin_interpolate_data(double *px,double *py,long int number_of_data_points,double *px_out, double *py_out,long int number_of_output_data_points,jitterhist_inputs *pjitterhist_inputs);
double max(double *px, long int N);

void print_string_to_log(char *pstring, jitterhist_inputs *pjitterhist_inputs);

#define PLOTTING_ROUTINES_DIR "/Users/sml/cproj/jitterhist/jitterhist_v1p80_051125"
