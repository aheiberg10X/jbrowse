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
from subprocess import Popen, PIPE

err_filename = "%s/upload_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/upload_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()

utils.printToServer( 'Content-type: text/html\n\n' )

#TODO: ownership and permission for inteval files
def validate( some_stuff, project_name ) :
    #how to check permissions to 
    return (True,"good to go")


fileitem = fields["interval_table"]
project_name = fields.getvalue("project_name")
if fileitem.filename :
    handle = fileitem.file
    stuff = handle.read()
    (ok,message) = validate(stuff, project_name)
    if not ok :
        json_data = "{'status':'ERROR', 'message':'%s'}" % message
    else :
        fn = os.path.basename(fileitem.filename)
        (name,ext) = os.path.splitext(fn)
        path = "%s/data/tracks/%s%s/interval_tables" % \
                    (GlobalConfig.ROOT_DIR, \
                     GlobalConfig.PROJECT_PREFIX, \
                     project_name )
        if not os.path.exists( path ) :
            os.mkdir( path )

        newfilename = "%s/%s_%s.it" % (path,project_name,name)
        if not os.path.exists( newfilename ) :
            open(newfilename, 'w').write( stuff )
            #update tables.txt, rebuild parsed tables

            path = "%s/genomequery/biosql_compiler/biosql" % GlobalConfig.ROOT_DIR
            ftables = "%s/tables.txt" % path
            ftables = open( ftables, 'a' )
            fullname = "%s_%s" % (project_name,name)
            schema = "table %s (string annot_id, string chr, char ornt, integer begin, integer end);\n" % fullname
            ftables.write( schema )
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
