IDIR =../include
CC=gcc
LIBS=-lm
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS = jitterhist.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = find_timestamp.o add_units.o check_inputs.o check_executable.o append_filename_keep_extension.o find_base_filename.o find_stats_column_one_of_file.o find_stats_column_N_of_file.o find_zero_crossings.o datascan.o find_slope_intercept_xy.o moving_average.o main.o parsestring_to_doubles_array.o program_info.o remove_carriage_return.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

SH_RESULT:=$(shell set_install_dir.sh)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

jitterhistv16: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
#rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

