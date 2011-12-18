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

err_filename = "%s/list_donor_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/list_donor_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
project_name = fields.getvalue("project_name")

utils.printToServer( 'Content-type: text/json\n\n' )

project_dir = "%s/data/tracks/%s%s/" \
        % (GlobalConfig.ROOT_DIR, GlobalConfig.PROJECT_PREFIX, project_name)

donors = []
for donor in os.listdir( project_dir ) :
    if donor.startswith( GlobalConfig.DONOR_PREFIX ) :
        donors.append( donor.strip( GlobalConfig.DONOR_PREFIX ) )

if len(donors) == 0 :
    status = "empty"
    message = "\"No genomes associated with this project\""
else :
    status = "ok"
    message = json.dumps( donors )

print donors
r = '{"status":"%s","message":%s}' % (status,message)
utils.printToServer( r )
