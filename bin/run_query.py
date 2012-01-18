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

def moveIfExists( source, dest ) :
    if os.path.exists( source ) :
        shutil.move( source, dest )
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
            usepath = use.sub( r'%s/%s_\1.it' % (path,donor_name), statements[i] )
            if os.path.exists( usepath ) :
                statements[i] = "use %s;" % usepath
            else :
                return (False, "No such table \\'%s\\'" % statements[i])

    return (True, "\n".join( statements ))

utils.printToServer( 'Content-type: text/json\n\n' )
#utils.printToServer( utils.textarea_opener )

if debugging :
    query = '''use parsed_tables;
    import READS;
    use genes222;

    H1=select count from READS mates(location,mate_loc) where location>=0 and mate_loc>=0 and ((mate_loc-location>1000 and mate_loc-location<2000000) or (location-mate_loc>1000 and location-mate_loc<2000000))

    select * from H1 where countvec>5'''

    print expandUses( query, 'NA18507' )[1]


fields = cgi.FieldStorage()
query_name = fields.getvalue("query_name")
donor = fields.getvalue("query_donor")
chromnum = fields.getvalue("query_chrom")
project = fields.getvalue("query_project");

err_filename = "%s/query_error_%s.txt" % (GlobalConfig.DEBUG_DIR,chromnum)
sys.stderr = open( err_filename,'w')
out_filename = "%s/query_output_%s.txt" % (GlobalConfig.DEBUG_DIR,chromnum)
sys.stdout = open( out_filename,'w')
print "fields", fields

(status, query) = expandUses( fields.getvalue("query_box"), donor )
print query, "\n"
if not status :
    utils.printToServer( "{'status':'ERROR', 'message': '%s'}" % query )
else :
    linking = fields.getvalue("query_linking")
    linking = "linking";
    root = GlobalConfig.ROOT_DIR


    query_loc = "%s/user_query.txt" % GlobalConfig.UPLOAD_DIR
    print "writing query to %s" % query_loc

    fuq = open( query_loc, 'wb')
    fuq.write( query)
    fuq.close()

    print "popping run_biosql.sh"
    t1 = time.time()
    #chrom is 1..22 X Y
    src_table_dir = "%s/data/tracks/%s_%s/interval_tables" \
                    % (root, GlobalConfig.DONOR_PREFIX, donor)
    
    #leaving space for donor and chrom
    trackpath = GlobalConfig.TRACK_TEMPLATE % \
                (project, "%s", query_name, "%s")
    
    #trackpath = GlobalConfig.TRACK_TEMPLATE % \
                #(project, donor, query_name, chrom)
    product_dest = "%s/data/%s" % (root, trackpath )
    #dest = "%s/data/%s" % (root, trackpath)


    pop = Popen(['bash', \
                 "../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                 query_loc, \
                 product_dest, \
                 src_table_dir], \
                stdin=PIPE, stdout=PIPE, stderr=PIPE)

    #pop = Popen(['bash', \
                 #"../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                 #query_loc, \
                 #donor, \
                 #chromnum, \
                 #src_table_dir], \
                #stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = pop.communicate()
    sys.stdout.write( out )
    sys.stderr.write( err )
    t2 = time.time()
    print "done with run_biosql, took: %f s" % (t2-t1)

    #check bytecode for syntax error
    #get genome list from bytecode

    fbyte = open("../genomequery/biosql_compiler/biosql/bytecode.txt")
    first_line = fbyte.readline().lower()
    fbyte.close()
    if "syntax error" in first_line :
        message = first_line.replace(':',';')
        t = json.dumps({'status':'error','message':first_line})
        utils.printToServer( t )
        assert 1==9

    for donor in donors :
        for chromnum in range(1,23) + ['X','Y'] :
            chrom = "chr%s" % str(chromnum)
            prefix = product_dest % (donor, chrom)
            if not os.path.exists( dest ) :
                os.makedirs( dest )

            #source = '%s/genomequery/biosql_compiler/biosql/dst/%s' % (root,chrom)

            #copy bam
            moveIfExists( "%s/out.bam" % prefix, \
                          "%s/%s_%s.bam" % (prefix, query_name, chromnum) )

            #copy query
            #put the gq file in the query_ dir
            moveIfExists( query_loc, \
                          "%s/../%s.gq" % (prefix, query_name) )

            #copy histogram
            moveIfExists( "%s/out.hist" % prefix, \
                          "%s/%s_%s.hist" % (prefix, query_name, chromnum) )

            #copy intervals
            moveIfExists( "%s/out.bam.short" % prefix, \
                          "%s/%s_%s.intervals" % (prefix, query_name, chromnum) )
            #
            t3 = time.time()
            #print "done moving, took: %f s" % (t3-t2)

            print "starting bam2ncl"
            pop = Popen(["./bam2ncl.pl", \
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

