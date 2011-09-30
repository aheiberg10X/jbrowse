#!/usr/bin/python
from subprocess import Popen, PIPE
import cgi
import simplejson as json
import os
import os.path
import re
import shutil
import sys
sys.path.append("../lib")
import GlobalConfig
import utils
import time

debugging = False 

def copyIfExists( source, dest ) :
    if os.path.exists( source ) :
        shutil.copy( source, dest )

utils.printToServer( 'Content-type: text/json\n\n' )
#utils.printToServer( utils.textarea_opener )

fields = cgi.FieldStorage()
query = fields.getvalue("query_box")
query_name = fields.getvalue("query_name");
donor = fields.getvalue("query_donor")
linking = fields.getvalue("query_linking");
linking = "linking"

root = GlobalConfig.ROOT_DIR

sys.stderr = open("%s/query_error.txt" % GlobalConfig.DEBUG_DIR,'w')
sys.stdout = open("%s/query_output.txt" % GlobalConfig.DEBUG_DIR,'w')
print "fields", fields
if debugging :
    query_name = "q2"
    donor = "NA18507"
    query = '''table READS (string id, string read_str, integer length, string
qvals,string chromo, integer location, char strand, string
match_descr, string mate_chromo, integer mate_loc, char mate_strand);

table genes (integer end, integer begin, string chr, string annot_id); #oposite order than what it appears

H2=select id from READS where location>0 and location < 1000

H3=select * from INTERSECT genes,H2

select * from H3 where chr=="chr1" and strand=='F' and location>0 and mate_loc<1000 and mate_loc>0 and id=="gene1"''' 
    print "query_name: %s" % query_name
    print "donor: %s" % donor
    print "query: %s" % query

query_loc = "%s/user_query.txt" % GlobalConfig.UPLOAD_DIR 
print "writing query to %s" % query_loc 

fuq = open( query_loc, 'wb')
fuq.write( query)
fuq.close()

print "popping run_biosql.sh"
t1 = time.time()
pop = Popen(['bash', "../genomequery/biosql_compiler/biosql/run_biosql.sh", \
             query_loc, \
             donor ], \
            stdin=PIPE, stdout=PIPE, stderr=PIPE)
(out, err) = pop.communicate()
sys.stdout.write( out )
sys.stderr.write( err )
t2 = time.time()
print "done with run_biosql, took: %f s" % (t2-t1)

t = '%s/genomequery/biosql_compiler/biosql/dst' % root
for thing in os.listdir(t) :
    if thing.startswith("chr") :
        chrom = thing
    else :
        continue
    source = "%s/%s" % (t,chrom)
    trackpath = GlobalConfig.TRACK_TEMPLATE % (donor, query_name, chrom)
    dest = "%s/data/%s" % (root, trackpath )
    #dest = "%s/data/tracks/chrom_%s/donor_%s/query_%s" % (root,chrom,donor,query_name)

    if not os.path.exists( dest ) :
        os.makedirs( dest )
    print "copying (change to moving!!) from %s to %s" % (source,dest)

    #copy bam
    copyIfExists( "%s/out.evidence.bam" % source, \
                  "%s/%s.bam" % (dest,query_name) )

    #copy query
    copyIfExists( query_loc, "%s/%s.gq" % (dest, query_name) )

    #copy histogram
    histogram = "%s/out.hist.txt" % source
    copyIfExists( histogram, \
                  "%s/%s.hist" % (dest, query_name) )

    #copy intervals
    copyIfExists( "%s/out.evidence.bam.short" % source, \
                  "%s/%s.intervals" % (dest,query_name) )
#
t3 = time.time()
#print "done moving, took: %f s" % (t3-t2)

print "starting bam2ncl"
pop = Popen(["./bam_to_json_paired_cgi.pl", \
             donor, \
             query_name, \
             linking], \
            stdin=PIPE, stdout=PIPE, stderr=PIPE)
(out, err) = pop.communicate()
print err
print out
t4 = time.time()
print "done with bam2ncl, took: %f s" % (t4-t3)
print "returning %s" % out
utils.printToServer( out )
#utils.printToServer( utils.textarea_closer )
#except IOError :
    #utils.printToServer( json.dumps( {"status":"error", \
                                      #"message":"io error in run query"} ) )
