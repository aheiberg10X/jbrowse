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
from GlobalConfig import ROOT_DIR, DATA_DIR, TRACK_TEMPLATE, PROJECT_PREFIX, DEBUG_DIR, UPLOAD_DIR, UNBOUND_CHROM, DONOR_DIR, CHROM_PREFIX
import utils
import time
import os.path

importre = re.compile(r'\s*import\s+(.*)\s*;')

debugging = False 

def moveIfExists( source, dest ) :
    if os.path.exists( source ) :
        shutil.move( source, dest )
        return True
    else :
        print "\n%s doesn't exist!\n" % source
        return False

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
    assembly = "hg18"

else :
    fields = cgi.FieldStorage()
    query_name = fields.getvalue("query_name")
    query = fields.getvalue("query_box")
    #donor = fields.getvalue("query_donor")
    #chromnum = fields.getvalue("query_chrom")
    project = fields.getvalue("query_project");
    assembly = fields.getvalue("assembly");
    user_name = fields.getvalue("user_name");


#Open debugging files
err_filename = "%s/query_error.txt" % (DEBUG_DIR)
sys.stderr = open( err_filename,'w')
out_filename = "%s/query_output.txt" % (DEBUG_DIR)
sys.stdout = open( out_filename,'w')

#write the user query to a temporary file
query_loc = "%s/user_query.txt" % UPLOAD_DIR
fuq = open( query_loc, 'wb')
fuq.write( query)
fuq.close()

#run christos' code
print "popping run_biosql.sh"

t1 = time.time()

biosql_home = os.environ['BIOSQL_HOME']

#where to look for user uploaded interval tables
src_table_dir = "%s/src_tables/%s" % (biosql_home, project)
if not os.path.exists( src_table_dir ) :
    os.mkdir( src_table_dir )

#where should the output go?
trackpath = TRACK_TEMPLATE % (project, "%s", query_name, "%s")
dest_template = "%s/data/%s" % (ROOT_DIR, trackpath )

print query_loc, type(query_loc)
print dest_template, type(dest_template)
print src_table_dir, type(src_table_dir)
print assembly, type(assembly)

#TODO:
#look for donor bams in DONOR_DIR/user/donor/chr*
pop = Popen(['bash', \
             biosql_home + "/run_biosql.sh", \
             query_loc, \
             dest_template, \
             src_table_dir,
             assembly, \
             "%s/%s" % (DONOR_DIR, user_name)], \
            stdin=PIPE, stdout=PIPE, stderr=PIPE)

(out, err) = pop.communicate()
sys.stdout.write( out )
sys.stderr.write( err )
t2 = time.time()
print "done with run_biosql, took: %f s" % (t2-t1)


#check bytecode for syntax error
#also, get genome list from bytecode
fbyte = open(biosql_home + "/bytecode.txt")
first_line = fbyte.readline().lower()
if "syntax error" in first_line :
    message = first_line.replace(':',';')
    t = json.dumps({'status':'Query Syntax Error','message':first_line})
    fbyte.close()
    utils.printToServer( t )
    assert 1==9
else :
    donors = []
    for line in fbyte.readlines() :
        if "genome" in line :
            donors.append( line.split()[1] )

fbyte.close()

#Each track_data will be a JSON object describing the newly generated track to JBrowse
track_datas = []
#Will make sure we avoid adding a duplicate track_data
trackkeys = {}
#messages store the JSON object we pass back the AJAX call.  Let's the user know if the visualization conversion process was successful
messages = []

