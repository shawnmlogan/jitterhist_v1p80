
set datafile separator ",";
set terminal qt size 1000,600 font "Verdana,16";
set termopt enhanced;

# base_filename = sprintf("jitter_corrected");
# plot_title = sprintf("my title");
# nom_freq_text = sprintf("92 MHz");
# plotting_routines_dir = sprintf("/Users/sml/cproj/jitterhist...");

nom_freq_Hz = 100e6;

input_filename = sprintf("%s.csv",base_filename);
output_filename = sprintf("%s_norm.png",base_filename);

# Replace any underscores in filename with "\\\_" to avoid being treated as
# enhanced text in plot title

system(sprintf("echo \"%s\" > tempfile",base_filename));
system("sed '1s/_/\\\\\\\\\\\\_/g' tempfile > tempfile1");
base_filename_formatted = "`cat tempfile1`";
system("rm tempfile tempfile1");

stats input_filename u 1:2 skip 1 nooutput;

x_min = nom_freq_Hz*STATS_min_x;
x_max = nom_freq_Hz*STATS_max_x;

neg_error_min_ui = STATS_min_y;
neg_error_max_ui = STATS_max_y;
neg_error_rms_ui = STATS_stddev_y;

stats input_filename u 1:3 skip 1 nooutput;

pos_error_min_ui = STATS_min_y;
pos_error_max_ui = STATS_max_y;
pos_error_rms_ui = STATS_stddev_y;

if (neg_error_max_ui > pos_error_max_ui) {
	y_max = neg_error_max_ui;
	} else {
	y_max = pos_error_max_ui;
	}
if (neg_error_min_ui < pos_error_min_ui) {
	y_min = neg_error_min_ui;
	} else {
	y_min = pos_error_min_ui;
	}

max_num_ticks = 8;
base = 10.0;
xtick_increment = (x_max - x_min)/max_num_ticks;
xtick_increment = base**(floor(log10(xtick_increment)/log10(base) + 0.50));

x_limit_min = xtick_increment * floor(x_min/xtick_increment);
x_limit_max = xtick_increment * ceil(x_max/xtick_increment);

set xrange [x_limit_min:x_limit_max];
if ((x_limit_max - x_limit_min)/xtick_increment < max_num_ticks) {
set xtics x_limit_min,xtick_increment,x_limit_max;
} else {
set xtics x_limit_min,2*xtick_increment,x_limit_max;
set mxtics 2;
set grid mxtics;
}

max_num_ticks = 8;
base = 10.0;
ytick_increment = (y_max - y_min)/max_num_ticks;
ytick_increment = base**(floor(log10(ytick_increment)/log10(base) + 0.50));

y_limit_min = ytick_increment * (floor(y_min/ytick_increment) - 1.0);
y_limit_max = ytick_increment * (1.0 + ceil(y_max/ytick_increment)); # Allow added height for legend

set yrange [y_limit_min:y_limit_max];
if ((y_limit_max - y_limit_min)/ytick_increment < max_num_ticks) {
set ytics y_limit_min,ytick_increment,y_limit_max;
} else {
set ytics y_limit_min,2*ytick_increment,y_limit_max;
set mytics 2;
set grid mytics;
}

# Check for a long filename that will exceed plot width; wrap filename if necessary
if (strlen(input_filename) > 80) {

# Wrap input filename

length_input_filename = strlen(input_filename);
# start_wrap_character_number = strstrt(input_filename,"_jitter") - 16;
start_wrap_character_number = floor(length_input_filename/2);
substring1_input_filename = substr(input_filename,0,start_wrap_character_number);
substring2_input_filename = substr(input_filename,start_wrap_character_number + 1,length_input_filename);

sys_command = sprintf("echo %s > .tempfile0",substring1_input_filename);
system sys_command;
sys_command = sprintf("sed '1,$s/_/\\\\_/g' .tempfile0 > .tempfile1\n");
system sys_command;
substring1_input_filename_formatted = system("cat .tempfile1");
system "rm .tempfile0 .tempfile1";

sys_command = sprintf("echo %s > .tempfile0",substring2_input_filename);
system sys_command;
sys_command = sprintf("sed '1,$s/_/\\\\_/g' .tempfile0 > .tempfile1\n");
system sys_command;
substring2_input_filename_formatted = system("cat .tempfile1");
system "rm .tempfile0 .tempfile1";

plot_header = sprintf("{/:Bold Negative and Positive Time Interval Error versus Time}\n{/:Bold Time interval error computed relative to %s}\n%s, {/:Bold filename:}\n{/:=12 %s...}\n{/:=12 %s}",nom_freq_text,plot_title,substring1_input_filename_formatted,substring2_input_filename_formatted);

} else {
plot_header = sprintf("{/:Bold Negative and Positive Time Interval Error versus Time}\n{/:Bold Time interval error computed relative to %s}\n%s, {/:Bold filename:}\n{/:=12 %s.csv}",nom_freq_text,plot_title,base_filename_formatted);
}

