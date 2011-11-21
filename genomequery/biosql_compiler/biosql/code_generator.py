from string import split, atoi, rfind
from sys import exit
from copy import copy, deepcopy
from os import rename
import sys

class Table:
	def __init__(self):
		self.mates_en=0
		self.origin_path=[] #A string that contains a dot separated sequence
							#of the tables that produced the current table. 
							#(e.g H2.H1.READS). The array is useful in case of 
							#join
		self.kind='regular' #mapjoin vs interval vs regular
		self.interval_code_files=[]
		self.filtering_cmd=[]
	
tables={} #it is indexed by table_name and contains Table objects
tables["READS"]=Table() #(None, 0)
tables["READS"].kind='READS'

def ioerror(msg):
	print >>sys.stderr, msg
	exit(1)

def open_file(fname):
	try:
		f=open(fname)
	except:
		ioerror("cannot open file "+fname)
	return f

#it translates several words into proper c representation
def translate2read(word_prefx, word, indx_name, pos):
	#print 'word_prefx, word, indx: ',word_prefx, word, indx_name
	strand_indx=indx_name+"_strand"
	if word==word_prefx+"length":
		return indx_name+"[%s].read_len"%pos
	elif word==word_prefx+"location":
		return indx_name+"[%s].loc"%pos
	elif word==word_prefx+"strand":
		return "get_strand("+strand_indx+", strand_len, %s)"%pos
	elif word==word_prefx+"mate_strand":
		return "get_strand("+strand_indx+", strand_len, mate_indx)"
	elif word==word_prefx+"mate_loc":
		return indx_name+"[mate_indx].loc"
	elif word==word_prefx+"mate_length":
		return indx_name+"indx[mate_indx].read_len"
	elif word==word_prefx+"chr":
		return indx_name+"[%s].chr"%pos
	else:
		return word

def translate2import(word_prefx, word, indx_name, pos):
	if word==word_prefx+'begin':
		return indx_name+"[%s].start"%pos
	elif word==word_prefx+'end':
		return indx_name+"[%s].end"%pos
	elif word==word_prefx+'chr':
		return indx_name+"[%s].chr"%pos
	elif word==word_prefx+'annot_id':
		return indx_name+"[%s].id"%pos
	else:
		return word


#depending on the code type the function prints the header to the opened file f
def print_header(code_type, f):
	print >> f, "#include<stdio.h>\n#include\"all_tools.h\""
	line=''
	if code_type=='read_filtering':
		line="inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){\n\tint mate_indx=indx[i].mate_indx;"
	elif code_type=='import_filtering':
		line="inline int isvalid_imported(Imported_info *indx, int i, int ttl_table){"
	elif code_type=='interval_filtering':
		line='inline int get_interval_coverage_param(int *up_low){'
	elif code_type=='join_filtering':
		line='inline int isvalid_join(Join *join_lst, int i, Mates *indx1, long *strand_indx1, Mates *indx2, long *strand_indx2, Imported_info *tbl1, Imported_info *tbl2){\n \
\tJoin jn=join_lst[i];\n\tint i1=jn.indx1;\n\tint i2=jn.indx2;\n'

	elif code_type=='read_interval':
		line='inline Intervals get_rd_interval(Mates *indx, int i){\n\tint mate_indx=indx[i].mate_indx;\n\tIntervals ret;'
	elif code_type=='imported_interval':
		line='inline Intervals get_imported_interval(Imported_info *indx, int i){\n\tIntervals ret;'
	else:
		ioerror('Unknown code_type %s in print_header'%code_type)
	print >> f, line

#it returns READS 1, READS 2, tbl 1 or tbl 2 according to the path of c
def get_join_indx_prefx(src_table, c):
	sp_c=c.split('.')
	c_type=sp_c[-2] #e.g. H2.READS.location

	if c_type==src_table[0].split('.')[-1]:
		if c_type=='READS':
			return 'indx',1
		else:
			return 'tbl',1
	elif c_type==src_table[1].split('.')[-1]:
		if c_type=='READS':
			return 'indx',2 #READS1
		else:
			return 'tbl',2
	else:
		ioerror('Invalid combination of src table and c origin')

