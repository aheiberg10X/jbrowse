#chr chrY
cp back_end/prod_Discordant.READSrd_condition.cpp back_end/rd_condition.cpp 
make
back_end/all_tools filter_reads /media/data/work/NA18506/chrY.mates.indx dst/NA18506/chrY/Discordant.flt 
cp back_end/prod_Disc2Intrvl.2.Disc2Intrvl.Discordantrd_intervals.cpp back_end/rd_intervals.cpp 
 make
back_end/all_tools reads2intervals /media/data/work/NA18506/chrY.mates.indx dst/NA18506/chrY/Discordant.flt 1 dst/NA18506/chrY/Disc2Intrvl.intrvl
back_end/all_tools interval_merge dst/NA18506/chrY/Disc2Intrvl.intrvl dst/NA18506/chrY/Deep_Intrvl.intrvl lower 5
awk '{if($2=="chrY") print $0}' src_tables/NA18506_known_deletions.txt > dst/NA18506/chrY/NA18506_known_deletions.txt
cp back_end/prod_out.3.out.NA18506_known_deletionstxt_intervals.cpp back_end/txt_intervals.cpp 
 make
back_end/all_tools text2intervals  dst/NA18506/chrY/NA18506_known_deletions.txt dst/NA18506/chrY/out.NA18506_known_deletions.3.intrvl
back_end/all_tools joined_creation dst/NA18506/chrY/out.joined dst/NA18506/chrY/Deep_Intrvl.intrvl Intervals dst/NA18506/chrY/NA18506_known_deletions.txt Text_table dst/NA18506/chrY/Deep_Intrvl.intrvl dst/NA18506/chrY/out.NA18506_known_deletions.3.intrvl
back_end/all_tools print Joined dst/NA18506/chrY/out.joined  dst/NA18506/chrY/out
