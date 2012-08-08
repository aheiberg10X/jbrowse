#!/usr/bin/python
import sys
sys.path.append("../lib")
import os
import simplejson as json
import re
import cgi
import cgitb
import utils
cgitb.enable()

from GlobalConfig import QUERY_PREFIX, PRIVATE_PREFIX, CHROM_PREFIX, DONOR_PREFIX, ROOT_DIR, DEBUG_DIR, TRACK_TEMPLATE, UNBOUND_CHROM, PROJECT_PREFIX


perms = utils.fileToJson(  "../lib/permissions.json" )
dassembly = utils.fileToJson( "../lib/project_assembly_mapping.json" )


def getChildren( user_name, path ) :
    r = []
    for thing in os.listdir( path ) :
        if show( user_name, path, thing) :
            r.append( thing )
    r.sort()
    return r

#needed by spec, see[]
def handleQuery( user_name, path ) :
    print "handleQuery,", path
    total = 0
    items = []
    things = os.listdir( path )
    things = sorted( things, key=utils.chromKeyer )
    for thing in things :
        total += 1
        fullpath = "%s/%s" % (path,thing)
        item = makeItem( user_name, fullpath )
        if item :
            items.append( item )

    jresponse = {'total' : total, 'items' : items}
    response = json.dumps( jresponse )
    print response
    utils.printToServer( response )

#needed by spec, see[]
def handlePath( user_name, path ) :
    print "handlePath,", path
    item = makeItem( user_name, path )
    utils.printToServer( json.dumps( item ) )

def getPrefix( path, name ) :
    if name.startswith( PROJECT_PREFIX ) :
        return PROJECT_PREFIX
    elif name.startswith( DONOR_PREFIX ) :
        return DONOR_PREFIX
    elif name.startswith( QUERY_PREFIX ) :
        return QUERY_PREFIX
    elif name.startswith( CHROM_PREFIX ) :
        return CHROM_PREFIX
    else :
        return PRIVATE_PREFIX

def hasPermission( user, path, name ) :
    if user == 'su' : return True
    else :
        if name.startswith(PROJECT_PREFIX) :
            try :
                return perms[user][utils.unprefix(name)]
            except KeyError :
                return False
        else :
            return True

def show( user_name, path, name ) :
    prefix = getPrefix( path,name )
    return not( prefix == 'file' or prefix == PRIVATE_PREFIX ) and \
           hasPermission( user_name, path, name)

def makeItem( user_name, path ) :
    print "path: %s" % path
    (head,gparent_name,parent_name,name) = path.rsplit('/',3)
    gparent = "%s/%s" % (head, gparent_name)
    parent= "%s/%s" % (gparent, parent_name)
    #is_dir = os.path.isdir(path)
    prefix = getPrefix( path, name )
    is_dir = prefix == DONOR_PREFIX or prefix == PROJECT_PREFIX
    item = {}
    if show( user_name, path, name) :
        name = utils.unprefix(name)
        parent_prefix = getPrefix( parent, parent_name )
        gparent_prefix = getPrefix( gparent, gparent_name )
        gparent_name = utils.extractName(gparent_prefix, path)
        parent_name = utils.extractName(parent_prefix, path)
        item = {'name' : name, \
                'parentDir' : parent, \
                'directory' : is_dir, \
                'path' : path,
                'prefix' : prefix}

        if prefix == PROJECT_PREFIX :
            if name in dassembly :
                item['assembly'] = dassembly[name]
            else :
                item['assembly'] = "n/a"


        if prefix == QUERY_PREFIX : 
            assert parent_prefix == DONOR_PREFIX
            item['project'] = gparent_name
            item['donor'] = parent_name
            #project/query_name
            item['key'] = "%s/%s" % (gparent_name,name)
            tt = TRACK_TEMPLATE % \
                    (gparent_name, parent_name, name, UNBOUND_CHROM)
            item['label'] = name
            item['type'] = 'FeatureTrack'

            if item['project'] in dassembly :
                item['assembly'] = dassembly[item['project']]
            else :
                item['assembly'] = "n/a"
            
            #Associate these three keys with each query menuitem
            #It will tell the browser how many of each filetype we
            #have in the chrom folders.  
            #(Remember: Using the counts for chr1)
            #dkey_ext = {"bam_ixs" :      ".bam", \
                        #"interval_ixs" : ".interval", \
                        #"txt_ixs" :      ".txt" }

            #for key in dkey_ext :
                #item[key] = [] 
            item["bams"] = []
            item["txts"] = []
            item["sub_results"] = []
            item["sub_exts"] = []
            folder = "%s/data/%s" % (ROOT_DIR, tt.replace( UNBOUND_CHROM, 'chr1' ) )
            print "folder is %s" % folder
            #TODO
            #more specific than just looking for a .txt file?
            for thing in sorted( os.listdir( folder ) ) :
                if os.path.isdir( folder+"/"+thing ) and thing != ".svn" :
                    print "listdir:", thing
                    item["sub_results"].append( thing )
                    for subthing in sorted(os.listdir( folder+"/"+thing )) :
                        head,ext = subthing.rsplit(".",1)
                        if ext == "short" or ext == "interval" :
                            if ext == "short" :
                                item["sub_exts"].append( "bam.short" )
                            else :
                                item["sub_exts"].append( ext )
                else :
                    head,ext = thing.rsplit(".",1)
                    if ext == "bam" :
                        item["bams"].append( thing )
                    if ext == "txt" :
                        item["txts"].append( thing )



                #for key in dkey_ext :
                    #if thing.lower().endswith( dkey_ext[key] ) :
                        #(head, ext) = thing.rsplit(".",1)
               # 
                        ##the new run_query will 
                        #splt = head.rsplit("+",1)
                        #if len(splt) == 2 :
                            #(head, i) = splt
                        #else :
                            #i = "0" 
                        #item[key].append( i )
                        #break
                   # 
            #for key in dkey_ext :
                #item[key] = ",".join( item[key] ) 
            if len(item['sub_results']) > 0 :
                item['url'] = "%s/%%s/trackData.json" % tt
            else :
                item['url'] = "%s/trackData.json" % tt
        if is_dir :
            item["children"] = getChildren( user_name, path )

    return item