#it translates the input list of words into a line of statements
#that are a result of either translate2read or translate2import
#according to the input argument mode
def get_translate_str(mode, inpt_prefx, indx_name, word_lst, pos):
	ret=''
	#print 'inpt: %s indx: %s'%(inpt_prefx, indx_name)
	for w in word_lst:
		if mode=='read':
			ret+=translate2read(inpt_prefx+'.', w, indx_name, pos)
		else:
			ret+=translate2import(inpt_prefx+'.', w, indx_name, pos)
	ret+=';'
	return ret

#l is a line of intermediate code. Depending on the code_type and src_table
#the function returns the corresponding c equivalent, the value of mates_en 
#if it is found and the candidate return value of the c code.
def print_body(l, code_type, src_table):
	sp=l.split()
	ret=''
	ret_val=''
	if code_type=='read_filtering':
		ret='int '+sp[1]
		ret+=get_translate_str('read', src_table[-1], 'indx', sp[2:], 'i')
		ret_val=sp[1]
	elif code_type=='import_filtering':
		ret='int '+sp[1]
		ret+=get_translate_str('import', src_table[-1], 'indx', sp[2:], 'i')
		ret_val=sp[1]
	elif code_type=='interval_filtering':
		if sp[3].count('countvec')<=0:
			ioerror('Cannot find countvec for interval filtering')
		cov=sp[5]
		comp=sp[4]
		if (comp[0]=='>'):
			up_low='0'
		elif (comp[0]=='<'):
			up_low='1'
		else:
			ioerror('comparator should be either < or >')
		ret='int ret=%s;\n\t*up_low=%s;'%(cov, up_low)
		ret_val='ret'
	elif code_type=='join_filtering':
		ret='int '+sp[1]
		for c in sp[2:]:
			if len(c.split('.'))==1:
				ret+=c
			else:
				indx,side=get_join_indx_prefx(src_table, c)
				if indx=='indx':
					ret+=translate2read(src_table[side-1]+'.', c, indx+'%d'%side, 'i%d'%side)
				else:
					ret+=translate2import(src_table[side-1]+'.', c, indx+'%d'%side, 'i%d'%side)
		ret+=';'
		ret_val=sp[1]

	elif code_type=='read_interval':
		if sp[2]=='mate_en':
			print_body.mates_en=atoi(sp[4])
		elif sp[2]=='=':
			ret='int '+sp[1]
			ret+=get_translate_str('read', src_table[-1], 'indx', sp[2:], 'i')
		elif sp[2]=='start':
			ret='ret.st'
			ret+=get_translate_str('read', src_table[-1], 'indx', sp[3:], 'i')
		elif sp[2]=='end':
			ret='ret.nd'
			if print_body.mates_en==1: 
				rd2='mate_indx'
			else:
				rd2='i'
			ret+=get_translate_str('read', src_table[-1], 'indx', sp[3:], 'i')		
			ret+=';\n\tret.rd1=i;\n\tret.rd2=%s;'%(rd2)
			ret_val='ret'

	elif code_type=='imported_interval':
		if sp[2]=='=':
			ret='int '+sp[1]
			ret+=get_translate_str('table', src_table[-1], 'indx', sp[2:], 'i')
		elif sp[2]=='start':
			ret='ret.st'
			ret+=get_translate_str('table', src_table[-1], 'indx', sp[3:], 'i')
		elif sp[2]=='end':
			ret='ret.nd'
			ret+=get_translate_str('table', src_table[-1], 'indx', sp[3:], 'i')
			ret+=';\n\tret.rd1=i;\n\tret.rd2=-1;'
			ret_val='ret'
	else:
		ioerror('Unknown code_type %s in print_header'%code_type)
	return ret, ret_val


#it transforms the input lines[st:] into file isvalid.c. 
#The translation stops when end_*_code is found.
#The function returns the tuple (status, line_nd, mates_en). if 
#the file has been created status=1, otherwise status=0.
#line_nd is going to contain the lineNo of end_filtering_code
#src_table can be either READS or a name of an imported table.
#code_type can be read_filtering, import_filtering (...to be continued)
def produce_code(lines, st, dst_file, src_table, code_type):
	sp_lines=lines[st].split()
	if sp_lines[1]=='N/A':
		return (0, st+1, None)
	f=open(dst_file, 'w')
	#print 'dst_file: ',dst_file
	print_header(code_type, f)
	cur_l=st
	print_body.mates_en=None
	for l in lines[st:]:
		spl=l.split()
		if spl[1]=='end_filtering_code' or spl[1]=='end_interval_spec':
			print >> f, '\treturn '+ret_val+';'
			print >> f, '}'
			f.close()
			return(1, cur_l, mates_en)
		c_line, ret_val=print_body(l, code_type, src_table)
		mates_en=print_body.mates_en
		print >> f, '\t%s'%c_line
		cur_l+=1
	return(1, st+1, mates_en)





	



