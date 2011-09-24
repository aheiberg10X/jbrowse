from string import split, atoi
from sys import exit
from copy import copy, deepcopy
import sys

class Command:
    def __init__(self, dep_graph, src, dst, cmd_type, line_st, line_nd, limit, direction):
        self.dep_graph=dep_graph #the dependency graph
        self.src_prefx=src #the source prefix
        self.dst_prefx=dst #the destination prefix
        self.cmd_type=cmd_type #mates vs single_coverage vs single_range vs join vs load vs filter_table
        self.line_st=line_st #first line of the command
        self.line_nd=line_nd #last line of the command
        self.limit=limit #used for clone/read lime
        self.direction=direction #used to decide between upper/lower limit
        self.cmd_string='' #used by join cases to get easy access to the original intersect command

def ioerror(msg):
    print >>sys.stderr, msg
    exit(1)

def open_file(fname):
    try:
        f=open(fname)
    except:
        ioerror("cannot open file "+fname)
    return f

#g is a dependency graph. It adds an edge from st_node (string) to each of 
#the vertixes of the list nd_nodes.
def update_dependency_graph(g, st_node, nd_nodes):
    if st_node not in g:
        g[st_node]=[]
    for n in nd_nodes:
        if n not in g: #if the node has not been encountered so far
            g[n]=[]
        if n not in g[st_node]:
            g[st_node].append(n)
    return

#if there is at least one mate variable, we are talking about mate pair mode.
#Otherwise we have a single mode
def mate_vs_single(g):
    for vrt in g:
        if vrt.count("mate_",0)>0:
            return "mate"
    return "single"

#it translates several words into proper c representation
def translate(word):
    if word=="length":
        return "indx[i].read_len"
    elif word=="location":
        return "indx[i].loc"
    elif word=="strand":
        return "get_strand(strand_indx, strand_len, i)"
    elif word=="mate_loc":
        return "indx[mate_indx].loc"
    elif word=="mate_length":
        return "indx[mate_indx].read_len"
    elif word=="chr":
        return "indx[i].chr"
    else:
        return word

#it translates several words into proper c representation. Need to be used exclusively for the creation of isvalid_join.c
def translate4join(word, type1, type2):
    if type1.count('bam')>0:
        ibam=1
        iimp=2
    elif type2.count('bam')>0:
        ibam=2
        iimp=1
    else:
        ioerror('Cannot generate join filter code for two tables of the same type')
    if word=="length":
        return "indx%d.mindx[i%d].read_len"%(ibam,ibam)
    elif word=="location":
        return "indx%d.mindx[i%d].loc"%(ibam,ibam)
    elif word=="strand":
        return "get_strand(strand_indx, strand_len, i%d)"%(ibam)
    elif word=="mate_loc":
        return "indx%d.mindx[mate_indx].loc"%ibam
    elif word=="mate_length":
        return "indx%d.mindx[mate_indx].read_len"%ibam
    elif word=="chr":
        return "indx%d.tbl[i%d].chr"%(iimp,iimp)
    elif word=="start":
        return "indx%d.tbl[i%d].start"%(iimp,iimp)    
    elif word=="end":
        return "indx%d.tbl[i%d].end"%(iimp,iimp)
    elif word=="id":
        return "indx%d.tbl[i%d].id"%(iimp,iimp)
    else:
        return word

#if there is a path from vrtx to dest it returns True. Otherwise False
def check_dependency(g, vrtx, dest):
    if g[vrtx]==[]:
        return False
    if g[vrtx].count(dest)>=1:
        return True
    for t in g[vrtx]:
        if check_dependency(g, t, dest):
            return True
    return False

