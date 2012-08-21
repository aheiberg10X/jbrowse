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
print fields
user_name = fields.getvalue("user_name")
donor_name = fields.getvalue("donor_name")
refinfo_file = fields["refinfo_file"]

user_upload_root = "%s/%s" % (GlobalConfig.DONOR_DIR, user_name)

if refinfo_file.filename :
    splt = refinfo_file.filename.rsplit('.',1)
    if len(splt) > 1 :
        refinfo_name = splt[0]
    else :
        refinfo_name = refinfo_file.filename
    os.mkdir( "%s/%s" % (user_upload_root, refinfo_name) )
    fout = open("%s/%s/%s" % (user_upload_root, refinfo_name, refinfo_file.filename), 'w' )    
    handle = refinfo_file.file
    lines = handle.read()
    fout.write( lines )
    fout.close()
    refinfo = refinfo_name
else :
    refinfo = fields.getvalue("refinfo")

#project_names = fields.getvalue("project_names")
#project_names = project_names.split(",")

utils.printToServer( 'Content-type: text/html\n\n' )

status, messages = "notcool",[]

upload_dir = "/home/uploader/%s/%s" % (user_name, donor_name)
#if not os.path.exists( upload_dir ) :
    #messages.append( "No folder named: %s exists" % upload_dir )
#else :
root = "%s/%s/%s" %  (user_upload_root, refinfo, donor_name)
#shutil.move( upload_dir, root )

#TODO
#index this using the ref file (maybe not all chroms are there, or are named differently)
#also, run_biosql will need to be changed
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
                         refinfo], \
                    stdin=PIPE, stdout=PIPE, stderr=PIPE )
            (out, err) = pop.communicate()
            #messages.append( "%s : %s" % (str(chrom),out) )
            sys.stdout.write( out )
            sys.stderr.write( err )
        else :
            messages.append( "%s already exists" % indx )

    else :
        messages.append( "chr%s.bam doesn't exist!" % str(chrom) )

    status, message = "ok", "Donor genome has finished indexing."


message = ";".join( messages )
r = '{"status":"%s","message":"%s"}' % (status,message)
utils.printPayload( r )
