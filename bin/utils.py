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

#supply to sorted( key= ) to sort lists in correct chrom order
#if doesn't start with 'chr' no changes to alphanumeric sort order are made
def chromKeyer( chrom ) :
    first,value = chrom[:3], chrom[3:]
    if first.lower() == 'chr' :
        if value.lower() == 'x' : return 31
        elif value.lower() == 'y' : return 32
        else :
            try :
                return int(value)
            except ValueError :
                return chrom
    else :
        return chrom