#it produces isvalid.c. g is the dependency graph. 
#Codelines are the set of the lines of the compiler 
#bytecode code. The lines that need to be used by the function are between
#code_st and code_nd
def produce_isvalid(g, dest_fname, codelines, code_st, code_nd):
    if dest_fname=='':
        return
    outfp=open(dest_fname, 'w')
    print >> outfp,"#include<stdio.h>\n#include\"all_tools.h\""
    if dest_fname.count('imported')>0:
        print >> outfp, "inline int isvalid_imported(Imported_info *indx, int i, int ttl_table){"
        print >> outfp, "if (i<0 || i>=ttl_table) ioerror(\"Isvalid call is out of bounds\");"
    elif dest_fname.count('single')>0:
        print >> outfp,"inline int is_valid_single(Mates *indx, int i, long *strand_indx, int strand_len){"
    else:
        print >> outfp,"inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){"
        print >> outfp, "\tint mate_indx=indx[i].mate_indx;"
    ret_str="\treturn 1;"
    for i in range(code_st, code_nd):
        line=codelines[i][:-1]
        sp_line=line.split(" ")
        for word in sp_line:
            if word==sp_line[0]:
                continue
            if word not in g: #ignore =,>,< etc
                continue
            if check_dependency(g, word, "countvec"):
                break
        else: #add the line in isvalid.c
            print >> outfp, "\tint %s;"%(sp_line[1])
            str=sp_line[1]+" "+sp_line[2]+" "
            ret_str="\treturn "+sp_line[1]+";"
            for j in range(3, len(sp_line)):
                str+=translate(sp_line[j])
            print >> outfp, "\t",str,";"
    print >> outfp, ret_str
    print >> outfp, "}"

#it produces isvalid_join.c. g is the dependency graph. 
#Codelines are the set of the lines of the compiler 
#bytecode code. The lines that need to be used by the function are between
#code_st and code_nd. type1, type2 are the "type" arguments of the join.
def produce_isvalid_join(g, dest_fname, codelines, code_st, code_nd, type1, type2):
    if dest_fname=='':
        return
    outfp=open(dest_fname, 'w')
    print >> outfp,"#include<stdio.h>\n#include\"all_tools.h\""
    print >> outfp,"inline int isvalid_join(Join *join_lst, int i, Imported_info *tbl1, Imported_info *tbl2, Mates *mindx, long *strand_indx, int strand_len){"
    print >> outfp, "\tunion IU {\n\t\tImported_info *tbl;\n\t\tMates *mindx;\n\t};" #declare the union
    print >> outfp, "\tunion IU indx1, indx2;"
    print >> outfp, "\tint i1=join_lst[i].indx1;\n\tint i2=join_lst[i].indx2;\n\tint mate_indx;"
    print >> outfp, "\tif(tbl1!=NULL)\n\t\tindx1.tbl=tbl1;\n\telse{\n\t\tindx1.mindx=mindx;\n\t\tmate_indx=indx1.mindx[i1].mate_indx;\n\t}"
    print >> outfp, "\tif(tbl2!=NULL)\n\t\tindx2.tbl=tbl2;\n\telse{\n\t\tindx2.mindx=mindx;\n\t\tmate_indx=indx2.mindx[i2].mate_indx;\n\t}"
    
    ret_str="\treturn 1;"
    for i in range(code_st, code_nd):
        line=codelines[i][:-1]
        sp_line=line.split(" ")
        for word in sp_line:
            if word==sp_line[0]:
                continue
            if word not in g: #ignore =,>,< etc
                continue
            if check_dependency(g, word, "countvec"):
                break
        else: #add the line in isvalid.c
            print >> outfp, "\tint %s;"%(sp_line[1])
            str=sp_line[1]+" "+sp_line[2]+" "
            ret_str="\treturn "+sp_line[1]+";"
            for j in range(3, len(sp_line)):
                str+=translate4join(sp_line[j], type1, type2)
            print >> outfp, "\t",str,";"
    print >> outfp, ret_str
    print >> outfp, "}"

#it gets the type of that entry of the list whose dest_prefx is s
def get_earlier_cmd_type(lst, s):
    for l in lst:
        if l.dst_prefx==s:
            if l.cmd_type=="load":
                return "filter_table"
            else:
                return l.cmd_type
    ioerror("Cannot find matching cmd_type")

#it parses the load arguments and figures the locations of id, chr, start, and end
def    get_loaded_fields(codelines, code_st, code_nd):
    id_col=-1
    chr_col=-1
    st_col=-1
    nd_col=-1

    cur_col=1

    for i in range(code_st, code_nd):
        line=codelines[i][:-1]
        sp_line=line.split(" ")
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
    if (id_col<0 or chr_col<0 or st_col<0 or nd_col<0):
        ioerror("Cannot load table because an important parameter is missing")
    return id_col, chr_col, st_col, nd_col

  