if __name__ == '__main__' :

    utils.printToServer( "Content-type: text/json\n\n" )

    projects_root = "%s/data/tracks" % ROOT_DIR

    sys.stderr = open("%s/filestore_err.txt" % DEBUG_DIR,'w')
    sys.stdout = open("%s/filestore_out.txt" % DEBUG_DIR,'w')

    user_name = "generic"
    for kvpair in os.environ["HTTP_COOKIE"].split(";") :
        [k,v] = kvpair.split("=")
        if k.endswith("user_name") :
            user_name = v


    print "user_name", user_name
    dparams = cgi.parse()
    print "fields, ",cgi.FieldStorage()
    print dparams
    #if 'user_name' in dparams :
        #user = dparams['user']
    user_name = "dev"
    if 'path' in dparams :
        handlePath( user_name, dparams['path'][0] )
    else :
        fqr = open("%s/query_requested.txt" % DEBUG_DIR,'w')
        fqr.write( str(cgi.FieldStorage()) )
        fqr.close()
        handleQuery( user_name, projects_root )
    #else :
        #print "no user in dparams!"



    sys.stderr.close()
    sys.stdout.close()

#print os.environ['REQUEST_URI']
#keys = ["query","queryOptions","start","count"]
#(query,queryOptions,start,count) = values

#{
  #"total": 2496,
  #"items": [
     #{"name": "foo0.txt", "parentDir": "some/dir0", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir0/foo.txt"},
     #{"name": "foo1.txt", "parentDir": "some/dir1", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir1/foo1.txt"},
     #{"name": "foo2.txt", "parentDir": "some/dir2", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir2/foo2.txt"},
     #{"name": "foo3.txt", "parentDir": "some/dir3", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir3/foo3.txt"},
   #{"name": "foo4.txt", "parentDir": "some/dir4", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir4/foo4.txt"},
     #{"name": "foo5.txt", "parentDir": "some/dir5", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir5/foo5.txt"},
     #{"name": "foo6.txt", "parentDir": "some/dir6", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir6/foo6.txt"},
     #{"name": "foo7.txt", "parentDir": "some/dir7", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir7/foo7.txt"},
     #{"name": "foofiles.txt", "parentDir": "some/dir8", "size": 1234, "modified": 1234567, "directory": true, "path": "some/dir8/foofiles.txt", "children": ["tmp1","tmp2","tmp3"]},
     #{"name": "foo9.txt", "parentDir": "some/dir9", "size": 1234, "modified": 1234567, "directory": false, "path": "some/dir9/foo9.txt"},
  #]
#}
#
#
###
