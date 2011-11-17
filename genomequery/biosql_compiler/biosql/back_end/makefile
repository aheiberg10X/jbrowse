SAMPRFX=-I$(SAMLOC)
SAMSFX=-lm -lz -L$(SAMLOC) -lbam
CC=gcc -O3 -pg -g
CFLAGS=-c -D_FILE_OFFSET_BITS=64 $(SAMPRFX)
OBJS=all_tools.o \
	defs.o \
	interval_abstraction.o \
	isvalid.o \
	mates.o \
	single_reads.o \
	interval_tree.o \
	load_table.o \
	isvalid_imported.o \
	isvalid_join.o	\
	get_rd_interval.o \
	get_imported_interval.o \
	get_interval_coverage_param.o

all: all_tools 
#create_index mapto coverage coverage_strength_vec locate_mates index_mates discordant_coverage haplograph

all_tools: $(OBJS)
	$(CC) $(OBJS) -o all_tools $(SAMSFX)

.o:
	$(CC) $(CFLAGS) $*.cc

clean:
	rm all_tools *.o