#it parses the load arguments and figures the locations of id, chr, start, and end
#it returns the line number after end_load_table
def	get_loaded_fields(codelines, i, tbl_name, src_table_dir, products_dir, back_end_dir, chromo):
	id_col=-1
	chr_col=-1
	st_col=-1
	nd_col=-1
	cur_col=1
	while i<len(codelines):
		line=codelines[i][:-1]
		sp_line=line.split(" ")
		if sp_line[1]=="end_load_table":
			break
		if sp_line[1]!="loaded_param":
			ioerror("Bad format of intermediate \"load\" code")
		if sp_line[2]=="char*" and (sp_line[3]=="id" or sp_line[3][-3:]=="_id"):
			id_col=cur_col
		elif sp_line[2]=="char*" and (sp_line[3]=="chr" or sp_line[3][:3]=="chr"):
			chr_col=cur_col
		elif sp_line[2]=="int" and (sp_line[3]=="begin" or sp_line[3]=="start"):
			st_col=cur_col
		elif sp_line[2]=="int" and (sp_line[3]=="end"):
			nd_col=cur_col
		cur_col+=1
		i+=1
	if (id_col<0 or chr_col<0 or st_col<0 or nd_col<0):
		ioerror("Cannot load table because an important parameter is missing")
	newtable=Table()
	tables[tbl_name]=newtable
	newtable.kind='imported'
	table_src=src_table_dir+"/"+tbl_name+".txt"
	table_dest=products_dir+"/"+tbl_name+".table"
	cmd_string="%s/all_tools load_table %s %s %d %d %d %d %s"%(back_end_dir, table_src, table_dest, id_col, chr_col, st_col, nd_col, chromo)
	print cmd_string
	return i+1

  



#it checks whether tbl has been originated by reads or it from an imported table. It returns either "in" (if tbl==READS)
#or the name of the imported table. cmd_list is the list of the commands
def trace_table(tbl, cmd_list):
	if tbl=="READS" or tbl=="in":
		#return "READS"
		return "in"
	for cmd in cmd_list:
		if tbl==cmd.dst_prefx:
			if cmd.cmd_type=="load":
				return tbl
			else:
				return trace_table(cmd.src_prefx, cmd_list)
	ioerror("Cannot trace table: "+tbl)




def call_read_filtering(tbl_out, tbl_in, products_dir, back_end_dir, indx_file, c_file):
		valid_dst=products_dir+'/'+tbl_out+'.vld'
		valid_in=products_dir+'/'+tbl_in+'.vld'
		cf=back_end_dir+'/%s.%s.%s'%(tbl_out, tbl_in, 'isvalid.c')
		target_cf=back_end_dir+'/isvalid.c'
		deflt_cf=back_end_dir+'/default.isvalid.c'
		if c_file.count(cf)>0:
			print 'cp %s %s'%(cf, target_cf)
		else:
			print 'cp %s %s'%(deflt_cf, target_cf)
		print 'make'

		try: #check if valid_in exists
			tstf=open(valid_in)
			tstf.close()
		except:
			valid_in=''
		print '%s/all_tools filter_reads %s %s %s'%(back_end_dir, indx_file, valid_dst, valid_in)

def call_imported_filtering(tbl_out, tbl_in, products_dir, back_end_dir, init_tbl, c_file):
	tbl_indx=products_dir+'/'+init_tbl+'.table'
	valid_dst=products_dir+'/'+tbl_out+'.vld'
	valid_in=products_dir+'/'+tbl_in+'.vld'
	cf=back_end_dir+'/%s.%s.%s'%(tbl_out, tbl_in, 'isvalid_imported.c')
	target_cf=back_end_dir+'/isvalid_imported.c'
	deflt_cf=back_end_dir+'/default.isvalid_imported.c'
	if c_file.count(cf)>0:
		print 'cp %s %s'%(cf, target_cf)
	else:
		print 'cp %s %s'%(deflt_cf, target_cf)
	print 'make'

	try: #check if valid_in exists
		open(valid_in)
	except:
		valid_in=''
	print '%s/all_tools filter_reads %s %s %s'%(back_end_dir, tbl_indx, valid_dst, valid_in)

