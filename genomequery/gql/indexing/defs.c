#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "defs.h"


void ioerror(char *msg){
	fprintf(stderr, "FATAL ERROR!!! %s\n",msg);
	exit(1);
}

FILE *open_file(char *fname){
	FILE *fp=fopen(fname, "r");
	if (fp==NULL){
		char buf[1024];
		strcpy(buf, "Cannot open file ");
		strcat(buf, fname);
		ioerror(buf);
	}
	return fp;
}

//It parses a line of a solexa file and it returns
//the respective Read variable.
Read parse_solexa_read(char *str){
	char *tok, *tmp;
	char *tok_sv;
	Read ret;
	int i=0;
	ret.name=str;
	tok=strtok_r(str,"\t", &tok_sv);
	for(i=1;i<=8;i++) tok=strtok_r(NULL,"\t", &tok_sv);
	ret.read=tok;
	tmp=ret.name;
	for (i=0;i<5;i++) { //remove the '\0' from the components of the name.
		*(tmp+strlen(tmp))='\t';
	}
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.qvals=tok;
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.chromo=tok;
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.loc=atoi(tok);
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.strand=tok[0];
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.match_descr=tok;
	tok=strtok_r(NULL, "\t", &tok_sv);
	ret.partner_chromo=NULL;
	ret.partner_strand='-';
	ret.partner_loc=-1;
	return ret;
}


//it finds all offsets of dscr
//it returns the total number of bits that are
//required for each instruction.
//Return value: An array of instructions.
//Data of *len will contain the length of that array.
Instr *parse_descriptor(char *rd, char *dscr, int *len, int rd_length){
	int *offst=(int*)malloc(sizeof(int)*(rd_length+1));
	Instr *ret_ar=(Instr*)malloc(rd_length*sizeof(Instr));
	int abs_offst_ref=rd_length, abs_offst_rd=rd_length-1;
	int instr_len=0;
	int offst_len=0, num_len=4;
	int i=0;
	
	//int bts=0;
	char prev='0'; //'A' if nucleotide, 'n' if numeric, '0' if at the beginning, 'd' for indel
	char num[5]={'0','0','0','0','\0'};
	int last_offst=0;
	int open_indel=0, total_ins=0, cnt=0;
	for (i=strlen(dscr)-1;i>=0;i--){//scan every character of dscr backwards
		if (dscr[i]>='0' && dscr[i]<='9'){ //case of numeric
			if (prev!='n'){
				num_len=4;
				num[0]='0';num[1]='0';num[2]='0';num[3]='0'; num[4]='\0';
			}
			num[--num_len]=dscr[i];
			prev='n';
		}
		else if(dscr[i]>='A' && dscr[i]<='T'){
			if(prev=='0'){
				offst[offst_len++]=0;
				last_offst=0;
				//fprintf(stderr,"new instruction1...i:%d, dscr[i]:%c, instr_len:%d, abs_offst_rd:%d, last_offst:%d\n",i, dscr[i], instr_len, abs_offst_rd, last_offst);
				add_new_instruction(ret_ar, instr_len++, 's', dscr[i], rd[abs_offst_rd], 1, last_offst, rd_length);
				abs_offst_rd--;
				//fprintf(stderr, "abs1: %d\n",abs_offst_rd);
			}
			else if(prev=='n'){
				if(open_indel){
					fprintf(stderr, "FATAL ERROR!!!Cannot have both insertions and deletions in one indel block\n");
					exit(1);
				}
				offst[offst_len++]=atoi(num)+1;
				last_offst=atoi(num);//-1; //now last offset indicates the total identities
				abs_offst_rd-=(last_offst);
				add_new_instruction(ret_ar, instr_len++, 's', dscr[i], rd[abs_offst_rd], 1, last_offst, rd_length);
				abs_offst_rd--; //skip also the current offset
			}
			else{
				if(open_indel){//case of deletion here
					add_new_instruction(ret_ar, instr_len++, 'd', dscr[i], rd[abs_offst_rd], 1, last_offst, rd_length);
					last_offst=0;
					abs_offst_rd=abs_offst_rd; //don't change this in case of deletion
					prev='A';
					continue;
				}
				offst[offst_len++]=0;
				last_offst=0;
				abs_offst_rd-=last_offst;
				if (rd[abs_offst_rd]=='N' && ret_ar[instr_len-1].rd_ltr=='N' && ret_ar[instr_len-1].type=='s') //case of consecutive Ns
					ret_ar[instr_len-1].repeat+=1;
				else
					add_new_instruction(ret_ar, instr_len++, 's', dscr[i], rd[abs_offst_rd], 1, last_offst, rd_length);
				abs_offst_rd--;
				//abs_offst_rd-=1;
			}
			prev='A';
		}
		else if(dscr[i]=='$'){
			/*fprintf(stderr, "FATAL ERROR!!!Indels not currently supported\n");
			exit(1);*/
			///////////////fprintf(stderr, "Openning indel...prev=%c\n",prev);
			open_indel=1;
			if(prev=='0'){
				last_offst=0;
			}
			else if(prev=='n'){
				last_offst=atoi(num);
				//////////fprintf(stderr, "LAST OFFSET: %d\n", last_offst);
				abs_offst_rd-=(last_offst);
			}
			else{ //bp or another indel is preceding
				last_offst=0;
			}
 			prev='d';
		}
		else if(dscr[i]=='^'){
			open_indel=0;
			/*fprintf(stderr, "FATAL ERROR!!!Indels not currently supported\n");
			exit(1);*/ 

			if(prev=='n'){
				total_ins=atoi(num);
				for(cnt=0;cnt<total_ins;cnt++){
					add_new_instruction(ret_ar, instr_len++, 'i', 'N', rd[abs_offst_rd], 1, last_offst, rd_length);
					last_offst=0;
					abs_offst_rd--;
				}
			}
			prev='0';
		}
	}
	(*len)=instr_len;
	free(offst);
	return ret_ar;
}

