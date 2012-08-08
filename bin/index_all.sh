bam_prefx=$1
#bam_prefx="sangwoo/chr"
mates_prefx=$bam_prefx
indx_prefx=$bam_prefx

loc_mates=$BIOSQL_HOME"/indexing/locate_mates"
indx_mates=$BIOSQL_HOME"/indexing/index_mates"
chr_info=$BIOSQL_HOME"/chromo_length_info$2.txt"

for c in {1..22} X Y
do
	chr=chr$c
	chr_len=$(grep "^$c\>" $chr_info | cut -f2)
	echo $chr
	bam=$bam_prefx$c".bam"
	mates=$mates_prefx$c".bam.mates"
	indx=$indx_prefx$c".mates.indx"

	$loc_mates $bam > $mates
	$indx_mates $mates $bam $indx $chr $chr_len
done



