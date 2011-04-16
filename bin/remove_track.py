#!/usr/bin/python
import sys
import json
import re
from os import mkdir
from shutil import rmtree

import cgi
import cgitb

#cgitb.enable()

def removeTracks( track_keys_to_remove, ref, delete=False, filename = "../data/trackInfo.js" ) :

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
            deleteTracks( ref, [track["label"]] )

    if len(retained_tracks) == len(json_tracks) :
        raise Exception("No tracks were removed. Check to make sure the supplied track names are correct")
    
    tifile = open( filename, 'w')
    tifile.write( "%s = \n%s" % (js_var_name, json.dumps( retained_tracks, indent=4 )))
    tifile.close()

def deleteTracks( ref, track_labels ) :
    for label in track_labels :
        rmtree( "../data/tracks/%s/%s" % (ref,label) )


if __name__ == '__main__' :
    
    #print "Content-Type: text/html"     # HTML is following
    #print                               # blank line, end of headers
    #print 'sup'
    fields = cgi.FieldStorage()

    delete_name = "delete_track"
    if delete_name not in fields :
	to_remove = sys.argv[2:]
	ref = sys.argv[1]

#	print "ref: ", ref, "to_remove: ", to_remove
        removeTracks( to_remove, ref, delete=True )
    else :
        print "noooooooooooooooo"
#	        removeTracks( fields[delete_name], fields[ref_name], delete=True )
             
    #removeTracks( ['Linked Test rEAds'], delete=True );
    
    

  

