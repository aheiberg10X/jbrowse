SAMLOC=/home/andrew/school/dnavis/samtools-0.1.16
SAMPRFX=-I$(SAMLOC)
SAMSFX=-lm -lz -L$(SAMLOC) -lbam
CC=gcc -pg #-O1 optimizer doesn't print BAM?!
CFLAGS=-c -D_FILE_OFFSET_BITS=64 $(SAMPRFX)
OBJS=all_tools.o \
	defs.o \
	interval_abstraction.o \
	isvalid.o \
	isvalid_single.o \
	mates.o \
	single_reads.o

all: all_tools 
#create_index mapto coverage coverage_strength_vec locate_mates index_mates discordant_coverage haplograph

all_tools: $(OBJS)
	$(CC) $(OBJS) -o all_tools $(SAMSFX)

.o:
	$(CC) $(CFLAGS) $*.cc

clean:
	rm all_tools *.o

