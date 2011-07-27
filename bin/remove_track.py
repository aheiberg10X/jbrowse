#!/usr/bin/python
import sys
import simplejson as json
import re
from os import mkdir
from shutil import rmtree
import cgi
import cgitb

from utils import fileToJson
#cgitb.enable()

def removeTracks( chrom, track_keys_to_remove, delete=False, filename = "../data/trackInfo.js" ) :

    #the name javascript expects the json to be loaded into
    js_var_name = "trackInfo"
    notrack = re.compile(r'\s*%s\s*=' % js_var_name)
    nowhite = re.compile(r'\n\t\r+')
    tifile = open(filename,'r')

    cleaned_lines = []
    for line in tifile.readlines() :
        line = notrack.sub('',line)
        line = nowhite.sub('',line)
        cleaned_lines.append( line )

    cleaned_json = ''.join(cleaned_lines)
    json_tracks = json.loads( cleaned_json )
    tifile.close()

    track_keys_to_remove = [k.lower() for k in track_keys_to_remove]
    #print track_keys_to_remove

    retained_tracks = []
    for track in json_tracks :
        #print track
        if str(track["key"]).lower() not in track_keys_to_remove :
            retained_tracks.append( track )
        elif delete :
            deleteTracks( chrom, [track["label"]] )

    if len(retained_tracks) == len(json_tracks) :
        raise Exception("No tracks were removed. Check to make sure the supplied track names are correct")

    tifile = open( filename, 'w')
    tifile.write( "%s = \n%s" % (js_var_name, json.dumps( retained_tracks, indent=4 )))
    tifile.close()

def deleteTracks( chrom, track_labels ) :
    for label in track_labels :
        rdir = "../data/tracks/%s/%s" % (chrom,label)
        try :
            rmtree( rdir )
        except OSError :
            print "can't remove %s\n" % rdir


if __name__ == '__main__' :
    jglobals = fileToJson( "../lib/GlobalConfig.js" )
    root_dir = jglobals['root_dir']

    fields = cgi.FieldStorage()
    print 'Content-type: text/html\n\n'

    if not fields :
        chrom = sys.argv[1]
        to_remove = sys.argv[2:]
        removeTracks( chrom, to_remove, delete=True )
    else :
        sys.stderr = open("%s/uploads/delete_error.txt" % root_dir,'w')
        sys.stdout = open("%s/uploads/delete_output.txt" % root_dir,'w')
        print "iuoasdhvoweovnwie"
        print fields
        print fields.getvalue("chrom")
        print fields.getlist("delete_track")

        removeTracks( fields.getvalue("chrom"), fields.getlist("delete_track"), delete=True )

    #removeTracks( ['Linked Test rEAds'], delete=True );