set title plot_header;

# Offset title using graph coordinates to better fit page (range 0.0 to 1.0)
set title offset graph -0.04,0.0; 

set xlabel 'Time (normalized to periods of 100 MHz sinusoid)';
set ylabel 'Time Interval Error (UI)';

set tics out;

set grid x lw 1.5;
set grid y lw 1.5;
set grid xtics;
set grid ytics;

if ((x_limit_max - x_limit_min > 1) && (x_limit_max - x_limit_min < 10000)) {
set format x "%.0f";
} else {
num_digits = floor(log10(abs(x_limit_max)/xtick_increment));
format_specifier = sprintf("%%.%de",num_digits);
set format x format_specifier;
# set format x "%.1e";
}

if ((y_limit_max - y_limit_min > 1) && (y_limit_max - y_limit_min < 10000)) {
set format y "%.0f";
} else {
num_digits = floor(log10(abs(y_limit_max)/ytick_increment));
format_specifier = sprintf("%%.%de",num_digits);
set format y format_specifier;
}

set key box autotitle columnhead;
set key right opaque width -2;
set key font "Verdana,12";
set border back;
set rmargin at screen 0.95;

# Format numerical data as strings #

neg_edge_error_pp_ui = neg_error_max_ui - neg_error_min_ui;
pos_edge_error_pp_ui = pos_error_max_ui - pos_error_min_ui;

num_digits = 3;
suffix = sprintf("UI");
awk_comamnd_0 = sprintf("echo \"%1.12e\" > .gnu_number",neg_edge_error_pp_ui);
awk_comamnd_1 = sprintf("awk -f %s/plotting_routines/gnuplot/add_units.nawk num_digits=%d suffix=%s \.gnu_number > \.gnu_number_formatted",plotting_routines_dir,num_digits,suffix);

system(awk_comamnd_0);
system(awk_comamnd_1);
neg_edge_error_pp_ui_string = system("cat .gnu_number_formatted");
system("rm .gnu_number .gnu_number_formatted");

awk_comamnd_0 = sprintf("echo \"%1.12e\" > .gnu_number",neg_error_rms_ui);
awk_comamnd_1 = sprintf("awk -f %s/plotting_routines/gnuplot/add_units.nawk num_digits=%d suffix=%s \.gnu_number > \.gnu_number_formatted",plotting_routines_dir,num_digits,suffix);

system(awk_comamnd_0);
system(awk_comamnd_1);
neg_error_rms_ui_string = system("cat .gnu_number_formatted");
system("rm .gnu_number .gnu_number_formatted");

awk_comamnd_0 = sprintf("echo \"%1.12e\" > .gnu_number",pos_edge_error_pp_ui);
awk_comamnd_1 = sprintf("awk -f %s/plotting_routines/gnuplot/add_units.nawk num_digits=%d suffix=%s \.gnu_number > \.gnu_number_formatted",plotting_routines_dir,num_digits,suffix);

system(awk_comamnd_0);
system(awk_comamnd_1);
pos_edge_error_pp_ui_string = system("cat .gnu_number_formatted");
system("rm .gnu_number .gnu_number_formatted");

awk_comamnd_0 = sprintf("echo \"%1.12e\" > .gnu_number",pos_error_rms_ui);
awk_comamnd_1 = sprintf("awk -f %s/plotting_routines/gnuplot/add_units.nawk num_digits=%d suffix=%s \.gnu_number > \.gnu_number_formatted",plotting_routines_dir,num_digits,suffix);

system(awk_comamnd_0);
system(awk_comamnd_1);
pos_error_rms_ui_string = system("cat .gnu_number_formatted");
system("rm .gnu_number .gnu_number_formatted");

negative_edge_label = sprintf("Negative edge TIE:\nPeak-to-peak = %s\nrms = %s",neg_edge_error_pp_ui_string,neg_error_rms_ui_string);
positive_edge_label = sprintf("Positive edge TIE:\nPeak-to-peak = %s\nrms = %s",pos_edge_error_pp_ui_string,pos_error_rms_ui_string);

summary_label = sprintf("%s\n%s",negative_edge_label,positive_edge_label);
set style textbox opaque fillcolor "white" noborder;
set style textbox 2 opaque fc "light-cyan" noborder;
set label 1 summary_label at graph 0.1,0.96 left front font "Verdana,14" boxed bs 2;

plot input_filename u ($1*nom_freq_Hz):2 title columnhead(2) with lines lw 1,input_filename u 1:3 title columnhead(3) with lines lw 1

pause(5);
set terminal push;
set terminal pngcairo enhanced size 1200,800 font "Verdana,16";

set output output_filename;
replot;
set terminal pop

