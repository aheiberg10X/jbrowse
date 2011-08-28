bam_prefx="NA18507/chr"
mates_prefx=$bam_prefx
indx_prefx=$bam_prefx

loc_mates="indexing/locate_mates"
indx_mates="indexing/index_mates"

for c in {3..23} X Y
do
	echo chr$c
	bam=$bam_prefx$c".bam"
	mates=$mates_prefx".mates"
	indx=$indx_prefx".mates.indx"

	$loc_mates $bam > $mates
	$indx_mates $mates $bam $indx
done



