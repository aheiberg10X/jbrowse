from string import split, atoi, rfind
from sys import exit
from copy import copy, deepcopy
from os import rename
import sys


def ioerror(msg):
	print >>sys.stderr, msg
	exit(1)

def open_file(fname):
	try:
		f=open(fname)
	except:
		ioerror("cannot open file "+fname)
	return f

class table:
	def __init__(self):
		self.name=""
		self.kind=""
		self.srctable=[]
		self.physical_loc=[]
		self.intermediate_products=[]
		self.mates_on=[]
	

	#if kind==READS physical_loc=[indx_file, products_dir/name.flt]
	def update_physical_loc(self, products_dir, indx_file):
		if (self.kind=="Reads"):
			if indx_file is not None:
				self.physical_loc=[indx_file, products_dir+'/'+self.name+'.flt']
			else:	
				ioerror("Wrong call 1 of update_physical_loc")
			return
		
		if indx_file is not None:
			ioerror("Wrong call 2 of update_physical_loc")
	
		if (self.kind=="Text_table"):
			self.physical_loc=[products_dir+'/'+self.name+'.txt']
		elif (self.kind=="Intervals"):
			self.physical_loc=[products_dir+'/'+self.name+'.intrvl']
		elif (self.kind=="Joined"):
			self.physical_loc=[products_dir+'/'+self.name+'.joined']
		else:	ioerror("Wrong call 3 of update_physical_loc")

#if src is in tables it adds a copy of the respective entry to src_table.
#Otherwise it brings it from src_table_dir.
	def update_src(self, tables, src, src_table_dir, products_dir, chr_name):
		if src in tables:
			self.srctable.append(copy(tables[src]))
		else:
			global chr_locations
			cur=table()
			cur.name=src
			cur.kind="Text_table"
			cur.physical_loc=[products_dir+'/'+src+'.txt']
			print 'awk \'{if($%s==\"%s\") print $0}\' %s > %s'%(chr_locations[src], chr_name, src_table_dir+'/'+src+'.txt', cur.physical_loc[0])
			self.srctable.append(copy(cur))
			tables[src]=copy(cur)

#it returns  the contents of code_skeletons.txt for the specified file_type
def get_skeleton_code(file_type):
	f=open_file('code_skeletons.txt')
	ret=''
	flag=False
	for line in f:
		if line.count('***'+file_type)>0:
			flag=True
			continue
		elif line.count('***') > 0:
			flag=False
			continue
		elif flag:
			if line.count('int ev') >0:
				ret+=line[:-1]
			else:
				ret+=line
	f.close()
	return ret

#it translates an expression from a READS table to cpp code 
def translate_read(line):
	ret=''
	sp_line=line.split()
	for str in sp_line:
		if len(str.split(':'))>0:
			str=str.split(':')[-1] #strip trailing table names
		if str in ['intvar', 'stringvar', 'floatvar', 'charvar']:
			continue
		if str=='length':
			ret+='rindx[i].read_len '
		elif str=='location':
			ret+='rindx[i].loc '
		elif str=='strand':
			ret+='get_strand(i) '
		elif str=='mate_length':
			ret+='rindx[mate].read_len '
		elif str=='mate_loc':
			ret+='rindx[mate].loc '
		elif str=='mate_strand':
			ret+='get_strand(mate) '
		else:
			ret+=(str+' ')
	return ret

def translate_txt(line):
	ret=''
	sp_line=line.split()
	i=0
	while(i<len(sp_line)):
		if sp_line[i] in ['intvar', 'stringvar','floatvar', 'charvar']:
			tpe=sp_line[i][:-3]
			i+=1
			tok_pos=atoi(sp_line[i].split('arg')[1])-1 #indexing starts from 0
			token='tokens[%d]'%tok_pos
			ctoken=token+'.c_str()'
			if tpe=='string':
				ret+=(token+' ')
			elif tpe=='int':
				ret+=('atoi('+ctoken+') ' )
			elif tpe=='float':
				ret+=('atof('+ctoken+') ' )
			elif tpe=='char':
				ret+=(ctoken+'[0] ' )
		else:
			ret=ret+sp_line[i]+' '
		i+=1
	return ret

