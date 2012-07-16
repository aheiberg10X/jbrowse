#!/bin/bash

echo make sure things are named chr1.bam, chr2.bam, ...

donor_name=$0
sorted=$1
project=$2

location=/home/uploads/$donor_name

#TODO
#export all the environment variables to the genomequery.conf and .bashrc
#then have scripts access these directly, rather than the module approach
#right now
project_dir=$GQ_ROOT/data/tracks/project_$project

#TODO
#permissions are a big issue here
#user will upload, but www-data will not have rights to do any moving/modding
#so right now this is just a convenience script for me
donor_loc=/home/andrew/jbrowse/donors
mv $location $donor_loc/$donor_name

cd $donor_loc

if $sorted then
    ./index_all.sh $donor_name
else
    ./sort_all.sh $donor_name
    ./index_all.sh $donor_name
fi

mkdir $project_dir/donor_$donor_name

#anything else?
