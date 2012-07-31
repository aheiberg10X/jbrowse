#!/usr/bin/python

import cgi
import simplejson as json
import os
import os.path
import shutil
import sys
sys.path.append("../lib")
import GlobalConfig
import utils
import re

err_filename = "%s/view_schema_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/view_schema_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

utils.printToServer( 'Content-type: text/json\n\n' )

fields = cgi.FieldStorage()
print fields
project_name = fields.getvalue("project_name")
table_name = fields.getvalue("table_name")
print "project_name", project_name

ftables_txt = open( "%s/tables.txt" % os.environ["BIOSQL_HOME"] )
src_table_dir = "%s/src_tables/%s" % (os.environ["BIOSQL_HOME"], project_name)

re_extracted_table_name = re.compile(r'table\s+(\w+)\s+(.*);', re.I)
status, message = "bad", "*something* went wrong with schema viewing"
for line in ftables_txt.readlines() :
    line = line.strip()
    m = re_extracted_table_name.match( line )
    print line
    if m :
        print "     match: %s " % m.group(1)  
        lines_table_name = m.group(1)
        print "table_name, {%s}, lines_table_name, {%s}" % (table_name, lines_table_name) 
        if table_name == lines_table_name :
            print "dealing with this line: %s" % line
            status, message = "ok", line
            break
    else :
        print "regexp not matching, not good"
        message = "regexp not matching"

ftables_txt.close()

print status, message
utils.printToServer( '{"status": "%s", "message": "%s" }' % (status,message) )