def translate_interval(line):
	ret=''
	sp_line=line.split()
	for str in sp_line:
		if len(str.split(':'))>0:
			str=str.split(':')[-1] #strip trailing table names
		if str in ['intvar', 'stringvar', 'floatvar', 'charvar']:
			continue
		elif str=='begin' or str=='start' or str=='st':
			ret+='vec[i].st '
		elif str=='end' or str=='nd':
			ret+='vec[i].nd '
		else:
			ret=ret+str+' '
	return ret

#it parses the variables of line and returns the initialization code for the
#needs of translate_join. It also returns a dictionary where each variable 
#corresponds to a vector
def var_processing_join(line):
	global tables
	vars={}
	retstr='\tif (variables.size()==0){\n'
	i=0
	sp_line=line.split()
	varno=1
	while i<len(sp_line):
		if sp_line[i] in ['intvar', 'stringvar','floatvar', 'charvar']:
			tpe=sp_line[i][:-3]
			i+=1
			orig_var=sp_line[i]
			if orig_var in vars:
				i+=1
				continue
			#modify var now
			new_var=''
			j=0
			sp_var=orig_var.split(':')
			for j in range(len(sp_var)-1):
				#if tables[sp_var[j]].kind=='READS' and tables[sp_var[j]].name!='READS':
				#	continue
				new_var=new_var+tables[sp_var[j]].physical_loc[0]+':'
				if tables[sp_var[j]].kind in ['Reads', 'Text_table', 'Intervals']:
					break
			new_var+=sp_var[-1]
			retstr+='\t\tvariables.push_back(\"%s\"); types.push_back(\"%s\");\n'%(new_var, tpe);
			vars[orig_var]=['arg%d'%varno, tpe, varno-1]
			varno+=1
		i+=1
	retstr+='\t}\n'
	return vars, retstr
	

def translate_join(line):
	ret=''
	vars, ret=var_processing_join(line)
	ret+='\tif(vec_addr.size()==0) dereference_variables(variables, types, vec_addr, this);\n'
	for v in vars:
		ret+='\tvector<%s>* %s=reinterpret_cast<vector<%s>* >(vec_addr[%d]);\n'%(vars[v][1], vars[v][0], vars[v][1], vars[v][2])
	sp_line=line.split()
	i=0
	ret+='\tint ev = '
	while i<len(sp_line):
		if sp_line[i] in ['intvar', 'stringvar','floatvar', 'charvar']:
			i+=1
			cur_var=sp_line[i]
			ret+='(*%s)[i] '%vars[cur_var][0]
		else:
			ret+=(sp_line[i]+' ')
		i+=1
	return ret



		

#it generates a cpp file (c_file) that contains the input code
def create_custom_file(c_file, file_type, code_line1, code_line2):
	f=open(c_file, 'w')
	src=get_skeleton_code(file_type)
	if file_type=='rd_condition':
		src=src+translate_read(code_line1)+';\n\treturn ev;\n}'
	elif file_type=='txt_condition':
		src=src+translate_txt(code_line1)+';\n\treturn ev;\n}'
	elif file_type=='join_condition':
		src=src+translate_join(code_line1)+';\n\treturn ev;\n}'
	elif file_type=='intrvl_condition':
		src=src+translate_interval(code_line1)+';\n\treturn ev;\n}'
	elif file_type=='rd_intervals':
		src=src+'\texpr1= '+translate_read(code_line1[4:])+';\n'
		src=src+'\texpr2= '+translate_read(code_line2[4:])+';\n}'
	elif file_type=='txt_intervals':
		src=src+'\texpr1= '+translate_txt(code_line1[4:])+';\n'
		src=src+'\texpr2= '+translate_txt(code_line2[4:])+';\n}'
	elif file_type=='join_intervals':
		src=src+'\texpr1= '+translate_join(code_line1[4:])+';\n'
		src=src+'\texpr2= '+translate_join(code_line2[4:])+';\n}'
	elif file_type=='intrvl_intervals':
		src=src+'\texpr1= '+translate_interval(code_line1[4:])+';\n'
		src=src+'\texpr2= '+translate_interval(code_line2[4:])+';\n}'
	else:
		ioerror("unknown file_type: "+file_type)
	print >> f, src
	f.close()