def call_filter_intervals(back_end_dir, products_dir, tbl_in, tbl_out, c_file):
	dst_valid=products_dir+'/'+tbl_out+'.vld'
	dst_reg=products_dir+'/'+tbl_out+'.reg'
	intrvl=products_dir+'/'+tbl_in+'.intrvl'
	cf=back_end_dir+'/%s.%s.%s'%(tbl_out, tbl_in, 'get_interval_coverage_param.c')
	target_cf=back_end_dir+'/get_interval_coverage_param.c'
	deflt_cf=back_end_dir+'/default.get_interval_coverage_param.c'
	if c_file.count(cf)>0:
		print 'cp %s %s'%(cf, target_cf)
	else:
		print 'cp %s %s'%(deflt_cf, target_cf)
	print 'make'
	print '%s/all_tools filter_intervals %s %s %s'%(back_end_dir, intrvl, dst_valid, dst_reg)

#tbl1, tbl2 are of the form xxx.*.READS or xxx.*.genes where the first part shows the table name
#and the second part shows the origin.
def call_filter_join(back_end_dir, products_dir, joined_table, tbl1, tbl2, tbl_out, read_indx, c_file):
	dst_valid=products_dir+'/'+tbl_out+'.join'
	join_indx=products_dir+'/'+joined_table+'.join'
	tbl1_name=tbl1.split('.')[0]
	type1=get_table_type(tbl1_name)
	if type1=='reads': 
		indx1=read_indx
	else:
		indx1=products_dir+'/'+tbl1_name+'.table'
	tbl2_name=tbl2.split('.')[0]
	type2=get_table_type(tbl2_name)
	if type2=='reads': 
		indx2=read_indx
	else:
		indx2=products_dir+'/'+tbl2_name+'.table'
	cf=back_end_dir+'/%s.%s.%s'%(tbl_out, joined_table, 'isvalid_join.c')
	target_cf=back_end_dir+'/isvalid_join.c'
	deflt_cf=back_end_dir+'/default.isvalid_join.c'
	if c_file.count(cf)>0:
		print 'cp %s %s'%(cf, target_cf)
	else:
		print 'cp %s %s'%(deflt_cf, target_cf)
	print 'make'
	print '%s/all_tools filter_join %s %s %s %s %s %s'%(back_end_dir, join_indx, indx1, indx2, dst_valid, type1, type2)

def call_mapjoin(back_end_dir, products_dir, in1, in2, dst):
	intrvl1=products_dir+'/'+in1+'.intrvl'
	intrvl2=products_dir+'/'+in2+'.intrvl'
	dst_valid=products_dir+'/'+dst+'.join'
	print '%s/all_tools mapjoin %s %s %s'%(back_end_dir, intrvl1, intrvl2, dst_valid)

#side is either 1 or 2. The function returns the call string instead of printing
#it to stdout
def get_isolate_join_outpt(join_table, dst_table, side, back_end_dir, products_dir):
	join_table=products_dir+'/'+join_table+'.join'
	dst_valid=products_dir+'/'+dst_table+'@%d.vld'%side
	return '%s/all_tools isolate_join %s %d %s\n'%(back_end_dir,join_table, side, dst_valid)

#tbl_in: source of valid lst
#tbl_dst: interval table
#target_tbl: the return table of the current sql statement (i.e. target_tbl=select *....)
def call_reads2intervals(back_end_dir, products_dir, indx_file, tbl_in, tbl_dst,target_tbl, c_file):
	valid=products_dir+'/'+tbl_in+'.vld'
	dst=products_dir+'/'+tbl_dst+'.intrvl'
	cf=back_end_dir+'/%s.%s.%s'%(target_tbl, tbl_in, 'get_rd_interval.c')
	target_cf=back_end_dir+'/get_rd_interval.c'
	deflt_cf=back_end_dir+'/default.get_rd_interval.c'
	if c_file.count(cf)>0:
		print 'cp %s %s'%(cf, target_cf)
	else:
		print 'cp %s %s'%(deflt_cf, target_cf)
	print 'make'
	print '%s/all_tools reads2intervals %s %s %s'%(back_end_dir, indx_file, valid, dst)

