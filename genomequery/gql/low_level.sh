#chr chr1
cp back_end/prod_H1.READSrd_condition.cpp back_end/rd_condition.cpp 
make
back_end/all_tools filter_reads /home/andrew/jbrowse/donors/NA18507/chr1.mates.indx /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H1.flt 
cp back_end/prod_H2.2.H2.H1rd_intervals.cpp back_end/rd_intervals.cpp 
 make
back_end/all_tools reads2intervals /home/andrew/jbrowse/donors/NA18507/chr1.mates.indx /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H1.flt 0 /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H2.intrvl
back_end/all_tools interval_merge /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H2.intrvl /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H3.intrvl lower 5
cp back_end/prod_out.3.out.H1rd_intervals.cpp back_end/rd_intervals.cpp 
 make
back_end/all_tools reads2intervals /home/andrew/jbrowse/donors/NA18507/chr1.mates.indx /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H1.flt 1 /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/out.H1.3.intrvl
back_end/all_tools joined_creation /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/out.joined /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H3.intrvl Intervals 1/home/andrew/jbrowse/donors/NA18507/chr1.mates.indx Reads /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/H3.intrvl /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/out.H1.3.intrvl
back_end/all_tools print Joined /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/out.joined  /home/andrew/jbrowse/data/tracks/project_main/donor_NA18507/query_inv/chrom_chr1/out
