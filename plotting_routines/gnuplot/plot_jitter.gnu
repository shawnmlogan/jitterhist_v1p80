
set datafile separator ",";
set terminal qt size 1200,600 font "Verdana,12";
set termopt enhanced;

# base_filename = ARGV[1];
# plot_title = ARGV[2];
# nom_freq_text = ARGV[3];

# base_filename = sprintf("jitter_corrected");
# base_filename = sprintf("jitter");
input_filename = sprintf("./%s.csv",base_filename);
output_filename = sprintf("%s.png",base_filename);

# Replace any underscores in filename with "\\\_" to avoid being treated as
# enhanced text in plot title

system(sprintf("echo \"%s\" > ./tempfile",base_filename));
system("sed '1s/_/\\\\\\\\\\\\_/g' ./tempfile > ./tempfile1");
base_filename_text = "`cat ./tempfile1`";
system("rm tempfile tempfile1");



stats input_filename u 1:2 skip 1 nooutput;

x_min = STATS_min_x;
x_max = STATS_max_x;

neg_error_min = STATS_min_y;
neg_error_max = STATS_max_y;
neg_error_rms = STATS_stddev_y;

stats input_filename u 1:3 skip 1 nooutput;

pos_error_min = STATS_min_y;
pos_error_max = STATS_max_y;
pos_error_rms = STATS_stddev_y;

if (neg_error_max > pos_error_max) {
	y_max = neg_error_max;
	} else {
	y_max = pos_error_max;
	}
if (neg_error_min < pos_error_min) {
	y_min = neg_error_min;
	} else {
	y_min = pos_error_min;
	}

plot_header = sprintf("{/:Bold Negative and Positive Time Interval Error versus Time}\n{/:Bold Time interval error computed relative to %s}\n%s, {/:Bold filename:}\n %s.csv",nom_freq_text,plot_title,base_filename_text);
set title plot_header;

# Offset title using graph coordinates to better fit page (range 0.0 to 1.0)
set title offset graph -0.04,0.0; 

set xlabel 'Time (s)';
set ylabel 'Time Interval Error (UI)';

set tics out;
set xrange [0:x_max];
set autoscale y;

set grid x lw 1.5;
set grid y lw 1.5;
set grid xtics;
set grid ytics;
set grid mxtics;
set format x "%.3e";
set format y "%.2e";
set key center top default opaque;
set border back;
set rmargin at screen 0.95;

negative_edge_label = sprintf("Negative edge TIE:\nPeak-to-peak = %1.3e UI\nrms = %1.3e UI",neg_error_max - neg_error_min,neg_error_rms);
positive_edge_label = sprintf("Positive edge TIE:\nPeak-to-peak = %1.3e UI\nrms = %1.3e UI",pos_error_max - pos_error_min,pos_error_rms);

summary_label = sprintf("%s\n%s",negative_edge_label,positive_edge_label);
set style textbox opaque fillcolor "white" noborder;
set style textbox 2 opaque fc "light-cyan" noborder;
set label 1 summary_label at graph 0.1,0.96 left front font ",11" boxed bs 2;

plot input_filename u 1:2 title columnhead(2) with lines lw 1,input_filename u 1:3 title columnhead(3) with lines lw 1

pause(5);
set terminal push;
set terminal pngcairo size 1250,600 font "Verdana,12";
set termopt enhanced;
set key center default opaque;

set output output_filename;
replot;
set terminal pop

