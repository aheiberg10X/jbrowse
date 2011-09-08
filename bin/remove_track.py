#!/usr/bin/python
import sys
sys.path.append("../lib")
import simplejson as json
import re
from os import mkdir
from os.path import exists
from shutil import rmtree
import cgi
import cgitb
import GlobalConfig
import utils
#cgitb.enable()

def removeQuery( donor, query_name, delete=False, filename = "../data/trackInfo.js" ) :

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

    retained_tracks = []
    for track in json_tracks :
        if not track["key"].lower() == "%s/%s" % (donor.lower(), query_name.lower()) :
            print "retaining %s" % track["key"]
            retained_tracks.append( track )
        elif delete :
            try :
                chroms = range(1,23) + ['X','Y']
                for c in chroms :
                    trackpath = GlobalConfig.TRACK_TEMPLATE % ("chr%s" % str(c), donor, query_name)
                    fullpath = "%s/%s" % (GlobalConfig.DATA_DIR, trackpath)
                    if( exists(fullpath) ) :
                        rmtree( fullpath )
            except OSError as e :
                print "can't remove %s: %s\n" % (trackpath,str(e))

    if len(retained_tracks) == len(json_tracks) :
        raise Exception("No tracks were removed. Check to make sure the supplied track names are correct")

    tifile = open( filename, 'w')
    tifile.write( "%s = \n%s" % (js_var_name, json.dumps( retained_tracks, indent=4 )))
    tifile.close()


if __name__ == '__main__' :
    root_dir = GlobalConfig.ROOT_DIR 

    fields = cgi.FieldStorage()
    print 'Content-type: text/json\n\n'

    if not fields :
        donor = sys.argv[1]
        query_name = sys.argv[2]
        removeQuery( donor, query_name, delete=True )
    else :
        sys.stderr = open("%s/delete_error.txt" % GlobalConfig.DEBUG_DIR,'w')
        sys.stdout = open("%s/delete_output.txt" % GlobalConfig.DEBUG_DIR,'w')
        fields = cgi.parse()
        print fields
        utils.printToServer('<html><body><textarea>')
        #try :
        removeQuery( fields["donor"][0], fields["query_name"][0], delete=True )
        utils.printToServer( '{"status":"ok", "message":"Query Deleted"}' )
        #except Exception :
            #utils.printToServer( '{"status":"error", "message":"Something went wrong, check the logs"}' )

        utils.printToServer('</textarea></body></html>')





