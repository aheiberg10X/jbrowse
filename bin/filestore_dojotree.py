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

perms = {"earthworm jim" : {"NA18507" : False}}

def getChildren( path ) :
    r = []
    for thing in os.listdir( path ) :
        if show(path,thing) :
            r.append( thing )
    r.sort()
    return r

#needed by spec, see[]
def handleQuery( path ) :
    total = 0
    items = []
    things = os.listdir( path )
    things = sorted( things, key=utils.chromKeyer )
    for thing in things :
        total += 1
        fullpath = "%s/%s" % (path,thing)
        item = makeItem( fullpath )
        if item :
            items.append( item )

    jresponse = {'total' : total, 'items' : items}
    response = json.dumps( jresponse )
    print response
    utils.printToServer( response )

#needed by spec, see[]
def handlePath( path ) :
    item = makeItem( path )
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

#def getPrefix( path, name ) :
    #if name.startswith( PRIVATE_PREFIX ) :
        #return PRIVATE_PREFIX
    #else :
        #if os.path.isdir( path ) :
            #if name.startswith( QUERY_PREFIX ) :
                #return QUERY_PREFIX
            #elif name.startswith( CHROM_PREFIX ) :
                #return CHROM_PREFIX
            #elif name.startswith( DONOR_PREFIX ) :
                #return DONOR_PREFIX
            #else :
                #return PRIVATE_PREFIX
        #else :
            #return "file"

def hasPermission( user, path, name ) :
    if user == 'su' : return True
    else :
        if name.startswith(DONOR_PREFIX) :
            try :
                return perms[user][utils.unprefix(name)]
            except KeyError :
                return False
        else :
            return True

def show( path, name ) :
    prefix = getPrefix( path,name )
    return not( prefix == 'file' or prefix == PRIVATE_PREFIX ) and \
           hasPermission("su",path,name)

def makeItem( path ) :
    print "path: %s" % path
    (head,gparent_name,parent_name,name) = path.rsplit('/',3)
    gparent = "%s/%s" % (head, gparent_name)
    parent= "%s/%s" % (gparent, parent_name)
    #is_dir = os.path.isdir(path)
    prefix = getPrefix( path, name )
    is_dir = prefix == DONOR_PREFIX or \
             prefix == PROJECT_PREFIX
            #prefix == CHROM_PREFIX or \
    item = {}
    if show( path, name) :
        name = utils.unprefix(name)
        parent_prefix = getPrefix( parent, parent_name )
        parent_name = utils.extractName(parent_prefix, path)
        item = {'name' : name, \
                'parentDir' : parent, \
                'directory' : is_dir, \
                'path' : path,
                'prefix' : prefix}

        if prefix == QUERY_PREFIX : 
            assert parent_prefix == DONOR_PREFIX
            item['project'] = gparent_name
            item['donor'] = parent_name
            item['key'] = "%s/%s/%s" % (gparent_name,parent_name,name)
            tt = TRACK_TEMPLATE % \
                    (gparent_name, parent_name, name, UNBOUND_CHROM)
            item['url'] = "%s/trackData.json" % tt
            item['label'] = name
            item['type'] = 'FeatureTrack'

        if is_dir :
            item["children"] = getChildren( path )

    return item


if __name__ == '__main__' :

    utils.printToServer( "Content-type: text/json\n\n" )

    projects_root = "%s/data/tracks" % ROOT_DIR

    sys.stderr = open("%s/filestore_err.txt" % DEBUG_DIR,'w')
    sys.stdout = open("%s/filestore_out.txt" % DEBUG_DIR,'w')

    dparams = cgi.parse()
    print dparams

    if 'path' in dparams :
        print 'handel path:', dparams['path'][0]
        handlePath( dparams['path'][0] )
    else :
        handleQuery( projects_root )

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
