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
import os.path

use = re.compile(r'\s*use\s+(.*)\s*;')

debugging = False 

def copyIfExists( source, dest ) :
    if os.path.exists( source ) :
        shutil.copy( source, dest )
    else :
        print "\n%s doesn't exist!\n" % source

#TODO: take the use _____ statements and
#fill in with the path to that file (is this what the compiler will need?)
def expandUses( query, donor_name ) :
    path = "%s/data/tracks/%s%s/interval_tables" % \
                (GlobalConfig.ROOT_DIR, \
                 GlobalConfig.DONOR_PREFIX, \
                 donor_name )
    statements = query.split('\n')
    for i in range(len(statements)) :
        #other use caveats?
        if 'parsed_tables' not in statements[i].lower() and \
            use.match(statements[i]) :
            usepath = use.sub( r'%s/\1.it' % path, statements[i] )
            if os.path.exists( usepath ) :
                statements[i] = "use %s;" % usepath
            else :
                return (False, "No such table '%s'" % usepath)

    return (True, "\n".join( statements ))

utils.printToServer( 'Content-type: text/json\n\n' )
#utils.printToServer( utils.textarea_opener )

if debugging :
    query = '''use parsed_tables;
    import READS;
    use genes;

    H1=select count from READS mates(location,mate_loc) where location>=0 and mate_loc>=0 and ((mate_loc-location>1000 and mate_loc-location<2000000) or (location-mate_loc>1000 and location-mate_loc<2000000))

    select * from H1 where countvec>5'''

    print expandUses( query, 'NA18507' )[1]


fields = cgi.FieldStorage()
query_name = fields.getvalue("query_name")
donor = fields.getvalue("query_donor")
chromnum = fields.getvalue("query_chrom")
(status, query) = expandUses( fields.getvalue("query_box"), donor )
linking = fields.getvalue("query_linking")
linking = "linking";
root = GlobalConfig.ROOT_DIR

err_filename = "%s/query_error_%s.txt" % (GlobalConfig.DEBUG_DIR,chromnum)
sys.stderr = open( err_filename,'w')
out_filename = "%s/query_output_%s.txt" % (GlobalConfig.DEBUG_DIR,chromnum)
sys.stdout = open( out_filename,'w')
print "fields", fields

query_loc = "%s/user_query.txt" % GlobalConfig.UPLOAD_DIR
print "writing query to %s" % query_loc

fuq = open( query_loc, 'wb')
fuq.write( query)
fuq.close()

print "popping run_biosql.sh"
t1 = time.time()
#chrom is 1..22 X Y
pop = Popen(['bash', "../genomequery/biosql_compiler/biosql/run_biosql.sh", \
             query_loc, \
             donor, \
             chromnum], \
            stdin=PIPE, stdout=PIPE, stderr=PIPE)
(out, err) = pop.communicate()
sys.stdout.write( out )
sys.stderr.write( err )
t2 = time.time()
print "done with run_biosql, took: %f s" % (t2-t1)

chrom = "chr%s" % chromnum
source = '%s/genomequery/biosql_compiler/biosql/dst/%s' % (root,chrom)
trackpath = GlobalConfig.TRACK_TEMPLATE % (donor, query_name, chrom)
dest = "%s/data/%s" % (root, trackpath )
#dest = "%s/data/tracks/chrom_%s/donor_%s/query_%s" % (root,chrom,donor,query_name)

if not os.path.exists( dest ) :
    os.makedirs( dest )
print "copying (change to moving!!) from %s to %s" % (source,dest)

#copy bam
copyIfExists( "%s/out.bam" % source, \
              "%s/%s_%d.bam" % (dest,chromnum,query_name) )

#copy query
copyIfExists( query_loc, "%s/%s_%d.gq" % (dest, chromnum, query_name) )

#copy histogram
histogram = "%s/out.hist" % source
copyIfExists( histogram, \
              "%s/%s_%d.hist" % (dest, chromnum,  query_name) )

#copy intervals
copyIfExists( "%s/out.bam.short" % source, \
              "%s/%s_%d.intervals" % (dest,chromnum,query_name) )
#
t3 = time.time()
#print "done moving, took: %f s" % (t3-t2)

print "starting bam2ncl"
pop = Popen(["./bam_to_json_paired_cgi.pl", \
             donor, \
             chromnum, \
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