#tbl_in=source of valid lst
#init_table=the imported table of interest
#tbl_dst: interval table
#target_tbl: the return table of the current sql statement (i.e. target_tbl=select *....)
def call_imported2intervals(back_end_dir, products_dir, tbl_in, init_table, tbl_dst, target_tbl, c_file):
	tbl_indx=products_dir+'/'+init_table+'.table'
	dst=products_dir+'/'+tbl_dst+'.intrvl'
	valid=products_dir+'/'+tbl_in+'.vld'
	cf=back_end_dir+'/%s.%s.%s'%(target_tbl, tbl_in, 'get_imported_interval.c')
	target_cf=back_end_dir+'/get_imported_interval.c'
	deflt_cf=back_end_dir+'/default.get_imported_interval.c'
	if c_file.count(cf)>0:
		print 'cp %s %s'%(cf, target_cf)
	else:
		print 'cp %s %s'%(deflt_cf, target_cf)
	print 'make'
	try: #check if valid_in exists
		tstf=open(valid_in)
		tstf.close()
	except:
		valid=''
	print '%s/all_tools imported2intervals %s %s %s'%(back_end_dir, tbl_indx, dst, valid)


def output_imported(tbl_in, init_table, tbl_out, back_end_dir, products_dir):
	tbl_indx=products_dir+'/'+init_table+'.table'
	valid_in=products_dir+'/'+tbl_in+'.vld'
	outfile=products_dir+'/'+tbl_in+'.txt'
	cmd='%s/all_tools print_imported %s %s %s\n'%(back_end_dir, tbl_indx, valid_in, outfile)
	return cmd

#it returns the command string
def output_reads(tbl_in, tbl_out, products_dir, back_end_dir, indx_file, bam_file, chromo_len):
	valid_file=products_dir+'/'+tbl_in+'.vld'
	dest_file=products_dir+'/'+tbl_out+'.bam'
	dest_reduced=products_dir+'/'+tbl_out+'.bam.short'
	hist_file=products_dir+'/'+tbl_out+'.hist'
	mates_en=tables[tbl_in.split('@')[0]].mates_en
	cmd= '%s/all_tools print_reads %s %s %s %s %s %s %d %d\n'%(back_end_dir, bam_file, indx_file, valid_file, dest_file, dest_reduced, hist_file, mates_en, chromo_len)
	return cmd

#it prints the data of the two parts of the joined table tbl_name
def get_join_output(tbl_name, products_dir, back_end_dir, indx_file, bam_file, chromo_len, type1, type2):
	newtable=tables[tbl_name]
	cmd=''
	cmd+=get_isolate_join_outpt(tbl_name, tbl_name, 1, back_end_dir, products_dir)
	cmd+=get_isolate_join_outpt(tbl_name, tbl_name, 2, back_end_dir, products_dir)
	if type1=='reads':
		cmd+=output_reads(tbl_name+'@1', tbl_name+'@1', products_dir, back_end_dir, indx_file, bam_file, chromo_len)
	elif type1=='imported':
		init_tbl=newtable.origin_path[0].split('.')[-1]
		cmd+=output_imported(tbl_name+'@1', init_tbl, tbl_name+'@1', back_end_dir, products_dir)
	if type2=='reads':
		cmd+=output_reads(tbl_name+'@2', tbl_name+'@2', products_dir, back_end_dir, indx_file, bam_file, chromo_len)
	elif type2=='imported':
		init_tbl=newtable.origin_path[0].split('.')[-2]
		cmd+=output_imported(tbl_name+'@2', init_tbl, tbl_name+'@2', back_end_dir, products_dir)
	return cmd

#it looks up the origin paths of the inputs of tbl_name which are of the forms A.B.C.* and returns
#the first non common field of both
def get_join_inputs(tbl_name):
	cur_in=tables[tbl_name].origin_path
	in1=cur_in[0]
	in2=cur_in[1]
	spin1=in1.split('.')
	spin2=in2.split('.')
	for i in range(min(len(spin1), len(spin2))):
		if spin1[i]!=spin2[i]:
			return spin1[i],spin2[i]
	ioerror('Cannot get the inputs of %s'%tbl_name)

