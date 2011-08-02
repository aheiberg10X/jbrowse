#!/usr/bin/python
from subprocess import Popen, PIPE
import cgi
import simplejson as json
import os
import sys
import re
import shutil

import utils
print 'Content-type: text/json\n\n'


fields = cgi.FieldStorage()
query = fields.getvalue("query_box")
query_name = fields.getvalue("query_name");
query_bam = fields.getvalue("query_bam")
host_chrom = fields.getvalue("host_chrom");

jconfig = utils.fileToJson( "../lib/GlobalConfig.js" )
root = jconfig["root_dir"]

sys.stderr = open("%s/bin/debugging/query_error.txt" % root,'w')
sys.stdout = open("%s/bin/debugging/query_output.txt" % root,'w')
print fields
print "query_bam: %s" % query_bam
print "query: %s" % query

if not os.path.isfile( query_bam ) :
    print json.dumps( {"status":"error",
                       "message":"%s is not a valid file" % query_bam} )
else :
    uq = "%s/bin/debugging/user_query.txt" % root
    fuq = open( uq, 'wb')
    fuq.write( query)
    fuq.close()

    indx_file = "%s.mates.indx" % query_bam

    # obvi don't use the shit query you are typing in that also has not indx...
    #uq = "%s/genomequery/biosql_compiler/query_amended.txt" % root
    pop = Popen(["../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                 uq, \
                 query_bam, \
                 "%s.mates.indx" % query_bam ], \
                stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = pop.communicate()
    
    print "RUN_BIOSQL.sh\n##########################################\n\n"
    print "out: %s \n\nerr: %s" % (out,err)

    #rename bam file
    t = '%s/genomequery/biosql_compiler/biosql/dst' % root
    renamed_bam = "%s/%s.bam" % (t,query_name)
    shutil.move( "%s/out.evidence.bam" % t, \
                 renamed_bam )
    histogram = "%s/out.hist.txt" % t
    pop = Popen(["./bam_to_json_paired_cgi.pl", \
                 renamed_bam, \
                 host_chrom, \
                 "linking", \
                 histogram], \
                stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = pop.communicate()
    print '''\n\n#######################################
             BAM_TO_JSON_PAIRED_CGI\n##################################\n\n'''
    print "out %s \n\nerr: %s" % (out,err)
    shutil.move( renamed_bam, "%s/data/tracks/%s/query_%s/%s.bam" % (root, host_chrom, query_name, query_name) )
    shutil.move( uq, "%s/data/tracks/%s/query_%s/%s.gq" % (root, host_chrom, query_name, query_name) )
    sys.stdout = sys.__stdout__
    utils.printToServer( json.dumps( {"status":"ok",
                                      "message":out} ) )
