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

debugging = False

fields = cgi.FieldStorage()

utils.printToServer( 'Content-type: text/html\n\n' )

def validate( some_stuff ) :
    return (True,"good to go")

fileitem = fields["interval_table"]
if fileitem.filename :
    handle = fileitem.file
    stuff = handle.read()
    (ok,message) = validate(stuff)
    if not ok :
        json_data = "{'status':'ERROR', 'message':'%s'}" % message
    else :
        fn = os.path.basename(fileitem.filename)
        path = "%s/genomequery/biosql_compiler/biosql/dst" % GlobalConfig.ROOT_DIR
        newfilename = "%s/%s" % (path,fn)
        open(newfilename, 'w').write( stuff )
        #update tables.txt, rebuild parsed tables

        json_data = "{'status':'OK','message':'Uploaded!'}"
else :
    json_data = "{'status':'ERROR','message':'Uploading went awry'}"

utils.printPayload( json_data );
