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

err_filename = "%s/index_donor_error.txt" % (GlobalConfig.DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/index_donor_output.txt" % (GlobalConfig.DEBUG_DIR)
sys.stdout = open( out_filename,'w')

fields = cgi.FieldStorage()
user_name = fields.getvalue("user_name")
donor_name = fields.getvalue("donor_name")
#TODO: get user input
assembly = "hg18"
#project_names = fields.getvalue("project_names")
#project_names = project_names.split(",")

utils.printToServer( 'Content-type: text/json\n\n' )

status, messages = "notcool",[]

upload_dir = "/home/uploader/%s/%s" % (user_name, donor_name)
#if not os.path.exists( upload_dir ) :
    #messages.append( "No folder named: %s exists" % upload_dir )
#else :
root = "%s/%s/%s" % (GlobalConfig.DONOR_DIR, user_name, donor_name)
#shutil.move( upload_dir, root )
#run script to index
for chrom in range(1,23) + ['X','Y'] :
    bam = "%s/chr%s.bam" % (root, str(chrom) )
    mates = "%s/chr%s.mates" % (root, str(chrom) )
    indx = "%s/chr%s.mates.indx" % (root, str(chrom) )
    if os.path.exists( bam ) :
        if not os.path.exists( indx ) :
            print "popping index_all.sh"
            pop = Popen(["bash",\
                         "index_all.sh", \
                         "%s/chr" % root, \
                         assembly], \
                    stdin=PIPE, stdout=PIPE, stderr=PIPE )
            (out, err) = pop.communicate()
            #messages.append( "%s : %s" % (str(chrom),out) )
            sys.stdout.write( out )
            sys.stderr.write( err )
        else :
            messages.append( "%s already exists" % indx )


        #if not os.path.exists( mates ) :
            #print "%s popping locate_mates" % str(chrom)
            #locate = "%s/indexing/locate_mates" % (GlobalConfig.BIOSQL_HOME)
            #fmates = open(mates,'w')
            #pop = Popen([locate, \
                         #bam], \
                        #stdin=PIPE, stdout=fmates, stderr=PIPE)
##
            #(out, err) = pop.communicate()
            #fmates.close()
            #sys.stderr.write( err )
       # 
        #if not os.path.exists( indx ) :
            #print "%s popping index_mates" % str(chrom)
            #index = "%s/indexing/index_mates" % (GlobalConfig.BIOSQL_HOME)
            #pop = Popen([index, \
                         #mates, \
                         #bam, \
                         #indx, \
                         #"chr%s" % str(chrom), \
                         ####], \
                        #stdin=PIPE, stdout=PIPE, stderr=PIPE)
##
            #(out, err) = pop.communicate()
            #sys.stdout.write( out )
            #sys.stderr.write( err )

    else :
        messages.append( "chr%s.bam doesn't exist!" % str(chrom) )

    status, message = "ok", "Donor genome has finished indexing."


message = ";".join( messages )
r = '{"status":"%s","message":"%s"}' % (status,message)
utils.printToServer( r )
