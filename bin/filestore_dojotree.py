#!/usr/bin/python
import sys
import os
import simplejson as json
import re
import cgi
import cgitb
cgitb.enable()
#TODO
#import filestore_funcs

print os.environ.keys()

output = "/home/andrew/school/dnavis/jbrowse/bin/debugging"
sys.stderr = open("%s/filestore_err.txt" % output,'w')
sys.stdout = open("%s/filestore_out.txt" % output,'w')

rootDir = "/home/andrew/school/dnavis/jbrowse/data/tracks"

fields = cgi.FieldStorage()

query = fields.getvalue("query")


