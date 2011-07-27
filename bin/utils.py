import simplejson as json
import re

# make this a function, doubtless will be using again
def fileToJson( filename ) :
    novar = re.compile(r'.*=(.*)', re.M | re.DOTALL)
    fin = open( filename )
    t = fin.read()
    m = novar.match( t )
    if not m : raise Exception("json file doesn't have a variable assignment")
    jtext = m.group(1)
    jconfig = json.loads( jtext )
    fin.close()
    return jconfig


