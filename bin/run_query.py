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
from GlobalConfig import ROOT_DIR, DATA_DIR, TRACK_TEMPLATE, PROJECT_PREFIX, DEBUG_DIR, UPLOAD_DIR, UNBOUND_CHROM
import utils
import time
import os.path

importre = re.compile(r'\s*import\s+(.*)\s*;')

debugging = False 

def moveIfExists( source, dest ) :
    if os.path.exists( source ) :
        shutil.move( source, dest )
    else :
        print "\n%s doesn't exist!\n" % source

#TODO: take the use _____ statements and
#fill in with the path to that file (is this what the compiler will need?)
def expandImports( query, project_name ) :
    
    path = "%s/data/tracks/%s%s/interval_tables" % \
                (ROOT_DIR, PROJECT_PREFIX, project_name )
    statements = query.split('\n')
    for i in range(len(statements)) :
        print statements[i]
        if 'reads' not in statements[i].lower() and \
            importre.match(statements[i]) :
            importname = importre.sub( r'%s_\1' % (project_name), statements[i] )
            importpath = "%s/%s.txt" % (path, importname)
            print importpath
            if os.path.exists( importpath ) :
                statements[i] = "import %s;" % importname
            else :
                return (False, "No such table \\'%s\\'" % statements[i])

    return (True, "\n".join( statements ))

def validateGenomes( query, perms ) :
    pass

utils.printToServer( 'Content-type: text/json\n\n' )
#utils.printToServer( utils.textarea_opener )

if debugging :
    query_name = 'debug'
    query = '''use parsed_tables
genome NA18507;
import READS;
import genes;

H1=select interval_creation() from READS using intervals(location,
                                                         mate_loc, both_mates) where location>=0 and mate_loc>=0 and
((mate_loc-location>1000 and mate_loc-location<2000000) or
 (location-mate_loc>1000 and location-mate_loc<2000000))

select * from H1 where interval_coverage>5'''
    project = 'main'

else :
    fields = cgi.FieldStorage()
    query_name = fields.getvalue("query_name")
    query = fields.getvalue("query_box")
    #donor = fields.getvalue("query_donor")
    #chromnum = fields.getvalue("query_chrom")
    project = fields.getvalue("query_project");
    assembly = fields.getvalue("assembly");

err_filename = "%s/query_error.txt" % (DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/query_output.txt" % (DEBUG_DIR)
sys.stdout = open( out_filename,'w')
print "fields", fields

(status, query) = expandImports( query, project )
print "\n query", query, "\n"
if not status :
    utils.printToServer( "{'status':'ERROR', 'message': '%s'}" % query )
else :
    #linking = fields.getvalue("query_linking")
    linking = "linking";
    root = ROOT_DIR

    query_loc = "%s/user_query.txt" % UPLOAD_DIR
    print "writing query to %s" % query_loc

    fuq = open( query_loc, 'wb')
    fuq.write( query)
    fuq.close()

    print "popping run_biosql.sh"
    t1 = time.time()
    #chrom is 1..22 X Y
    src_table_dir = "%s/data/tracks/%s%s/interval_tables" \
                    % (root, PROJECT_PREFIX, project)
    
    #leaving space for donor and chrom
    trackpath = TRACK_TEMPLATE % (project, "%s", query_name, "%s")
    
    #trackpath = GlobalConfig.TRACK_TEMPLATE % \
                #(project, donor, query_name, chrom)
    product_dest = "%s/data/%s" % (root, trackpath )
    #dest = "%s/data/%s" % (root, trackpath)

    print query_loc, type(query_loc)
    print product_dest, type(product_dest)
    print src_table_dir, type(src_table_dir)
    print assembly, type(assembly)

    pop = Popen(['bash', \
                 "../genomequery/biosql_compiler/biosql/run_biosql.sh", \
                 query_loc, \
                 product_dest, \
                 src_table_dir,
                 assembly], \
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
    if "syntax error" in first_line :
        message = first_line.replace(':',';')
        t = json.dumps({'status':'error','message':first_line})
        fbyte.close()
        utils.printToServer( t )
        assert 1==9
    else :
        donors = []
        for line in fbyte.readlines() :
            if "genome" in line :
                donors.append( line.split()[2] )

    fbyte.close()

    track_datas = []
    messages = []
    for donor in donors :
        for chromnum in [str(x) for x in range(1,23) + ['X','Y']] :
            chrom = "chr%s" % str(chromnum)
            prefix = product_dest % (donor, chrom)

            #if not os.path.exists( prefix ) :
                #t = json.dumps({'status':'error','message':'Trouble creating the necessary folders'})
                #utils.printToServer( t )

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
            print "donor", donor, type(donor)
            print "chromnum", chromnum, type(chromnum)
            print "query_name", query_name, type(query_name)
            print "linking", linking, type(linking)
            print "assembly ", assembly

            pop = Popen(["perl", "bam2ncl.pl", \
                         project, \
                         donor, \
                         query_name, \
                         chromnum, \
                         linking, \
                         assembly], \
                        stdout=PIPE, stderr=PIPE)
            (out, err) = pop.communicate()
            messages.append(out)
            print "\n\nerr: ", err
            print "\n\nout: ", out
            t4 = time.time()
            print "done with bam2ncl, took: %f s" % (t4-t3)
            #print "returning %s" % out
        

        url = TRACK_TEMPLATE % (project, donor, query_name, UNBOUND_CHROM )
        track_data = {'label' : query_name, \
                      'key' : "%s/%s" % (project,query_name), \
                      'url' : "%s/trackData.json" % url, \
                      'type' : "FeatureTrack"}

        track_datas.append( track_data )
            
          
    messages = '\n'.join(messages)
    response = json.dumps({'status':'OK','message':messages,'trackData':track_datas})
    print "returning: ", response
    utils.printToServer( response  )

