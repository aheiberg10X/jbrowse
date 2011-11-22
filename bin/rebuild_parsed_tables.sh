#!/bin/bash

cd $BIOSQL_HOME

front_end/biosql < tables.txt > parsed_tables
