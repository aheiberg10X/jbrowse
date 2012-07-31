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

err_filename = "%s/delete_project_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/delete_project_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
project_name = fields.getvalue("project_name")

utils.printToServer( 'Content-type: text/json\n\n' )
try :
    project_dir = "%s/data/tracks/%s%s" % \
                   (GlobalConfig.ROOT_DIR, 
                    GlobalConfig.PROJECT_PREFIX, 
                    project_name) 

    #setup directory for the explorer tree to find
    shutil.rmtree( project_dir )
    
    #setup directory for uploaded tables
    shutil.rmtree( "%s/%s" % (GlobalConfig.SRC_TABLE_DIR, project_name) )

    #TODO:
    # remove the new project:assembly mapping to file
    #fmap = open("../lib/project_assembly_mapping.json")
    #lines = fmap.readlines()
    #lines[-1] = "%s : %s" % (project_name,assembly)
    #lines.append('}')
    #fmap.close()

    status = "ok"
    message = "good to go"

except OSError as e :
    status = "error"
    message = str(e)
    print str(e)


utils.printToServer( '{"status":"%s", "message":"%s"}' % (status,message) )
