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

test = open("derpderp.txt",'w')
test.write("derp")
fields = cgi.FieldStorage()
test.write( str(fields) )
test.write( "hehe" )

utils.printToServer( 'Content-type: text/json\n\n' )

fileitem = fields["interval_table"]
if fileitem.filename :
    fn = os.path.basename(fileitem.filename)
    open(fn, 'w').write( fileitem.file.read() )
    utils.printToServer("{'status':'OK','message':'Uploaded!'}")
else :
    utils.printToServer("{'status':'ERROR','message':'Uploading went awry'}")

test.close()
