#!/bin/bash

cd ../genomequery/biosql_compiler/biosql
rm -f low_level.sh bytecode.txt H1isvalid_mates.c outisvalid_mates.c

rm -rf dst/chr*

cd ../../../debug
rm -f *

cd ../uploads
rm -f *