#it returns either 'reads' or 'imported' or mapjoin
def get_table_type(tbl_name):
	cur_in=tables[tbl_name].origin_path
	if(len(cur_in)>1):
		return 'mapjoin'
	elif (len(cur_in)==0):
		if tbl_name=='READS':
			return 'reads'
		else:
			return 'imported'
	elif cur_in[0].split('.')[-1]=='READS':
		return 'reads'
	else:
		return 'imported'

#it looks up origin paths to find an entry that looks like
#tbl_in.H2.init_table and returns init_table. if origin_paths
#is empty, the function returns tbl_in
def get_init_imported_name(tbl_in, origin_paths):
	init_table=''
	if origin_paths==[]:
		return tbl_in
	for orig in origin_paths:
		if tbl_in==orig.split('.')[0]:
			init_table=orig.split('.')[-1]
			break
	return init_table

def genomequery_calls(newtable, tbl_name, c_files, cur_input, back_end_dir, products_dir, bam_file, indx_file, chromo, chromo_len):
	if len(newtable.filtering_cmd)>1: ioerror("Too many filtering commands")
	if newtable.filtering_cmd.count("read_filtering")>0: #case of read filtering
		#if newtable.kind=='READS' and tables[cur_input[-1]].kind=='READS': #case of read filtering
		call_read_filtering(tbl_name, cur_input[-1], products_dir, back_end_dir, indx_file, c_files)
		output_cmd=output_reads(tbl_name, tbl_name, products_dir, back_end_dir, indx_file, bam_file, chromo_len)
	elif newtable.filtering_cmd.count("import_filtering")>0: #case of import_filtering
		#elif newtable.kind=='imported' and tables[cur_input[-1]].kind=='imported':
		init_tbl=newtable.origin_path[-1].split('.')[-1]
		call_imported_filtering(tbl_name, cur_input[-1], products_dir, back_end_dir, init_tbl, c_files)
		output_cmd=output_imported(cur_input[-1], init_tbl, tbl_name, back_end_dir, products_dir)
	elif newtable.filtering_cmd.count('interval_filtering')>0: # and len(cur_input)==1: #case of filter_interval
		call_filter_intervals(back_end_dir, products_dir, cur_input[-1], tbl_name, c_files)
		type1=get_table_type(cur_input[-1])
		if type1=='reads':
			output_cmd=output_reads(cur_input[-1], tbl_name, products_dir, back_end_dir, indx_file, bam_file, chromo_len)
		elif type1=='imported':
			init_tbl=newtable.origin_path[-1].split('.')[-1]
			output_cmd=output_imported(cur_input[-1], init_tbl, tbl_name, back_end_dir, products_dir)
		else:
			ioerror('Unknown type of interval origin')
	elif newtable.filtering_cmd.count('join_filtering')>0:
	#elif newtable.kind=='mapjoin' and len(cur_input)==1: #case of join filtering
		tbl1,tbl2=get_join_inputs(tbl_name)
		call_filter_join(back_end_dir, products_dir, cur_input[0], tbl1, tbl2, tbl_name, indx_file, c_files)
		type1=get_table_type(tbl1)
		type2=get_table_type(tbl2)
		output_cmd=get_join_output(tbl_name, products_dir, back_end_dir, indx_file, bam_file, chromo_len, type1, type2)

	if newtable.kind=='interval' and tables[cur_input[-1]].kind=='READS': #case of rd2interval
		call_reads2intervals(back_end_dir, products_dir, indx_file, tbl_name, tbl_name, tbl_name, c_files)
	elif newtable.kind=='interval' and tables[cur_input[-1]].kind=='imported': #case of imported2interval
		tbl_in=cur_input[-1]
		init_table=get_init_imported_name(tbl_in, newtable.origin_path)
		call_imported2intervals(back_end_dir, products_dir, tbl_in, init_table, tbl_name, tbl_name, c_files)

	if newtable.kind=='mapjoin' and len(cur_input)==2: #case of mapjoin
		in1=cur_input[0]
		in2=cur_input[1]
		if tables[in1].kind=='READS':
			call_reads2intervals(back_end_dir, products_dir, indx_file, in1, in1, tbl_name, c_files)
			type1='reads'
		elif tables[in1].kind=='imported':
			tbl_in=in1
			init_table=get_init_imported_name(tbl_in, tables[tbl_in].origin_path)
			call_imported2intervals(back_end_dir, products_dir, tbl_in, init_table, tbl_in, tbl_name, c_files)
			type1='imported'
		elif tables[in1].kind=='interval':
			type1=get_table_type(in1)
		else:
			ioerror('Table %s is not mapjoinable'%in1)
		if tables[in2].kind=='READS':
			call_reads2intervals(back_end_dir, products_dir, indx_file, in2, in2, tbl_name, c_files)
			type2='reads'
		elif tables[in2].kind=='imported':
			tbl_in=in2
			init_table=''
			for orig in tables[in2].origin_paths:
				if tbl_in==orig.split('.')[0]:
					init_table=orig.split('.')[-1]
					break
			call_imported2intervals(back_end_dir, products_dir, tbl_in, init_table, tbl_in, tbl_name, c_files)
			type2='imported'
		elif tables[in2].kind=='interval':
			type2=get_table_type(in2)
		else: ioerror('Table %s is not mapjoinable'%in2)
		call_mapjoin(back_end_dir, products_dir, in1, in2, tbl_name)
		output_cmd=get_join_output(tbl_name, products_dir, back_end_dir, indx_file, bam_file, chromo_len, type1, type2)

		
	if tbl_name=='out':
		if output_cmd!='':
			print output_cmd
		else:
			print '!!!!Nothing to print'

