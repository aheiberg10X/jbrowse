import sys
sys.path.append('../lib')
import simplejson as json
import re
import GlobalConfig

textarea_opener = "<html><body><textarea>"
textarea_closer = "</textarea></body></html>"

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

#normally we'll have a file setup so we can print debug info
#and we'll point stdout to this file for convenience
#but eventually we'll want to to print to true stdout (the server)
def printToServer( s ) :
    temp = sys.stdout
    sys.stdout = sys.__stdout__
    print s
    sys.stdout = temp

def printPayload( json_data ) :
    wrapped = textarea_opener + json_data + textarea_closer
    printToServer( wrapped )

#WATCH OUT WHAT IF name HAS AN UNDERSCORE IN IT NATURALLY?
def unprefix( name ) :
    splt = name.split('_',1)
    if len(splt) == 2 :
        return splt[1]
    else : return splt

def extractName( prefix, path ) :
    if not prefix : return ""
    splt = path.split('/')
    for s in splt :
        if prefix in s :
            return unprefix(s)

if __name__ == '__main__' :
    print extractName('andrew@rodion:~/school/dnavis/jbrowse/data/tracks/chrom_chr1/donor_NA18507')