#tbl is the destination table, code is the user input
def create_filter_command(tbl, code, back_end_src):
	c_file= back_end_src+'/prod_'
	src_table=tbl.srctable[0]
	if tbl.kind=="Reads":
		c_file+=tbl.name+'.'+src_table.name+'rd_condition.cpp'
		create_custom_file(c_file, 'rd_condition', code, None)
		#create_filter_read(c_file, code)
		print 'cp %s %s/rd_condition.cpp \nmake'%(c_file, back_end_src)
		try:
			pout=back_end_src+'/all_tools filter_reads '+tbl.physical_loc[0]+' '+tbl.physical_loc[1]+' '
		except:
			print 'name: ',tbl.name, 'physical loc: ', tbl.physical_loc
			exit(1)
		#if len(tbl.srctable)>1:
		if tbl.srctable[0].name!="READS":
			pout += tbl.srctable[0].physical_loc[1]
		print pout
		return

	elif tbl.kind=="Text_table":
		c_file+=tbl.name+'.'+src_table.name+'txt_condition.cpp'
		create_custom_file(c_file, 'txt_condition', code, None)
		#create_filter_text(c_file, code)
		print 'cp %s %s/txt_condition.cpp \n make'%(c_file, back_end_src)
		pout=back_end_src+'/all_tools filter_text '

	elif tbl.kind=="Joined":
		c_file+=tbl.name+'.'+src_table.name+'join_condition.cpp'
		create_custom_file(c_file, 'join_condition', code, None)
		#create_filter_joined(c_file, code)
		print 'cp %s %s/join_condition.cpp \n make'%(c_file, back_end_src)
		pout= back_end_src+'/all_tools filter_joined '

	elif tbl.kind=="Intervals":
		c_file+=tbl.name+'.'+src_table.name+'intrvl_condition.cpp'
		create_custom_file(c_file, 'intrvl_condition', code, None)
		#create_filter_intervals(c_file, code)
		print 'cp %s %s/interval_condition.cpp \n make'%(c_file, back_end_src)
		pout= back_end_src+'/all_tools filter_intervals '

	print pout+' '+tbl.srctable[0].physical_loc[0]+' '+tbl.physical_loc[0]