//It takes whatever is necessary in order to initialize the variable members
//of instr_arr[i]
void add_new_instruction(Instr *instr_ar, int i, char type, char ref_ltr, char rd_ltr,int repeat, int offset, int instr_array_length){
	if(i>=instr_array_length){
		fprintf(stderr,"FATAL ERROR!!!Instr index greater than total supported instructions\n");
		exit(0);
	}
	instr_ar[i].type=type;
	instr_ar[i].ref_ltr=ref_ltr;
	instr_ar[i].rd_ltr=rd_ltr;
	instr_ar[i].repeat=repeat;
	instr_ar[i].offset=offset;
	//fprintf(stderr, "adding at i=%d, type: %c, ref: %c, rd: %c, repeat: %d, offset: %d\n",i, instr_ar[i].type, instr_ar[i].ref_ltr, instr_ar[i].rd_ltr, instr_ar[i].repeat, instr_ar[i].offset);

}

//It updates the index of the reference according to the values of code and strand.
void update_db_pos(int *pos, char *code, char strand){
	if (strcmp(code, "advance")==0){
		if (strand=='F') 
			(*pos)--;
		else
			(*pos)++;
	}
	else if(strcmp(code, "correct")==0){
		if(strand=='F')
			(*pos)++;
		else
			(*pos)--;
	}
	else {fprintf(stderr,"Unrecognized update_db code");exit(1);}
}

