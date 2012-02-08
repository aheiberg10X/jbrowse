#!/bin/bash

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 21 X Y
do
    ./prepare-refseqs.pl --fasta ../hg19/chr$i.fa --out ../hg19
done
