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

status = "ok"
message = "good to go"

try :
    project_dir = "%s/data/tracks/%s%s" % \
                   (GlobalConfig.ROOT_DIR, 
                    GlobalConfig.PROJECT_PREFIX, 
                    project_name) 

    #rm directory for the explorer tree
    try :
        shutil.rmtree( project_dir )
    except Exception as e :
        message = "Project directory already removed"
    
    #rm directory for uploaded tables
    try :
        shutil.rmtree( "%s/%s" % (GlobalConfig.SRC_TABLE_DIR, project_name) )
    except Exception as e :
        pass

except OSError as e :
    status = "error"
    message = str(e)
    print str(e)


utils.printToServer( '{"status":"%s", "message":"%s"}' % (status,message) )