#visualize christos' files one by one
print "donors: ", donors
for donor in donors :
    
    #TODO:
    #overkill to do this for every chrom, just needs to happen once
    #copy query
    #put the file with the user query in the query_dir as a .gq file
    query_folder = dest_template.rsplit('/',1)[0] % donor
    print "query_folder,", query_folder
    moveIfExists( query_loc, \
                  "%s/%s.gq" % (query_folder, query_name) )

    #for chromnum in [str(x) for x in range(1,23) + ['X','Y']] :
    for listing in os.listdir( query_folder ) :
        print listing
        if listing.startswith(CHROM_PREFIX ) :
            chrom = utils.unprefix( listing )
            print "chrom", chrom
            if chrom.startswith('chr') :
                chrprefix = "chr"
                chromnum = chrom[3:]
            else :
                chrprefix = ""
                chromnum = chrom
            print "chromnum",chromnum

            #chrom = "chr%s" % str(chromnum)
            product_folder = dest_template % (donor, chrom)

            print "product_folder", product_folder
            if os.path.exists( product_folder ) and len( os.listdir( product_folder ) ) > 0 :
                print "exists!\n"
                for file in os.listdir( product_folder ) :
                    splt = file.split('.')
                    head, ext = splt[0], splt[-1]
                    trackkey = "%s/%s/%s" % (project,query_name,head)
                    something_to_vis = False
                    if ext == "interval" or ext == "short" :
                        something_to_vis = True
                        t3 = time.time()
                        print "starting interval2ncl for ", file
                        print "donor", donor, type(donor)
                        print "chromnum", chromnum, type(chromnum)
                        print "query_name", query_name, type(query_name)
                        print "assembly ", assembly

                        #make a new sub-foler for the file to be visualized
                        fldr = product_folder + "/" + head
                        os.mkdir( fldr )
                        moveIfExists( product_folder+"/"+file, fldr )
                        moveIfExists( product_folder+"/"+head+".hist", fldr )
                        interval_file = fldr + "/" + file

                        pop = Popen(["perl", "interval2ncl.pl", \
                                     trackkey, \
                                     query_name, \
                                     chromnum, \
                                     interval_file, \
                                     assembly], \
                                    stdout=PIPE, stderr=PIPE)

                        (out, err) = pop.communicate()
                        messages.append(out)

                        print "\n\nerr: ", err
                        print "\n\nout: ", out
                        t4 = time.time()
                        print "done with interval2ncl, took: %f s" % (t4-t3)

                        url = TRACK_TEMPLATE % \
                                (project, donor, query_name, \
                                 "%s%s" % (chrprefix,UNBOUND_CHROM ) ) \
                                 + "/" + head
                        
                        track_data = {'label' : trackkey, \
                                      'key' : trackkey, \
                                      'url' : "%s/trackData.json" % url, \
                                      'type' : "FeatureTrack"}

                        #don't want to add duplicate track_data for every chromosome
                        if trackkey in trackkeys :
                            pass
                        else :
                            track_datas.append( track_data )
                            trackkeys[trackkey] = 1

                if not something_to_vis :
                    #track_data = {'label': trackkey, \
                                  #'key': trackkey, \
                                  #'url': "nope", \
                                  #'type': 'DataTrack'}
                    messages.append( "%s - Nothing to visualize. No interval or short files." % chrom )


            #product_folder doesn't exist or is empty
            else :
                trackkey = "%s/%s" % (project, query_name)
                track_data = {'label': trackkey, \
                              'key': trackkey, \
                              'url': "nope", \
                              'type': 'BlankTrack'}
                messages.append( "%s - Nothing to visualize. The destination folder is empty." % chrom )


            if trackkey in trackkeys :
                pass
            else :
                track_datas.append( track_data )
                trackkeys[trackkey] = 1

#TODO 
#add some sort of trackData here, want to be able to download results
if len(messages) == 0 :
    messages.append("Shouldn't get here. Nothing to visualize.")
    #track_data = {'label': trackkey, \
                  #'key': trackkey, \
                  #'url': "nope", \
                  #'type': 'DataTrack'}
    #track_datas.append( track_data )


messages = '\n'.join(messages)
response = json.dumps({'status':'OK','message':messages,'trackData':track_datas})
print "returning: ", response
utils.printToServer( response  )








#DEPRECATED
#in the query text, change:
# import table.tbl => import /projectpath/interval_tables/<projectname>_table.tbl
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


