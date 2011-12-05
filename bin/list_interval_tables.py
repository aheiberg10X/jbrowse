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

err_filename = "%s/list_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/list_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
donor_name = fields.getvalue("donor_name")

utils.printToServer( 'Content-type: text/json\n\n' )
path = "%s/data/tracks/%s%s/interval_tables" % \
            (GlobalConfig.ROOT_DIR, \
             GlobalConfig.DONOR_PREFIX, \
             donor_name)
print path
interval_tables = []
if os.path.exists( path ) :
    for listing in os.listdir( path ) :
        print listing
        viewable =  listing.strip("%s_" % donor_name).strip('.it')
        interval_tables.append( viewable )
    message = json.dumps(interval_tables)
    status = '"OK"'
else :
    status = '"EMPTY"'
    message = '"No tables loaded"'

print message
utils.printToServer( '{"status": %s, "message": %s }' % (status,message) )
