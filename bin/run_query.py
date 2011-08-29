#!/usr/bin/python
from subprocess import Popen, PIPE
import cgi
import simplejson as json
import os
import re
import shutil
import sys
sys.path.append("../lib")
import GlobalConfig
import utils

print 'Content-type: text/json\n\n'


fields = cgi.FieldStorage()
query = fields.getvalue("query_box")
query_name = fields.getvalue("query_name");
donor_name = fields.getvalue("query_donor_bam")
host_chrom = fields.getvalue("host_chrom");

#jconfig = utils.fileToJson( "../lib/GlobalConfig.js" )
root = GlobalConfig.ROOT_DIR

#sys.stderr = open("%s/bin/debugging/query_error.txt" % root,'w')
#sys.stdout = open("%s/bin/debugging/query_output.txt" % root,'w')

if False : #not os.path.isfile( query_donor_bam ) :
    print 'heh'
    print json.dumps( {"status":"error",
                       "message":"something's null"} )
else :
    try :
        query_name = "fudgsicle"
        donor_name = "NA18507"
        print 'wut'
        uq = "%s/bin/debugging/user_query.txt" % root
        #fuq = open( uq, 'wb')
        #fuq.write( query)
        #fuq.close()
    #
        #indx_file = "%s.mates.indx" % query_donor_bam
    #
        ## obvi don't use the shit query you are typing in that also has not indx...
        ##uq = "%s/genomequery/biosql_compiler/query_amended.txt" % root
        #pop = Popen(["../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                     #uq, \
                     #query_donor_bam, \
                     #"%s.mates.indx" % query_donor_bam ], \
                    #stdin=PIPE, stdout=PIPE, stderr=PIPE)
        #(out, err) = pop.communicate()
    #
        #print "RUN_BIOSQL.sh\n##########################################\n\n"
        #print "out: %s \n\nerr: %s" % (out,err)
    #
        t = '%s/genomequery/biosql_compiler/biosql/dst' % root
        print t
        for thing in os.listdir(t) :
            if thing.startswith("chr") :
                chrom = thing
            else :
                continue
            source = "%s/%s" % (t,chrom)
            dest = "%s/data/tracks/chrom_%s/donor_%s/query_%s" % (root,chrom,donor_name,query_name)
            if not os.path.exists( dest ) :
                os.makedirs( dest )
            #shutil.copy( src,dest )
            print "src,dst: " , source, dest
            #copy bam
            shutil.copy( "%s/out.evidence.bam" % source, \
                         "%s/%s.bam" % (dest,query_name) )
            #copy query
            shutil.copy( uq, "%s/%s.gq" % (dest, query_name) )
            #copy histogram
            histogram = "%s/out.hist.txt" % source 
            shutil.copy( histogram, "%s/%s.hist" % (dest, query_name) )
            #copy intervals
            shutil.copy( "%s/out.evidence.bam.short" % source, \
                         "%s/%s.intervals" % (dest,query_name) )

        #pop = Popen(["./bam_to_json_paired_cgi.pl", \
                     #chrom, \
                     #donor, \
                     #query_name, \
                     #"linking"], \
                    #stdin=PIPE, stdout=PIPE, stderr=PIPE)
        #(out, err) = pop.communicate()
        #print "\nout %s \n\nerr: %s" % (out,err)


        #sys.stdout = sys.__stdout__
        #utils.printToServer( json.dumps( {"status":"ok",
                                          #"message":out} ) )

    except IOError as e:
        utils.printToServer( json.dumps( {"status":"error", \
                                          "message":str(e)} ) )
