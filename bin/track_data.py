#!/usr/bin/python
import sys
sys.path.append("../lib")
import os
import simplejson as json
import re
import cgi
import cgitb
import utils
import filestore_dojotree
cgitb.enable()
from GlobalConfig import QUERY_PREFIX, PRIVATE_PREFIX, CHROM_PREFIX, DONOR_PREFIX, ROOT_DIR, DEBUG_DIR, TRACK_TEMPLATE, UNBOUND_CHROM

#'static' track data, such as SequenceTracks, + query track data inferred from directory structure
def getTrackData( root ) :
    #start from root and recursively makeItems
    #if they have a key, ie are queries/FeatureTracks turn only the relevant bits into a json string suitable for trackInfo.  
    #Make a big list of all these and print to the server
    #important to read in SequenceTrack data (and other "default" tracks, like one with gene annotations or something) here
    print "inside getTrackData"
    trackData = []
    
    #manually add the sequence trackdata
    seqtrack = \
    {"args" : {"chunkSize" : 20000}, \
     "url" : "seq/{refseq}/", \
     "type" : "SequenceTrack", \
     "label" : "DNA", \
     "key" : "DNA"}
    trackData.append( seqtrack )

    #generate all the query trackdata
    for listing in os.listdir(root) :
        fullpath = "%s/%s" % (root,listing)
        if listing.startswith( DONOR_PREFIX ) and \
           os.path.isdir( fullpath ) :
            for sublisting in os.listdir(fullpath) :
                subfullpath = "%s/%s" % (fullpath,sublisting)
                if sublisting.startswith( QUERY_PREFIX ) and \
                   os.path.isdir(subfullpath) :
                    item = filestore_dojotree.makeItem( subfullpath )
                    properties = ["url","label","key","type"]
                    entry = {}
                    for p in properties :
                        entry[p] = item[p]
                    trackData.append(entry)
                    print entry

    utils.printToServer( json.dumps(trackData) );

if __name__ == '__main__' :

    utils.printToServer( "Content-type: text/json\n\n" )

    project_root = "%s/data/tracks" % ROOT_DIR
    sys.stderr = open("%s/trackdata_err.txt" % DEBUG_DIR,'w')
    sys.stdout = open("%s/trackdata_out.txt" % DEBUG_DIR,'w')

    dparams = cgi.parse()
    print dparams

    getTrackData( project_root )

