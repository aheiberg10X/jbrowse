import os
import simplejson as json

tries = ["GlobalConfig.js", "../lib/GlobalConfig.js"]
good = False
for t in tries :
    if os.path.exists(t) :
        fin = open(t)
        good = True
        break

if not good :
    raise Exception("cannot find GlobalConfig.js from GlobalConfig.py")

fin.readline()
js = json.loads(fin.read())


ROOT_DIR = js["ROOT_DIR"]
DATA_DIR = ROOT_DIR + js["DATA_DIR"]
DEBUG_DIR = ROOT_DIR + js["DEBUG_DIR"]

QUERY_PREFIX = js["QUERY_PREFIX"]
PRIVATE_PREFIX = js["PRIVATE_PREFIX"]
CHROM_PREFIX = js["CHROM_PREFIX"]
DONOR_PREFIX = js["DONOR_PREFIX"]

fin.close()