#it returns a list of commands
def parse_commands(lines):
    ret=[]
    g={} #the dependency graph of each command
    cmd_st=0
    src_prefx='in'
    limit=0
    direction=">"
    intersect_en=False
    if lines[0][:-1]!="Compilation completed without errors":
        return []
    for i in range(1,len(lines)):
        sp_line=lines[i][:-1].split(" ")
        if sp_line[1]=="load_table":
            cmd_st=i+1
            src_prefx=sp_line[2]
            src_dest=sp_line[2]
            cmd_type="load"
        elif sp_line[1]=="loaded_param":
            cmd_nd=i
        elif sp_line[1]=="end_load_table":
            cmd_nd=i
            cmd=Command({}, src_prefx, src_dest, cmd_type, cmd_st, cmd_nd, -1, -1)
            ret.append(cmd)
            src_prefx='in'
        elif sp_line[1]=="begin_intersect":
            cmd_st=i+1
            cmd_type="intersect"
            intersect_en=True
        elif sp_line[1]=="end_intersect":
            cmd_nd=i
            intersect_en=False
            src_prefx=''
            limit=-1
            direction=-1
            if i==len(lines)-1: #copy paste from below
                dst_prefx="out"
                cmd_nd=i
                cmd=Command(deepcopy(g), src_prefx, dst_prefx, cmd_type, cmd_st, cmd_nd, limit, direction)
                ret.append(cmd)
                g={}

            #cmd=Command({}, '', '', cmd_type, cmd_st, cmd_nd, -1, -1)
            #ret.append(cmd)
        elif sp_line[1]=="Input":
            if intersect_en:
                continue
            if sp_line[3]=="READS":
                src_prefx='in'
            if sp_line[3]!="READS":
                if src_prefx=='in':
                    src_prefx=sp_line[3]
                else: ###############lookup sp_line[3] and if it is a product of join proceed... else ioerror
                    src_prefx=sp_line[3]
                    cmd_type=get_earlier_cmd_type(ret, src_prefx)
                    if cmd_type=="join" or cmd_type=="intersect":
                        cmd_type="join"
                    else:
                        ioerror("Cannot handle multiple sources at this point")
            cmd_st=i+1 #the first code line of the form t0=loc<1000
        elif sp_line[1]=="result_table" or i==len(lines)-1:
            if sp_line[1]=="result_table":
                dst_prefx=sp_line[3]
            else:
                dst_prefx="out"
                cmd_nd=i
            cmd=Command(deepcopy(g), src_prefx, dst_prefx, cmd_type, cmd_st, cmd_nd, limit, direction)
            ret.append(cmd)
            g={}
        elif sp_line[1]=="return_arg": #decide the type of the command
            cmd_nd=i
            if mate_vs_single(g)=="mate":
                if sp_line[3]=="strength_vector":
                    cmd_type="mate"
                else:
                    print "line[3]: ",sp_line[3]
                    ioerror("Can't understand the mate command")
            elif sp_line[3]=="strength_vector":
                cmd_type="single_coverage"
            elif src_prefx!="in":
                cmd_type=get_earlier_cmd_type(ret, src_prefx)##same cmd type as the one that produced the products of src_prefx
            else:
                cmd_type="single_range"
        else: #sth like t0=t1<23
            st_node=sp_line[1]
            nd_nodes=[]
            #print lines[i]
            if not sp_line[3].isdigit():
                nd_nodes.append(sp_line[3])
            if not sp_line[5].isdigit():
                nd_nodes.append(sp_line[5])
            update_dependency_graph(g, st_node, nd_nodes)
            if sp_line[3]=="countvec":
                limit=atoi(sp_line[5])
                direction=sp_line[4][0] #for now treat the same >=,> and <=,<
    return ret


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