//recovers the read based on the input chromosome and the info encoded on olg.
void print_differences(Instr *instr_ar, Read rd, int total_instr, int rd_length){
	int start;
	int end;
	int i=0, j=0, k=0, cur=0;
	int read_len=rd_length;
	char *fragment=(char*)malloc(rd_length+4);
	int offst_cnt=0;
	int open_indel=0;
	int event_cnt=0;
	char ref_symbl;
	int old_offst=0;
	int offst_indx=-1;
	int tmp_db_pos;
	fragment[rd_length]='\0';
	char *norm_read=(char*)malloc(rd_length+1);//In case of R strand, it takes the complement.
	norm_read[rd_length]='\0';
	for(i=0;i<rd_length;i++){
		if (rd.strand=='F') norm_read[i]=rd.read[i];
		else switch(rd.read[i]){
			case 'A':	norm_read[i]='T'; break;
			case 'C':	norm_read[i]='G'; break;
			case 'G':	norm_read[i]='C'; break;
			case 'T':	norm_read[i]='A'; break;
		}
	}
	for(i=0;i<total_instr;i++){
		if (instr_ar[i].type=='d') read_len++;
		else if(instr_ar[i].type=='i') read_len--;
	}
	if(rd.strand=='F'){////////////////append this after the loop that counts the read len.
		cur=rd.loc+read_len-2;
		//tmp_db_pos=olg.rd.loc+read_len;
	}
	else{
	 	cur=rd.loc-1; 
		//tmp_db_pos=olg.rd.loc-1;
	}
	/*if(olg.total_instr>0){
		offst_indx=get_initial_offset_index(offset_set, total_offsets, tmp_db_pos, olg.strand); ///work around this
	}*/
	i=rd_length-1;
	//printf("HELLO FROM DIFFERENCES... total_instr: %d rd_len: %d cur_loc: %d\n",total_instr, rd_length,cur);
	while(i>=0){
		if(j<total_instr){
			offst_cnt=instr_ar[j].offset;
			//printf("offst_cnt: %d\n",offst_cnt);
			for(k=offst_cnt;k>0;k--){//no error copying from chromosome
				//fragment[i]=instr_arr[i].rd_ltr;
				printf("%d\t%c\n",cur, '.');
				///print . for identity + cur
							
				i--;
				update_db_pos(&cur, "advance", rd.strand);
			}
			if(instr_ar[j].type=='s'){
				//if(get_chromosome_data(chromosome, cur, olg.strand, 0, 0)=='N'){//case that the chromosome contains N
				if(instr_ar[j].rd_ltr!='N' || (instr_ar[j].rd_ltr=='N' && instr_ar[j].repeat==1)){
					printf("%d\t%c\n",cur, norm_read[i]);
					i--;
				}	
					
				else{//multiple Ns in a row
					for(k=0;k<instr_ar[j].repeat;k++){
						printf("%d\t%c\n",cur, norm_read[i]);
						i--;
						update_db_pos(&cur, "advance", rd.strand);
						if(k>0) update_db_pos(&tmp_db_pos, "advance", rd.strand);

					}
					update_db_pos(&cur, "correct", rd.strand);
					
				}
				
				update_db_pos(&cur, "advance", rd.strand);
				
			}
			else if(instr_ar[j].type=='d'){
				//ref_symbl=get_chromosome_data(chromosome, cur, rd.strand, 0, 0);
				printf("%d\t%c\n",cur, 'd');
				update_db_pos(&cur, "advance", rd.strand);
				
			}
			else if(instr_ar[j].type=='i'){
				event_cnt++;
				//fragment[i--]=uncompress_bp(olg.instr_ar[j].code, 'F');
				printf("%d\t%c\n",cur, norm_read[i]);
				i--;
				/*//print cur + ref_ltr
				if(j+1<olg.total_instr){
					if (olg.instr_ar[j+1].type!='i'|| olg.instr_ar[j+1].offset>0)	update_db_pos(&tmp_db_pos, "correct", olg.rd.strand);//if(olg.rd.strand=='F') tmp_db_pos++;else tmp_db_pos--;
				}*/

			}
			j++;
		}
		else{
//			fragment[i]=get_chromosome_data(chromosome, cur,olg.rd.strand, 0, 0);
			printf("%d\t%c\n",cur, '.');

			//print cur+ref_ltr
			i--;
			update_db_pos(&cur, "advance", rd.strand);
			event_cnt++;
		}
	}
	//printf("%s\t%s\t\t%d\t%c\t",fragment, chr_name, olg.loc, olg.rd.strand);
	free(fragment);
	free(norm_read);
	//print_match_descr(mdlist);
	//printf("\n");
}

inline int min(int a, int b){if (a<=b) return a; else return b;}
inline int max(int a, int b){if (a<=b) return b; else return a;}