#tbl is the destination table, src_line contains info about the src
def create_interval_command(tbl, src_line, st_line, nd_line, back_end_src, products_dir):
	#c_file='%s/prod_'%back_end_src
	c_file='%s/prod_%s.%d.'%(back_end_src, tbl.name, len(tbl.srctable)+1)
	src=src_line.split()[3]
	global tables
	src_table=tables[src]
	table_src_name=src_table.name

	if len(tbl.physical_loc)>0:
		dst=tbl.physical_loc[0]
	else:
		dst=products_dir+'/'+tbl.name+'.'+src+'.'+'%d.'%(len(tbl.srctable)+1)+'intrvl'
		tbl.intermediate_products.append(copy(dst))

	#for name in tbl.srctable:
	#	if name==tbl.name:
	#		src_table=tbl.name
	#if tbl.srctable[0].kind=="READS":
	if src_table.kind=="Reads":
		mates_on='0'
		if src_line.count('both_mates')>0:
			mates_on='1'
			tbl.mates_on.append('1');
		else:
			tbl.mates_on.append('0');
		c_file+=tbl.name+'.'+table_src_name+'rd_intervals.cpp'
		create_custom_file(c_file, 'rd_intervals', st_line, nd_line)
		#create_interval_read(c_file, st_line, nd_line)
		print 'cp %s %s/rd_intervals.cpp \n make'%(c_file, back_end_src)
		#pout=back_end_src+'/all_tools reads2intervals '+tbl.srctable[0].physical_loc[0]+' '+tbl.srctable[0].physical_loc[1]+' '+mates_on+' '+tbl.physical_loc[0]
		#pout=back_end_src+'/all_tools reads2intervals '+src_table.physical_loc[0]+' '+src_table.physical_loc[1]+' '+mates_on+' '+tbl.physical_loc[0]
		pout=back_end_src+'/all_tools reads2intervals '+src_table.physical_loc[0]+' '+src_table.physical_loc[1]+' '+mates_on+' '+dst
		print pout
		return
	
	#elif tbl.srctable[0].kind=="Text_table":
	elif src_table.kind=="Text_table":
		tbl.mates_on.append('0');
		c_file+=tbl.name+'.'+table_src_name+'txt_intervals.cpp'
		create_custom_file(c_file, 'txt_intervals', st_line, nd_line)
		#create_interval_text(c_file, st_line, nd_line)
		print 'cp %s %s/txt_intervals.cpp \n make'%(c_file, back_end_src)
		pout=back_end_src+'/all_tools text2intervals '

	#elif tbl.srctable[0].kind=="Joined":
	elif src_table.kind=="Joined":
		tbl.mates_on.append('0');
		c_file+=tbl.name+'.'+table_src_name+'join_intervals.cpp'
		create_custom_file(c_file, 'join_intervals', st_line, nd_line)
		#create_interval_joined(c_file, st_line, nd_line)
		print 'cp %s %s/join_intervals.cpp \n make'%(c_file, back_end_src)
		pout= back_end_src+'/all_tools joined2intervals '

	#elif tbl.srctable[0].kind=="Intervals":
	elif src_table.kind=="Intervals":
		tbl.mates_on.append('0');
		c_file+=tbl.name+'.'+table_src_name+'intrvl_intervals.cpp'
		create_custom_file(c_file, 'intrvl_intervals', st_line, nd_line)
		#create_intrvl_intervals(c_file, st_line, nd_line)
		print 'cp %s %s/intrval_condition.cpp \n make'%(c_file, back_end_src)
		pout= back_end_src+'/all_tools intrvl2intervals '


	#print pout+' '+tbl.srctable[0].physical_loc[0]+' '+dst
	print pout+' '+src_table.physical_loc[0]+' '+dst
	#print pout+' '+tbl.srctable[0].physical_loc[0]+' '+tbl.physical_loc[0]
	return

def create_merge_command(tbl, merge_condition, src_line, back_end_src, table_dict, src_table_dir):
	src=src_line.split()[1]
	tbl.update_src(table_dict, src, src_table_dir, None, None)
	if merge_condition.count('>')>0:
		op='lower'
	else:
		op='upper'
	coverage=merge_condition.split()[2]
	print back_end_src+'/all_tools interval_merge '+tbl.srctable[0].physical_loc[0]+' '+ \
		tbl.physical_loc[0]+ ' '+ op + ' '+ coverage

	return

#join the src fields
def create_join_command(tbl, back_end_src):
	dst=tbl.physical_loc[0]
	src1=tbl.srctable[0].physical_loc[0] #or -1??? consult READS behavior
	kind1=tbl.srctable[0].kind
	src2=tbl.srctable[1].physical_loc[0]
	kind2=tbl.srctable[1].kind
	if kind1=='Reads':
		src1=tbl.mates_on[0]+src1
	if kind2=='Reads':
		if(len(tbl.mates_on)==2):
			src2=tbl.mates_on[1]+src2
		else: #this is an exceptional case when an interval table is input of 
				#the join command.
			src2=tbl.mates_on[0]+src2 

	if len(tbl.intermediate_products)==2: #same as above shit here 
		ip1=tbl.intermediate_products[0]
		ip2=tbl.intermediate_products[1]
	elif len(tbl.intermediate_products)==0:
		ip1=tbl.srctable[0].physical_loc[-1]
		ip2=tbl.srctable[1].physical_loc[-1]
	elif len(tbl.intermediate_products)==1:
		if atoi(tbl.intermediate_products[0].split('.')[-2])==2: #make sure that the number is correct
			ip1=tbl.intermediate_products[0]
			ip2=tbl.srctable[1].physical_loc[-1]
		elif atoi(tbl.intermediate_products[0].split('.')[-2])==3:
			ip2=tbl.intermediate_products[0]
			ip1=tbl.srctable[0].physical_loc[-1]
		else:
			ioerror('Cannot find Interval source')
	else:
		ioerror('Wrong number of intermediate products')

	#print back_end_src+'/all_tools joined_creation '+dst+' '+src1+' '+kind1+' '+src2+' '+kind2+' '+tbl.intermediate_products[0]+' '+tbl.intermediate_products[1]
	print back_end_src+'/all_tools joined_creation '+dst+' '+src1+' '+kind1+' '+src2+' '+kind2+' '+ip1+' '+ip2
	return


