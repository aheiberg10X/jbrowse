#/bin/bash

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X Y
do

    perl bam2ncl.pl NA18507 $i q1 linking

done

#perl bam_to_json_paired_cgi.pl NA18507 1 q2 not_linking
