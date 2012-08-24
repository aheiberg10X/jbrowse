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

err_filename = "%s/create_new_project_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/create_new_project_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
user_name = fields.getvalue("user_name")
project_name = fields.getvalue("project_name")
assembly = fields.getvalue("assembly")

utils.printToServer( 'Content-type: text/json\n\n' )
project_dir = "%s/data/tracks/%s%s" % \
               (GlobalConfig.ROOT_DIR, 
                GlobalConfig.PROJECT_PREFIX, 
                project_name) 
src_table_dir = "%s/%s" % (GlobalConfig.SRC_TABLE_DIR, project_name)
#setup directory for the explorer tree to find
if os.path.exists( project_dir ) or os.path.exists( src_table_dir ) :
    print "project_dir", project_dir
    #TODO:
    #name comflicts between users?
    status,message = "bad","The project name '%s' is already taken" % project_name
else :
    os.mkdir( project_dir )
    #setup directory for uploaded tables
    os.mkdir( src_table_dir )

    #add the new project:assembly mapping to file
    pamfile = "../lib/project_assembly_mapping.json"
    jpam = utils.fileToJson( pamfile )
    jpam[project_name] = assembly
    utils.jsonToFile( jpam, pamfile )
    #lines = fmap.readlines()
    #lines[-1] = "%s : %s" % (project_name,assembly)
    #lines.append('}')
    #fmap.close()

    #add permissions
    perm_file = "../lib/permissions.json"
    jperms = utils.fileToJson( perm_file )
    jperms[user_name][project_name] = True
    utils.jsonToFile( jperms, perm_file )

    status = "ok"
    message = "good to go"
    print "made the directory"



utils.printToServer( '{"status":"%s", "message":"%s"}' % (status,message) )
