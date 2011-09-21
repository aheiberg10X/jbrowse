back_end/all_tools load_table dst/genes.txt dst/chr1/genes 1 2 3 4 chr1
cp H2isvalid_single.c back_end/isvalid_single.c
make
back_end/all_tools single_range ../../../donors/NA18507/chr1.bam ../../../donors/NA18507/chr1.bam.mates.indx 247249719 dst/chr1/H2 0
back_end/all_tools intersect dst/chr1/genes.table imported ../../../donors/NA18507/chr1.bam.mates.indx bam_single ../../../donors/NA18507/chr1.bam dst/chr1/H3 2 247249719 dst/chr1/genes dst/chr1/H2 dummy
cp outisvalid_join.c back_end/isvalid_join.c
make
back_end/all_tools intersect dst/chr1/genes.table imported ../../../donors/NA18507/chr1.bam.mates.indx bam_single ../../../donors/NA18507/chr1.bam dst/chr1/out 5 247249719 dummy dummy dst/chr1/H3
