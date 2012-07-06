#SAMLOC=/home/christos/downloads/samtools-0.1.6
SAMPRFX=-I$(SAMLOC)
SAMSFX=-lm -lz -L$(SAMLOC) -lbam
CPP=g++
#CPPFLAGS=-pg -g -D_FILE_OFFSET_BITS=64 $(SAMPRFX)
CPPFLAGS=-O3 -D_FILE_OFFSET_BITS=64 $(SAMPRFX)
OBJS= Text_table.o\
	all_tools.o \
	Command.o \
	Reads.o \
	Intervals.o \
	Interval_tree.o \
	Histogram.o \
	Joined.o \
	rd_condition.o \
	txt_condition.o \
	intrvl_condition.o \
	join_condition.o \
	rd_intervals.o \
	txt_intervals.o \
	join_intervals.o \
	intrvl_intervals.o


all: all_tools 
#create_index mapto coverage coverage_strength_vec locate_mates index_mates discordant_coverage haplograph

all_tools: $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o all_tools $(SAMSFX)

.o:
	$(CPP) -c $(CPPFLAGS) $*.cpp

clean:
	rm all_tools *.o

