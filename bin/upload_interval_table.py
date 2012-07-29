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

err_filename = "%s/upload_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/upload_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()

utils.printToServer( 'Content-type: text/html\n\n' )

#TODO: ownership and permission for inteval files
rename = re.compile(r'#table\s+(\w+)\s+(.*);', re.I)
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

fileitem = fields["interval_table"]
project_name = fields.getvalue("project_name")
if fileitem.filename :
    handle = fileitem.file
    first_line = handle.readline().strip()
    rest_lines = handle.read()
    filename = os.path.basename(fileitem.filename)
    (name,ext) = os.path.splitext(filename)
    (ok,message) = validateSchema(first_line, name)
    if not ok :
        json_data = "{'status':'ERROR', 'message':'%s'}" % message
    else :
        #path = "%s/data/tracks/%s%s/interval_tables" % \
                    #(GlobalConfig.ROOT_DIR, \
                     #GlobalConfig.PROJECT_PREFIX, \
                     #project_name )
        #if not os.path.exists( path ) :
            #os.mkdir( path )
        src_table_dir = "%s/src_tables/%s" % (os.environ["BIOSQL_HOME"], project_name)
        if not os.path.exists( src_table_dir ) :
            os.mkdir( src_table_dir )
        newfilename = "%s/%s.txt" % (src_table_dir, name)

        if not os.path.exists( newfilename ) :
            open(newfilename, 'w').write( rest_lines )
            #update tables.txt, rebuild parsed tables

            ftables = "%s/tables.txt" % os.environ["BIOSQL_HOME"]
            ftables = open( ftables, 'a' )
            #fullname = "%s_%s" % (project_name,name)
            #schema = "table %s (string annot_id, string chr, char ornt, integer begin, integer end);\n" % fullname
            ftables.write( first_line[1:] )
            ftables.close()

            pop = Popen(['bash','rebuild_parsed_tables.sh'], \
                        stdin=PIPE, stdout=PIPE, stderr=PIPE)
            (out, err) = pop.communicate()
            sys.stderr.write(err)
            sys.stdout.write(out)

            #check err here?

            json_data = "{'status':'OK','message':'%s'}" % name
        else :
            json_data = "{'status':'DUPLICATE','message':'A table named: \\'%s\\' already exists.'}" % name
else :
    json_data = "{'status':'ERROR','message':'Uploading went awry'}"

utils.printPayload( json_data );