#it parses up to 2 Input commands and returns the cmd_string for the intersection command
def get_intersect_info(codelines, cmd, cmd_list, bam_file, indx_file, product_dir, back_end_dir, chromo_length):
    sp_line=codelines[cmd.line_st].split(" ")
    if sp_line[1]!="Input":
        ioerror("Input command expected for the intersection")
    t1=sp_line[3]
    in_prefx_1=product_dir+'/'+t1
    origin1=trace_table(t1, cmd_list)
    if origin1=="in": #if origin1=="READS":
        indx_file1=indx_file
        if t1=='READS' or t1=='in':
            etype='bam_single'
        else:
            etype=get_earlier_cmd_type(cmd_lst, t1)
        if etype=="mates":
            type1="bam_mates"
        elif etype.count("single")>0:
            type1="bam_single"
        else:
            ioerror("Unknown origin of READ product")
    else:
        type1="imported"
        indx_file1=product_dir+'/'+origin1+".table"

    
    sp_line=codelines[cmd.line_st+1].split(" ")
    if sp_line[1]!="Input":
        ioerror("Input command expected for the intersection")
    t2=sp_line[3]
    in_prefx_2=product_dir+'/'+t2
    origin2=trace_table(t2, cmd_list)
    if origin2=="in": #if origin2=="READS":
        indx_file2=indx_file
        if (t2=='in' or t2=='READS'):
            etype="bam_single"
        else:
            etype=get_earlier_cmd_type(cmd_list, t2)
        if etype=="mates":
            type2="bam_mates"
        elif etype.count("single")>0:
            type2="bam_single"
        else:
            ioerror("Unknown origin of READ product")
    else:
        type2="imported"
        indx_file2=product_dir+'/'+origin2+".table"
    
    if type1=="imported" and type2=="imported":
        mode=3
    elif type1=="imported":
        mode=2
    elif type1=="imported":
        mode=1
    else:
        ioerror("Cannot decide on the mode of the intersection")

    dest_prefx=product_dir+"/"+cmd.dst_prefx
    
    cmd_str="%s/all_tools intersect %s %s %s %s %s %s %d %d %s %s dummy"%(back_end_dir, indx_file1, type1, indx_file2, type2, bam_file, dest_prefx, mode, chromo_length, in_prefx_1, in_prefx_2)
    cmd.cmd_string=cmd_str
    return cmd_str

#it returns the command string as well type1 and type2
def get_join_info(cmd, cmd_lst,product_dir):
    #step 1: find the command whose dst_prefx==cmd.src_prefx
    pcmd=None
    for c in cmd_lst:
        if c.dst_prefx==cmd.src_prefx:
            pcmd=c
            break
    #step2: obtain back_end_dir, indx_file1, type1, indx_file2, type2, bam_file, dest_prefx, mode, chromo_length from pcmd.cmd_string
    sp_pcmd=pcmd.cmd_string.split(" ")
    program_loc=sp_pcmd[0]
    indx_file1=sp_pcmd[2]
    type1=sp_pcmd[3]
    indx_file2=sp_pcmd[4]
    type2=sp_pcmd[5]
    bam_file=sp_pcmd[6]
    join_src_prefx=sp_pcmd[7] #cmd.src_prefx #this is a new one here
    mode=-1 #leave this for step3
    chromo_length=sp_pcmd[9]
    dest_prefx=product_dir+"/"+cmd.dst_prefx
    #step3: fix the mode
    if type1.count("bam")>0 and type2.count("imported")>0:
        mode=4
    elif type2.count("bam")>0 and type1.count("imported")>0:
        mode=5
    else:
        ioerror('Cannot currently filter the join of two bams or two imported') #mode=6
    
    #create the cmd_string now
    cmd_str="%s intersect %s %s %s %s %s %s %d %s dummy dummy %s"%(sp_pcmd[0], indx_file1, type1, indx_file2, type2, bam_file, dest_prefx, mode, chromo_length, join_src_prefx)
    cmd.cmd_string=cmd_str
    return cmd_str, type1, type2