#def update_names(newtable, tbl_name):
def update_input_paths(cur_input_paths, input_name):
	for orig in tables[input_name].origin_path:
		cur_input_paths.append('%s.%s'%(input_name, orig))
	if tables[input_name].origin_path==[]:
		cur_input_paths.append('%s'%(input_name))
	return

#the function decides the type and the filename.c of the interval command. The
#interval commands that we currently support are read_interval and 
#imported_interval. If cur_input_paths contain more than one elements, we 
#consider only the last one.
def get_interval_cmd(cur_input_paths):
	if cur_input_paths==[]:
		ioerror("No input files found")
	elif len(cur_input_paths)==1:
		cip=cur_input_paths[0]
	else:
		cip=cur_input_paths[-1]
		for c in cur_input_paths[:-1]:
			if c.split('.')[0]==cip.split('.')[0]:
				ioerror("Cannot support intervals on joined tables")
	cip=cur_input_paths[-1]
	if cip.split('.')[-1]=='READS':
		return 'get_rd_interval.c','read_interval'
	else:
		return 'get_imported_interval.c', 'imported_interval'

#the function decides the type and the filename.c of the filtering command. The
#filtering commands that we currently support are read_filtering, 
#import_filtering, interval_filtering, and join_filtering. 
#If cur_input_paths contain more than one elements, we 
#consider only the last one except for the case of join_filtering
def get_filtering_cmd(cur_input_paths):
	tmp=cur_input_paths
	if tmp==[]:
		ioerror("No input files found")
	elif len(tmp)==1:
		cip=tmp[0]
		inpt_name=cip.split('.')[0]
		origin=cip.split('.')[-1]
		#print 'inpt: ', inpt_name
		#print 'instance: ', tables[inpt_name].kind
		kind=tables[inpt_name].kind
		if kind=='READS' and origin=='READS':
			return 'isvalid.c','read_filtering'
		elif kind=='imported':
			return 'isvalid_imported.c', 'import_filtering'
		elif kind=='interval':
			return 'isvalid_interval.c', 'interval_filtering'
	else:
		cip=tmp[-1]
		inpt_name=cip.split('.')[0]
		origin=cip.split('.')[-1]
		kind=tables[inpt_name].kind
		if kind=='mapjoin':
			return 'isvalid_join.c', 'join_filtering'
		else:
			ioerror('Don\'t know what filtering you are talking about')

#it changes filenames from dir/tmpc.* to dir/tbl_name.*
def update_names(tbl_name, c_files):
	for i in range(len(c_files)):
		old=c_files[i]
		lc=old.rfind('tmpc.')
		newfile=old[:lc]+tbl_name+old[lc+4:]
		rename(old, newfile)
		c_files[i]=newfile
	return

