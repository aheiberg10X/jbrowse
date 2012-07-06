SAMLOC=/home/christos/downloads/samtools-0.1.6
SAMPRFX=-I$(SAMLOC)
SAMSFX=-lm -lz -L$(SAMLOC) -lbam
CC=gcc -pg -O3
CFLAGS=-c -D_FILE_OFFSET_BITS=64

all: locate_mates index_mates


locate_mates: locate_mates.o defs.o
	$(CC) locate_mates.o defs.o -o locate_mates $(SAMSFX)

index_mates: index_mates.o defs.o
	$(CC) index_mates.o defs.o -o index_mates $(SAMSFX)

defs.o: defs.c
	$(CC) $(CFLAGS) defs.c

locate_mates.o: locate_mates.c
	$(CC) $(CFLAGS) $(SAMPRFX) locate_mates.c

index_mates.o: index_mates.c
	$(CC) $(CFLAGS) $(SAMPRFX) index_mates.c


clean:
	rm -rf *.o locate_mates index_mates
