#!/usr/bin/python
from subprocess import Popen, PIPE
import cgi
import simplejson as json
import os
import sys
print 'Content-type: text/json\n\n'


fields = cgi.FieldStorage()
query = fields.getvalue("query_box")
query_bam = fields.getvalue("query_bam")

sys.stderr = open("/home/andrew/school/dnavis/jbrowse/bin/debugging/sample_error.txt",'w')
sys.stdout = open("/home/andrew/school/dnavis/jbrowse/bin/debugging/sample_output.txt",'w')
print "query_bam: %s" % query_bam

if not os.path.isfile( query_bam ) :
    print json.dumps( {"status":"error",
                       "message":"%s is not a valid file" % query_bam} )
else :
    uq = "/home/andrew/school/dnavis/jbrowse/bin/debugging/user_query.txt"
    fuq = open( uq, 'wb')
    fuq.write( query)
    fuq.close()

    indx_file = "%s.mates.indx" % query_bam

    #pop = Popen(["../genomequery/biosql_compiler/biosql/sample.sh"], \
                 #stdin=PIPE, stdout=PIPE, stderr=PIPE)

    # obvi don't use the shit query you are typing in that also has not indx...
    uq = "/home/andrew/school/dnavis/jbrowse/genomequery/biosql_compiler/query.txt"
    pop = Popen(["../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                 uq, \
                 query_bam, \
                 "%s.mates.indx" % query_bam ], \
                stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = pop.communicate()
    print "out: %s \n\nerr: %s" % (out,err)

    sys.stdout = sys.__stdout__
    print json.dumps( {"status":"ok",
                       "message":out} )