Mult_hist create_histogram(){
	Mult_hist ret;
	ret.ttl_histograms=19;
	ret.ttl_bins=(int*)malloc(ret.ttl_histograms*sizeof(int));
	ret.ttl_bins[0]=25;
	ret.ttl_bins[1]=50;
	ret.ttl_bins[2]=125;
	ret.ttl_bins[3]=250;
	ret.ttl_bins[4]=500;
	ret.ttl_bins[5]=1250;
	ret.ttl_bins[6]=2500;
	ret.ttl_bins[7]=5000;
	ret.ttl_bins[8]=12550;
	ret.ttl_bins[9]=25000;
	ret.ttl_bins[10]=50000;
	ret.ttl_bins[11]=125000;
	ret.ttl_bins[12]=250000;
	ret.ttl_bins[13]=500000;
	ret.ttl_bins[14]=1250000;
	ret.ttl_bins[15]=2500000;
	ret.ttl_bins[16]=5000000;
	ret.ttl_bins[17]=12000050;
	ret.ttl_bins[18]=25000000;

	ret.hist=(hist_t**)malloc(sizeof(hist_t*)*(ret.ttl_histograms+1));
	int i=0;
	int j=0;
	for(i=0;i<ret.ttl_histograms;i++){
		ret.hist[i]=(hist_t*)malloc(sizeof(hist_t)*(ret.ttl_bins[i]+1));
		for(j=0;j<ret.ttl_bins[i];j++)
			ret.hist[i][j]=0;
	}
	return ret;
}

inline void update_histogram_solo(Mult_hist *mhist, int pos, int chromo_len){
	//int bin_range=chromo_len/ttl_bins;
	//int i=pos/bin_range;
	int bin_range;
	int i,j;
	for (i=0;i<(mhist->ttl_histograms);i++){
		bin_range=chromo_len/(mhist->ttl_bins[i]);
		j=pos/bin_range;
		mhist->hist[i][j]+=1;
	}
}

void print_histogram(FILE *fp, Mult_hist mhist, int chromo_len){
	int i=0, j=0;
	fprintf(fp, "histograms = \n[\n");
	for(i=0;i<mhist.ttl_histograms;i++){
		fprintf(fp, "\t{\"basesPerBin\" : %d,\n",chromo_len/(mhist.ttl_bins[i]));
		fprintf(fp, "\t\"counts\" : [%d",mhist.hist[i][0]);
		for(j=1;j<mhist.ttl_bins[i];j++) //not j=0 because mhist[i][0] is above
			fprintf(fp, ",%d",mhist.hist[i][j]);
		fprintf(fp,"]\n");
		if (i==(mhist.ttl_histograms)-1) fprintf(fp, "}\n");
		else fprintf(fp, "},\n");
	}
	fprintf(fp, "]\n");
}

void destroy_histogram(Mult_hist *mhist){
	int i=0;
	for (i=0;i<(mhist->ttl_histograms);i++)
		free(mhist->hist[i]);
	free(mhist->hist);
	mhist->hist=NULL;
	free(mhist->ttl_bins);
	mhist->ttl_bins=NULL;
}

///bitmap related functions.
//it initializes and returns a bit vector of "size" bits.
unsigned long *initialize_bit_vector(int size){
	if(size<0) ioerror("Negative size of bit vector");
	int len=1+size/(sizeof(long)*8);
	unsigned long *ret=(long*)malloc(sizeof(long)*(len+3));
	int i=0;
	for(i=0;i<len;i++)
		ret[i]=0;
	return ret;
}

//it sets to 1 the pos-th position of vec. Size is the length of the 
//bit vector.
inline void set_bit(unsigned long *vec, int size, int pos){
	if(pos<0) ioerror("Trying to set a negative position at a bitvector");
	if(pos>=size) ioerror("Trying to set a position that is greater than the length of bitvector");
	int x_pos=pos/(sizeof(long)*8); //convert bytes to bits
	int y_pos=pos%(sizeof(long)*8);
	unsigned long w=1;
	w=w<<y_pos;
	vec[x_pos]=vec[x_pos]|w;
	return;
}

//It returns the value of the pos-th bit of the bitvector. Size is the length
//of the bit vector (and not of the array).
inline int get_bit(long *vec, int size, int pos){
	if(pos<0) ioerror("Trying to set a negative position at a bitvector");
	if(pos>=size){
		fprintf(stderr, "pos: %d size: %d\n",pos, size);
		ioerror("Trying to get a position that is greater than the length of bitvector");
	}
	int x_pos=pos/(sizeof(long)*8);
	int y_pos=pos%(sizeof(long)*8);
	unsigned long w=vec[x_pos];
	w=w>>y_pos;
	return w&1;
}



