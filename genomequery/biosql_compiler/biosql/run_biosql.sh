if [ $# -ne 4 ] ; then
	echo 'usage: ./run_biosql.sh <biosql_code> <dst_dir> <src_table_dir> <ucsc_hg>'
    echo "biosql code is a file with the GQL query"

    echo "dst_dir is the directory where the output (bam and stuff for jbrowse) goes.  Because we can run across multiple donors and multiple chromosomes, this string is just a template, with donor and chrom supplied in the loops below.  It will look something like: /home/andrew/jbrowse/data/tracks/project_<project_name>/donor_%s/query_<query_name>/chrom_%s"

    echo "src_table_dir contains the table files that users can use in their queries, e.g genes.  The name and columns of each of these files needs to be in tables.txt, which needs to get built into parsed_tables (see upload_interval_tables.py and rebuild_parsed_tables.sh"

    echo "ucsc_hg is the number of the reference assembly associated with the project"
	exit 
fi

input_sql=$1
dest_template=$2 #"dst"
src_table_dir=$3 

interm_code="bytecode.txt"
front_end_dir="front_end"
back_end_dir="back_end"
low_level_calls="low_level.sh"
chr_info="chromo_length_info$4.txt"
#bam_prefx="$DONOR_DIR/$donor/chr"
#indx_prefx="$DONOR_DIR/$donor/chr"

cd $BIOSQL_HOME
#rm -rf $src_table_dir/chr*

$front_end_dir/biosql < $input_sql > $interm_code

if test -f $low_level_calls
then
	rm $low_level_calls
fi

genomes=`grep genome bytecode.txt | cut -d " " -f3` #get the individual genomes
for donor in $genomes
do ###complete things here
	bam_prefx="$DONOR_DIR/$donor/chr"
	indx_prefx="$DONOR_DIR/$donor/chr"

	for c in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X Y
	do
		chr="chr"$c
		chr_len=$(grep "^$c\>" $chr_info | cut -f2)
		bam_file=$bam_prefx$c".bam"
		indx_file=$bam_prefx$c".mates.indx"
		chrom_dir=`printf "$dest_template" $donor $chr`

		if test ! -f $bam_file
		then
			echo no bamfile $bam_file
			continue
		fi
		if test ! -f $indx_file
		then
			echo no indxfile $indx_file
			continue
		fi

        query_dir=${chrom_dir%/*}        
		if test ! -d $query_dir
		then
		    mkdir $query_dir
		fi
		if test ! -d $chrom_dir
		then
		    mkdir $chrom_dir
		fi

        echo chr_len $chr_len
		
		python code_generator.py $interm_code $chrom_dir $front_end_dir $back_end_dir $src_table_dir $bam_file $indx_file $chr $chr_len >> $low_level_calls

	done
done

chmod +x $low_level_calls
./$low_level_calls

