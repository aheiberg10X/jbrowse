from string import atoi, split
from sys import exit

def open_file(fname):
	try:
		f=open(fname)
	except:
		print 'cannot open '+fname
		exit(1)
	return f

def parse_file(fname):
	f=open_file(fname)
	ret={}
	for line in f:
		num=atoi(line.split()[1])
		if num in ret:
			ret[num]+=1
		else:
			ret[num]=1

	f.close()
	return ret

dct=parse_file("keys.txt")
f=open_file("joined_indexes.txt")
f.next()

for line in f:
	key=atoi(line.split()[1])
	if key in dct:
		dct[key]-=1
		if dct[key]==0:
			dct.pop(key)
#	try:
#		dct[key]-=1
#		if dct[key]==0:
#			dct.pop(key)
	else:
		print 'key %d not found'%key

for k in dct:
	print 'missing ', k


