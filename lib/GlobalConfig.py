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
    raise Exception("cannot find GlobalConfig.js for GlobalConfig.py")

fin.readline()
globes = json.loads(fin.read())


ROOT_DIR = globes["ROOT_DIR"]
DATA_DIR = ROOT_DIR + globes["DATA_DIR"]
DEBUG_DIR = ROOT_DIR + globes["DEBUG_DIR"]
UPLOAD_DIR = ROOT_DIR + globes["UPLOAD_DIR"]

QUERY_PREFIX = globes["QUERY_PREFIX"]
PRIVATE_PREFIX = globes["PRIVATE_PREFIX"]
CHROM_PREFIX = globes["CHROM_PREFIX"]
DONOR_PREFIX = globes["DONOR_PREFIX"]

TRACK_TEMPLATE = globes["TRACK_TEMPLATE"] % (CHROM_PREFIX,DONOR_PREFIX,QUERY_PREFIX)

fin.close()
