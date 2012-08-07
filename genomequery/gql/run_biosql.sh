if [ $# -ne 5 ] ; then
    echo 'usage: ./run_biosql.sh <biosql_code> <dst_dir> <src_table_dir> <assembly> <user_donor_dir>'
    exit 
fi

input_sql=$1
dest_template=$2 #"dst"
src_table_dir=$3

echo input_sql $input_sql
echo dest_template $dest_template
echo src_table_dir $src_table_dir

interm_code="bytecode.txt"
front_end_dir="front_end"
back_end_dir="back_end" #the directory where the src files of the back end are.
low_level_calls="low_level.sh"
chr_info="chromo_length_info$4.txt"
log="dummy_log.txt"
table_defs="tables.txt"
#bam_prefx="$DONOR_DIR/$donor/chr"
#indx_prefx="$DONOR_DIR/$donor/chr"

cd $BIOSQL_HOME
#rm -rf $src_table_dir/chr*
echo "table READS (string id, string read_str, integer length, string
qvals,string chr, integer location, char strand, string
match_descr, string mate_chromo, integer mate_loc, char mate_strand);" > $table_defs
for f in $src_table_dir/*; do head -1 $f | cut -d# -f2; done >> $table_defs


$front_end_dir/biosql < $input_sql > $interm_code

if test -f $low_level_calls
then
    rm $low_level_calls
fi

genomes=`grep genome bytecode.txt | cut -d " " -f2` #get the individual genomes
for gen in $DONOR_DIR/$genomes
do 
    echo donoooor $donor
    bam_prefx="$user_donor_dir/$donor/chr"
    indx_prefx="$user_donor_dir/$donor/chr"

    for c in 1 #2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X Y
    do
        chr="chr"$c
        echo "#chr $chr" > $low_level_calls
        echo "#chr $chr" >> $log
        chr_len=$(grep "^$c\>" $chr_info | cut -f2)
        bam_file=$bam_prefx$c".bam"
        indx_file=$bam_prefx$c".mates.indx"
        #products_dir=$products_dir_prefx/$donor/$chr
        chrom_dir=`printf "$dest_template" $donor $chr`

        if test ! -f $bam_file
        then
            echo no bamfile $bam_file
            continue
        fi
        if test ! -f $indx_file
        then
            echo no indexfile $indx_file
            continue
        fi
    
        query_dir=${chrom_dir%/*}    
        echo query_dir $query_dir
        if test ! -d $query_dir
        then
            mkdir $query_dir
        fi
        if test ! -d $chrom_dir
        then
                mkdir $chrom_dir
        fi

        echo hellleleleleleooooo
        
        python code_generator.py $interm_code $chrom_dir $front_end_dir $back_end_dir $src_table_dir $bam_file $indx_file $chr $chr_len >> $low_level_calls
        echo done with code_generator
        chmod +x $low_level_calls
        /usr/bin/time -f "real %e user %U sys %S avg_mem(KB) %K max_resident(KB) %M avg_resident(KB) %t" ./$low_level_calls 2>> $log

    done
done


