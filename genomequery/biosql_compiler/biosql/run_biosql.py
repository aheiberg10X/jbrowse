#!/usr/bin/python
import sys

input_sql = sys.argv[1]

if not input_sql :
    print 'usage: ./run_biosql.sh <biosql_code>'

interm_code = "bytecode.txt"
products_dir = "dst"
front_end_dir = "front_end"
back_end_dir = "back_end"

low_level_calls = "low_level.sh"


