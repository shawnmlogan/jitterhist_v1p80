
# base_filename = sprintf("Myfile");
# plot_title = sprintf("My title");
# nom_freq_text = sprintf("92 MHz");
# plotting_routines_dir = sprintf("/Users/sml/cproj/jitterhist...");
# fft_length = 512;
# limit_max_phase_noise_dbc_Hz = -20;
# sample_time_text = sprintf("5 us");
# window_type = sprintf("Blackman-Hanning");

term_type = sprintf("qt");
input_filename = sprintf("%s.csv",base_filename);
output_filename = sprintf("%s.png",base_filename);

set datafile separator ",";
set terminal term_type size 1000,600 font "Verdana,16"

# Include escape characters in filename if it includes underscores since
# using enhanced text

sys_command = sprintf("echo %s > .tempfile0",input_filename);
system sys_command;
sys_command = sprintf("sed '1,$s/_/\\\\_/g' .tempfile0 > .tempfile1\n");
system sys_command;
input_filename_formatted = system("cat .tempfile1");
system "rm .tempfile0 .tempfile1";

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

plot_header = sprintf("{/:Bold Phase Noise of Negative and Positive Edge Time Interval Errors}\n{/:Bold Time interval error computed relative to %s}\n%s, {/:Bold filename:}\n{/:=12 %s...}\n{/:=12 %s}",nom_freq_text,plot_title,substring1_input_filename_formatted,substring2_input_filename_formatted);

} else {
plot_header = sprintf("{/:Bold Phase Noise of Negative and Positive Edge Time Interval Errors}\n{/:Bold Time interval error computed relative to %s}\n%s, {/:Bold filename:}\n{/:=12 %s}",nom_freq_text,plot_title,input_filename_formatted);
}
set title plot_header offset -3.5,0;

stats input_filename u 1:2 nooutput;

number_of_records = STATS_records;
x_min = STATS_min_x;
x_max = STATS_max_x;
delta_x = (x_max - x_min)/(number_of_records - 1);
neg_edge_min_pn = STATS_min_y;
neg_edge_max_pn = STATS_max_y;

stats input_filename u 1:3 nooutput;

number_of_records = STATS_records;
pos_edge_min_pn = STATS_min_y;
pos_edge_max_pn = STATS_max_y;

if(neg_edge_max_pn > pos_edge_max_pn) {
	max_pn = neg_edge_max_pn
} else {
	max_pn = pos_edge_max_pn
}

if(neg_edge_min_pn > pos_edge_min_pn) {
	min_pn = pos_edge_min_pn
} else {
	min_pn = neg_edge_min_pn
}

if (max_pn - min_pn) > 60 {
ylim_min = 10.0*floor(min_pn/10.0);
ylim_max = 10.0*floor(max_pn/10.0) + 40.0;
yticks = 10.0;
set ytics ylim_min,yticks,ylim_max;
} else {
ylim_min = 5.0*floor(min_pn/5.0);
ylim_max = 5.0*floor(max_pn/5.0) + 20.0;
yticks = 5.0;
set ytics ylim_min,yticks,ylim_max;
}

set yrange [ylim_min:ylim_max];

# Format numerical data as strings #

num_digits = 3;
suffix = sprintf("Hz");
awk_comamnd_0 = sprintf("echo \"%1.12e\" > .gnu_number",delta_x);
awk_comamnd_1 = sprintf("awk -f %s/plotting_routines/gnuplot/add_units.nawk num_digits=%d suffix=%s \.gnu_number > \.gnu_number_formatted",plotting_routines_dir,num_digits,suffix);

system(awk_comamnd_0);
system(awk_comamnd_1);
delta_x_string = system("cat .gnu_number_formatted");
system("rm .gnu_number .gnu_number_formatted");

# Determine number of non-zero entries in file #
column_num = 2;

awk_comamnd_0 = sprintf("awk -f %s/plotting_routines/gnuplot/find_non_zero_entries.nawk column_number=%d %s > \.gnu_number",plotting_routines_dir,column_num,input_filename);

system(awk_comamnd_0);
number_non_zero_entries = 0 + system("cat .gnu_number");
system("rm .gnu_number");

set key box autotitle columnhead;
set key right opaque width -2;
set key font "Verdana,12";

set xlabel "Frequency (Hz)";
set ylabel sprintf("{/Times:Italic=18 L(f)} (dBc/Hz)") offset 0.25;
set format x "%.0e"
set format y "%.0f"

set logscale x;
set grid xtics linewidth 1.5;
set grid ytics linewidth 1.5;
set grid mxtics linewidth 1.5;

# Create label to indicate fft_length, time sample, and window used in fft analysis

label_1_text = sprintf("{/Times:Italic=18 Phase noise based on %d point FFT}\n{/Times:Italic=18 using last %s of data sample}\n{/Times:Italic=18 and a %s window.}",fft_length,sample_time_text,window_type);

set style textbox opaque fillcolor "white" noborder;
set style textbox 2 opaque fc "white" noborder;
set label 1 label_1_text at graph 0.05,0.94 left front boxed bs 2;

# Determine if warning message should be included if max_pn > limit_max_phase_noise_dbc_Hz

if (max_pn > limit_max_phase_noise_dbc_Hz) {
	label_2_text = sprintf("{/:Bold=16 Warning! Maximum phase noise of %.1f dBc/Hz exceeds}\n{/:Bold=16 %.1f dBc/Hz. {/Times:Italic=18 L(f)} approximation is compromised}\n{/:Bold=16 and its accuracy is impacted.}",max_pn,limit_max_phase_noise_dbc_Hz);
	set style textbox 3 opaque fc rgb "0xffcccc";
	set label 2 label_2_text at graph 0.05,0.22 left front boxed bs 3;
	ylim_min = ylim_min - 4*yticks
	set yrange [ylim_min:ylim_max];
	set ytics ylim_min,yticks,ylim_max;
}

plot input_filename u 1:2 title columnhead(2) with lines lw 1.5 lc rgb "0xff9933", \
input_filename u 1:3 title columnhead(3) with lines lw 1.5 lc rgb "0x3399ff";
pause(5);
set term push						# save current terminal settings

# write the graph in PNG format, pngcairo preferred to png as it supports better fonts and dashed lines

set terminal pngcairo enhanced size 1200,800 font "Verdana,16"

set output sprintf("%s",output_filename);
replot;
set term pop