def main(argv=sys.argv):
    print argv
    if len(argv)!=10:
        ioerror( "argv: %s" % str(argv) )
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
    cmds=parse_commands(lines)

    cmd_string=''
    for cmd in cmds:
        if cmd.cmd_type=="mate":
            target_valid=cmd.dst_prefx+"isvalid_mates.c"
            if cmd.dst_prefx=="out" and cmd.src_prefx!="in":
                mode=4
            elif cmd.dst_prefx!="out" and cmd.src_prefx=="in":
                mode=3
            else:
                print 'src: %s dst: %s'%(cmd.src_prefx, cmd.dst_prefx)
                ioerror("mates mode currently not supported")
            cmd_string= "%s/all_tools mates %s %s %d %s %d %s/%s %s/%s %d"%(back_end_dir, bam_file, indx_file, chromo_length, chr_name, mode, products_dir,cmd.src_prefx, products_dir, cmd.dst_prefx, cmd.limit)
        elif cmd.cmd_type=="single_coverage":
            if cmd.direction==">":
                up_low="low"
            else:
                up_low="up"
            if cmd.dst_prefx=="out" and cmd.src_prefx!="in":
                mode=4
            elif cmd.dst_prefx!="out" and cmd.src_prefx=="in":
                mode=3
            else:
                "mates case currently not supported"
            target_valid=cmd.dst_prefx+"isvalid_single.c"
            cmd_string="%s/all_tools single_coverage %s %s %s %d %d %s %d %s/%s %s/%s"%(back_end_dir, up_low, bam_file, indx_file, cmd.limit, chromo_length, chr_name, mode, products_dir, cmd.src_prefx, products_dir, cmd.dst_prefx)

        elif cmd.cmd_type=="load":
            id_col, chr_col, st_col, nd_col=get_loaded_fields(lines, cmd.line_st, cmd.line_nd)
            table_src=src_table_dir+"/"+cmd.src_prefx+".txt"
            table_dest_prefx=products_dir+"/"+cmd.dst_prefx
            cmd_string="%s/all_tools load_table %s %s %d %d %d %d %s"%(back_end_dir, table_src, table_dest_prefx, id_col, chr_col, st_col, nd_col, chr_name)
            target_valid=''

        elif cmd.cmd_type=="filter_table":
            table_src=products_dir+"/"+cmd.src_prefx
            table_dst=products_dir+"/"+cmd.dst_prefx
            target_valid=cmd.dst_prefx+"isvalid_imported.c"
            cmd_string="%s/all_tools filter_imported %s %s"%(back_end_dir, table_src, table_dst)

        elif cmd.cmd_type=="intersect":
            target_valid=''
            cmd_string=get_intersect_info(lines, cmd, cmds, bam_file, indx_file, products_dir, back_end_dir, chromo_length)
        elif cmd.cmd_type=="join":
            target_valid=cmd.dst_prefx+"isvalid_join.c"
            cmd_string, type1, type2=get_join_info(cmd, cmds, products_dir)
    
        elif cmd.cmd_type=="single_range":
            if cmd.dst_prefx=="out" and cmd.src_prefx!="in":
                mode=2
            elif cmd.dst_prefx!="out" and cmd.src_prefx=="in":
                mode=0 #mode=1, but we usually need the bam as well
            elif cmd.dst_prefx=="out" and cmd.src_prefx=="in":
                mode=0
            else:
                "no other single range mode is transported"
            target_valid=cmd.dst_prefx+"isvalid_single.c"
            dst_prefx=products_dir+"/"+cmd.dst_prefx
            cmd_string="%s/all_tools single_range %s %s %d %s %s"%(back_end_dir, bam_file, indx_file, chromo_length, dst_prefx, mode)
        else:
            ioerror("Command %s is not supported"%(cmd.cmd_type))
            #ioerror("single read range currently not supported...need to work on efficiency")
        
        if cmd.cmd_type!="join":
            produce_isvalid(cmd.dep_graph, target_valid, lines, cmd.line_st, cmd.line_nd)
        else:
            produce_isvalid_join(cmd.dep_graph, target_valid, lines, cmd.line_st, cmd.line_nd, type1, type2)

        if target_valid.count('mates')>0:
            print "cp %s %s/isvalid.c"%(target_valid,back_end_dir)
        elif target_valid.count('imported')>0:
            print "cp %s %s/isvalid_imported.c"%(target_valid, back_end_dir)
        elif target_valid.count('single')>0:
            print "cp %s %s/isvalid_single.c"%(target_valid,back_end_dir)
        elif target_valid.count('join')>0:
            print "cp %s %s/isvalid_join.c"%(target_valid,back_end_dir)    
        else:
            pass
        if target_valid!='':
            print "make"
        print cmd_string
    f.close()

if __name__=="__main__":
    main()









    
