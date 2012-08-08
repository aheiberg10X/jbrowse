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

err_filename = "%s/add_user_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/add_user_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
user_name = fields.getvalue("user_name")
passwd = fields.getvalue("passwd")

utils.printToServer( 'Content-type: text/json\n\n' )
status, message = "bad", "work in progress"

#TODO: create lib/permissions.json
#update lib/permissions.json
#create user dir in donors to accept uploads, have to have www-data chmod it to 777 to allow uploader writing rights?
#create user dir in DONOR_DIR

r = '{"status":"%s","message":%s}' % (status,message)
utils.printToServer( r )
