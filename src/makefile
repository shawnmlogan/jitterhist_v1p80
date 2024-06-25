IDIR =../include
CC=gcc
LIBS=-lm
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS = jitterhist.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = find_timestamp.o add_units_2.o check_jitterhist_inputs.o validate_input_file.o check_executable.o check_osx_executable.o append_filename_keep_extension.o find_base_filename.o find_stats_column_one_of_file.o find_stats_column_N_of_file.o find_zero_crossings.o datascan.o find_slope_intercept_xy.o moving_average.o main.o parse_3_column_csv_file.o parsestring_to_doubles_array.o program_info.o remove_carriage_return.o spfftc.o sppowr.o arith.o replace_string.o find_window_type.o mean.o print_repeated_char.o min.o find_min_pp_slope_intercept_xy.o rms.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

SH_RESULT:=$(shell set_install_dir.sh)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

jitterhist: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Comment out changing symbolic link until testing is complete
SH_RESULT:=$(shell install_symbolic_link.sh)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
#rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

