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

err_filename = "%s/add_donor_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/add_donor_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
donor_name = fields.getvalue("donor_name")
project_names = fields.getvalue("project_names")
project_names = project_names.split(",")

utils.printToServer( 'Content-type: text/json\n\n' )

status, messages = "notcool",[]

upload_dir = "/home/uploader/%s"
if not os.path.exists( upload_dir ) :
    message = "No folder named: %s has been uploaded" % donor_name
else :
    shutil.move( upload_dir, GlobalConfig.DONOR_DIR )
    root = "%s/%s" % (GlobalConfig.DONOR_DIR, donor_name)
    #run script to index
    for chrom in range(1,23) + ['X','Y'] :
        bam = "%s/chr%s.bam" % (root, str(chrom) )
        mates = "%s/chr%s.mates" % (root, str(chrom) )
        indx = "%s/chr%s.mates.indx" % (root, str(chrom) )
        if os.path.exists( bam ) :
            if os.path.exists( mates ) and os.path.exists( indx ) :
                pass
            else :
                print "popping locate_mates"
                locate = "%s/indexing/locate_mates" % (GlobalConfig.BIOSQL_HOME)
                pop = Popen([locate, \
                             bam, \
                             '>', \
                             mates], \
                            stdin=PIPE, stdout=PIPE, stderr=PIPE)

                (out, err) = pop.communicate()
                sys.stdout.write( out )
                sys.stderr.write( err )
                
                print "popping index_mates"
                index = "%s/indexing/index_mates" % (GlobalConfig.BIOSQL_HOME)
                pop = Popen([index, \
                             mates, \
                             bam, \
                             indx], \
                            stdin=PIPE, stdout=PIPE, stderr=PIPE)

                (out, err) = pop.communicate()
                sys.stdout.write( out )
                sys.stderr.write( err )

        else :
            messages.append( "chr%s.bam doesn't exist!" % str(chrom) )

    for pn in project_names :
        os.mkdir( "%s/tracks/%s%s/%s%s" % ( GlobalConfig.DATA_DIR, \
                                            GlobalConfig.PROJECT_PREFIX, \
                                            pn, \
                                            GlobalConfig.DONOR_PREFIX, \
                                            donor_name ) )

    status, message = "ok", "Donor genome has finished indexing."


message = "\n".join( messages )
r = '{"status":"%s","message":%s}' % (status,message)
utils.printToServer( r )
