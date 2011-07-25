#!/bin/bash

if [ $# -ne 3 ] ; then
	echo 'usage: ./run_biosql.sh <biosql_code> <bam_file> <index_file>'
	exit 
fi

cd ../genomequery/biosql_compiler/biosql

input_sql=$1
bam_file=$2
indx_file=$3
interm_code="bytecode.txt"
products_dir="dst"
front_end_dir="front_end"
back_end_dir="back_end"

low_level_calls="low_level.sh"

echo '1'
$front_end_dir/biosql < $input_sql > $interm_code
echo '2'

echo "python code_generator.py $interm_code $products_dir $front_end_dir $back_end_dir $bam_file $indx_file > $low_level_calls"

python code_generator.py $interm_code $products_dir $front_end_dir $back_end_dir $bam_file $indx_file > $low_level_calls
echo "3: low_level_calls: $low_level_calls"
chmod +x $low_level_calls
echo '4'
./$low_level_calls
echo '5'
