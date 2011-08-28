if [ $# -ne 1 ] ; then
	echo 'usage: ./run_biosql.sh <biosql_code>'
	exit 
fi

input_sql=$1
interm_code="bytecode.txt"
products_dir_prefx="dst"
front_end_dir="front_end"
back_end_dir="back_end"
src_table_dir="dst"
low_level_calls="low_level.sh"
chr_info="chromo_length_info.txt"
bam_prefx="indexing/indexed/chr"
indx_prefx="indexing/indexed/chr"

$front_end_dir/biosql < $input_sql > $interm_code

if test -f $low_level_calls
then
	rm $low_level_calls
fi


for c in {1..23} X Y
do
	chr="chr"$c
	chr_len=$(grep "^$c\>" $chr_info | cut -f2)
	bam_file=$bam_prefx$c".bam"
	indx_file=$bam_prefx$c".bam.mates.indx"
	products_dir=$products_dir_prefx/$chr

	if test ! -f $bam_file
	then
		echo no $bam_file
		continue
	fi
	if test ! -f $indx_file
	then
		echo no $indx_file
		continue
	fi
	
	if test ! -d $products_dir
		then
			mkdir $products_dir
	fi
	
	python code_generator.py $interm_code $products_dir $front_end_dir $back_end_dir $src_table_dir $bam_file $indx_file $chr $chr_len >> $low_level_calls

done


chmod +x $low_level_calls

./$low_level_calls

