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

err_filename = "%s/list_user_donors_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/list_user_donors_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
user_name = fields.getvalue("user_name")

utils.printToServer( 'Content-type: text/json\n\n' )

user_donor_dir = "%s/%s" % (GlobalConfig.DONOR_DIR, user_name)
donors = []
status, message = "bad","generic"

if os.path.exists( user_donor_dir ) :
    for donor in os.listdir( user_donor_dir ) :
        donors.append( donor )

    if len(donors) == 0 :
        status = "empty"
        message = "No donors"
    else :
        status = "ok"
        message = json.dumps( donors )

else :
    status = "User donor dir '%s' does not exist" % user_donor_dir

print donors
r = '{"status":"%s","message":"%s"}' % (status,message)
utils.printToServer( r )