#cur_input is a set of table names, e.g. [H2, READS, H3]. If
#any of the input tables have mates_en==1 the function returns 1.
def get_mates_en(cur_input):
	has_value=False
	for c in cur_input:
		if tables[c].mates_en==1:
			return 1
		elif tables[c].mates_en==0:
			has_value=True
	if has_value:
		return 0
	else:
		return None

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
	lines=f.readlines()
	if lines[0].count('Compilation completed without errors')==0:
		ioerror('Cannot create low level calls due to compilation errors')

	i=1
	cur_input=[]
	cur_input_paths=[]
	newtable=Table() #(None, 0)
	c_files=[]
	while i<len(lines):
		sp_line=lines[i].split()
		if sp_line[1]=='load_table':
			#i=load_genes(lines,i)
			i=get_loaded_fields(lines, i+1, sp_line[2], src_table_dir, products_dir, back_end_dir, chr_name)
			continue
		elif sp_line[1]=='Input':
			cur_input.append(sp_line[3])
			update_input_paths(cur_input_paths,sp_line[3])
		elif sp_line[1]=='begin_interval_spec':
			if lines[i+1].count('N/A')==0:
				intrvl_file, intrvl_cmd=get_interval_cmd(cur_input_paths)
				intrvl_file=back_end_dir+'/'+'tmpc.'+cur_input[-1]+'.'+intrvl_file
			else:
				intrvl_file, intrvl_cmd='nope', 'nope'
			#print 'file, cmd ',intrvl_file, intrvl_cmd
			status,i,mates_en=produce_code(lines, i+1, intrvl_file, cur_input_paths, intrvl_cmd)
			if newtable.mates_en!=1: newtable.mates_en=mates_en
			if(status>0): c_files.append(intrvl_file)
		elif sp_line[1]=='begin_filtering_code':
			if lines[i+1].count('N/A')==0:
				flt_file, flt_cmd=get_filtering_cmd(cur_input_paths)
				#print 'file cmd',flt_file, flt_cmd
				flt_file=back_end_dir+'/'+'tmpc.'+cur_input[-1]+'.'+flt_file
			else:
				flt_file, flt_cmd='nope', 'nope'
			#print 'file, cmd ',flt_file, flt_cmd
			status,i,mates_en=produce_code(lines, i+1, flt_file, cur_input_paths, flt_cmd)
			if newtable.mates_en!=1: newtable.mates_en=mates_en
			if(status>0): 
				c_files.append(flt_file)
				newtable.filtering_cmd.append(flt_cmd)
		elif sp_line[1]=='mapjoin':
			newtable.kind='mapjoin'
			if newtable.mates_en!=1: newtable.mates_en=get_mates_en(cur_input)
		elif sp_line[1]=='return_arg':
			if newtable.mates_en!=1: newtable.mates_en=get_mates_en(cur_input)
			if sp_line[3]=='strength_vector':
				newtable.kind='interval'
			elif newtable.kind=='regular': #i.e no mapjoin here
				newtable.kind=tables[cur_input[-1]].kind
		elif sp_line[1]=='result_table':
			tbl_name=sp_line[3]
			update_names(tbl_name, c_files)
			newtable.origin_path=deepcopy(cur_input_paths)
			newtable.c_files=deepcopy(c_files)
			tables[tbl_name]=deepcopy(newtable)
			genomequery_calls(newtable, tbl_name, c_files, cur_input, back_end_dir, products_dir, bam_file, indx_file, chr_name, chromo_length)
			newtable=Table() #(None, 0)
			cur_input=[]
			cur_input_paths=[]
			c_files=[]
		else:
			pass
			#ioerror('Cant parse line: %s'%lines[i][:-1])
		i+=1
	###produce calls for the final "out" table
	tbl_name='out'
	update_names(tbl_name, c_files)
	newtable.origin_path=deepcopy(cur_input_paths)
	newtable.c_files=deepcopy(c_files)
	if newtable.mates_en!=1: newtable.mates_en=get_mates_en(cur_input)
	tables[tbl_name]=deepcopy(newtable)
	genomequery_calls(newtable, tbl_name, c_files, cur_input, back_end_dir, products_dir, bam_file, indx_file, chr_name, chromo_length)
	cur_input=[]
	cur_input_paths=[]






if __name__=="__main__":
	main()

	