def init_reads(tables, indx_file):
	cur=table()
	cur.name="READS"
	cur.kind="Reads"
	cur.physical_loc=[indx_file]
	tables['READS']=copy(cur)


def main(argv=sys.argv):
	if len(argv)!=10:
		ioerror("usage: python code_generator.py <bytecode_file> <products_dir> <front_end_dir> <back_end_dir> <src_table_dir> <bam_file> <indx_file> <chr_name> <chr_length>")
	byte_code_file=argv[1]
	products_dir=argv[2]
	front_end_dir=argv[3]
	back_end_dir=argv[4]
	src_table_dir=argv[5]
	bam_file=argv[6] #"NA18507/chr1.bam"
	indx_file=argv[7] #"NA18507/chr1.bam.mates.indx"
	
	#bam_file="tmp_src/boom.bam"
	#indx_file="tmp_src/boom.bam.mates.indx"
	chr_name=argv[8] #"chr1"
	chromo_length=atoi(argv[9]) #250000000

	f=open_file(byte_code_file)
	lines=f.next()
	if lines.count('Compilation completed without errors')==0:
		ioerror('Errors during compilation')
	global tables
	global chr_locations
	tables={}
	chr_locations={} #indexed by table names. It contains the column no of chr
	init_reads(tables, indx_file)
	name=""
	kind=""
	srctable=[]
	physical_loc=[]
	cur=table()
	
	for lines in f:
		if lines.count('chr_loc:')>0:
			sp_lines=lines.split()
			chr_locations[sp_lines[1]]=sp_lines[-1]
		elif lines.count("result_table:")>0:
			cur.name=lines.split()[1]
		elif lines.count("INPUT:")>0:
			src=lines.split()[1]
			cur.update_src(tables, src, src_table_dir, products_dir, chr_name)
			#cur.srctable.append(copy(tables[src])) or case of genes.txt when first found.
		elif lines.count("WHERE")>0:
			cur.kind=cur.srctable[0].kind
			if cur.kind=='Reads':
				cur.update_physical_loc(products_dir, indx_file)

			else:
				cur.update_physical_loc(products_dir, None)
			create_filter_command(copy(cur), f.next(), back_end_dir) #call+code
		elif lines.count("Interval calling on")>0:
			create_interval_command(copy(cur), lines, f.next(), f.next(), back_end_dir, products_dir)
		elif lines.count("MAPJOIN")>0:
			cur.kind="Joined"
			cur.update_physical_loc(products_dir, None)
			create_join_command(copy(cur), back_end_dir)
		elif lines.count("CREATE_INTERVALS")>0:
			cur.kind="Intervals"
			cur.update_physical_loc(products_dir, None)
		elif lines.count("MERGE_INTERVALS")>0:
			cur.kind="Intervals"
			cur.update_physical_loc(products_dir, None)
			create_merge_command(copy(cur), f.next(), f.next(), back_end_dir, tables, src_table_dir)

		elif lines.count("End_of_sentence")>0:
			tables[cur.name]=copy(cur)
			cur=table()

		elif lines.count("print:") > 0:
			target=lines.split()[1]
			tbl=tables[target]
			prefx=products_dir+'/'+target
			if tables[target].kind=='Reads':
				mates_on='0'
				if lines.count("both_mates")>0:
					mates_on='1'
				print back_end_dir+'/all_tools print Reads '+ \
				tbl.physical_loc[0]+' '+ tbl.physical_loc[1]+' '+mates_on+prefx
			else:
				if lines.count("both_mates")>0:
					ioerror("both_mates to be combined with a table of type Reads only")
				print back_end_dir+'/all_tools print '+ tbl.kind+' '+ \
				tbl.physical_loc[0]+' '+ ' '+prefx

	



if __name__=="__main__":
	main()


