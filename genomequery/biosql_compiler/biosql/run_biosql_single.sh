if [ $# -ne 3 ] ; then
	echo 'usage: ./run_biosql.sh <biosql_code> <donor> <chrom>'
	exit 
fi

input_sql=$1
donor=$2
c=$3
interm_code="bytecode.txt"
products_dir_prefx="dst"
front_end_dir="front_end"
back_end_dir="back_end"
src_table_dir="dst"
low_level_calls="low_level.sh"
chr_info="chromo_length_info.txt"
bam_prefx="$DONOR_DIR/$donor/chr"
indx_prefx="$DONOR_DIR/$donor/chr"

cd $BIOSQL_HOME
rm -rf $src_table_dir/chr*

$front_end_dir/biosql < $input_sql > $interm_code

if test -f $low_level_calls
then
	rm $low_level_calls
fi


chr="chr"$c
chr_len=$(grep "^$c\>" $chr_info | cut -f2)
bam_file=$bam_prefx$c".bam"
indx_file=$bam_prefx$c".bam.mates.indx"
products_dir=$products_dir_prefx/$chr

if test ! -f $bam_file
then
    echo no $bam_file
fi
if test ! -f $indx_file
then
    echo no $indx_file
fi

if test ! -d $products_dir
    then
        mkdir $products_dir
fi
echo "USING MOFOCKING SINGLE RUN BIOSQL"
python code_generator.py $interm_code $products_dir $front_end_dir $back_end_dir $src_table_dir $bam_file $indx_file $chr $chr_len >> $low_level_calls


chmod +x $low_level_calls

./$low_level_calls

