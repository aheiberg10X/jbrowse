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
import time
import re
from subprocess import Popen, PIPE

err_filename = "%s/delete_table_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/delete_table_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()

utils.printToServer( 'Content-type: text/html\n\n' )

def validateSchema( first_line, filename ) :

    #name check
    m = rename.match( first_line )
    linename = m.group(1)
    name_check = linename.lower() == filename.lower()

    #TODO:
    #column type checking?

    if name_check :
        return (True,"good to go")
    else :
        return (False, "%s != %s" % (linename, filename))

table_names = fields.getvalue("table_names")
project_name = fields.getvalue("project_name")

print fields
print "table_names", table_names
print "project_names", project_name

table_names = table_names.split(',')

src_table_dir = "%s/src_tables/%s" % (os.environ["BIOSQL_HOME"], project_name)
for table_name in table_names :
    os.remove( "%s/%s.txt" % (src_table_dir, table_name) )

    #TODO: clean up tables.txt

json_data = "{'status':'OK','message':'%s'}" % "No display" 

utils.printToServer( json_data );
