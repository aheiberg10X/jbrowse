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
import hashlib
from subprocess import Popen, PIPE

err_filename = "%s/login_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/login_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
user_name = fields.getvalue("user_name")
passwd = fields.getvalue("passwd")
is_registering = fields.getvalue("is_registering")

utils.printToServer( 'Content-type: text/json\n\n' )
status, message = "ok", "ok"

print fields

if is_registering == 'true' :
    #add permissions
    passwd_file = "../lib/passwds.json"
    perm_file = "../lib/permissions.json"
    jperms = utils.fileToJson( perm_file )
    jpasswds = utils.fileToJson( passwd_file )
    if user_name in jpasswds :
        status,message = "bad","A user by the name '%s' already exists" % user_name
    else :
        m = hashlib.md5()
        m.update(passwd)
        jpasswds[user_name] = m.hexdigest()
        utils.jsonToFile( jpasswds, passwd_file )

        jperms[user_name] = {}
        utils.jsonToFile( jperms, perm_file )

        #user dir in DONOR_DIR
        os.mkdir( "%s/%s" % (GlobalConfig.DONOR_DIR, user_name) )

        #user dir in uploader
        os.mkdir( "/home/uploader/%s" % user_name)
else :
    bad_combo = "Bad user/pass combo"

    print "logging in..."
    jpasswds = utils.fileToJson( "../lib/passwds.json" )
    if user_name in jpasswds :
        m = hashlib.md5()
        m.update( passwd )
        hashed_pass = m.hexdigest()
        print "hashed_pass", hashed_pass
        print "japasswd", jpasswds[user_name]
        if hashed_pass == jpasswds[user_name] :
            pass
        else:
            status,message = "bad",bad_combo
    else :
        status,message = "bad",bad_combo



r = '{"status":"%s","message":"%s"}' % (status,message)
utils.printToServer( r )
