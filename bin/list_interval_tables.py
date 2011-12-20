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

err_filename = "%s/list_tables_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/list_tables_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
project_name = fields.getvalue("project_name")
print project_name
utils.printToServer( 'Content-type: text/json\n\n' )
path = "%s/data/tracks/%s%s/interval_tables" % \
            (GlobalConfig.ROOT_DIR, \
             GlobalConfig.PROJECT_PREFIX, \
             project_name)
print path
interval_tables = []
if os.path.exists( path ) :
    for listing in os.listdir( path ) :
        print listing
        if listing.find( project_name ) == 0 :
            viewable = listing[len(project_name)+1:].rsplit('.',1)[0]
        else :
            viewable = "## %s ##" % listing
        #viewable = listing.split('_',1)[1].rsplit('.',1)[0]
        #viewable =  listing.strip("%s_" % project_name).strip('.it')
        print viewable
        interval_tables.append( viewable )
    message = json.dumps(interval_tables)
    status = "ok"
else :
    status = "empty"
    message = '"No tables loaded"'

print message
utils.printToServer( '{"status": "%s", "message": %s }' % (status,message) )
