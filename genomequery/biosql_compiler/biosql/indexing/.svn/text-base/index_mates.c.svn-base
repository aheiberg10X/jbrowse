#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
#include"defs.h"
#include<bam.h>
#include<sam.h>

/*typedef struct{
	Mates *mates;
	Ins_len_groups *groups;
} MatesGroups;*/

//It reads the file that is produced by the program locate_mates and returns
//an array of mates of length len and an array of Ins_len_groups of length ttl_grps
Mates *init_index(char *fname, int *len){
	FILE *fp=open_file(fname);
	char *buf=(char*)malloc(1024*sizeof(char));
	fgets(buf, 1023, fp);
	if (buf[0]!='!') ioerror("Bad file formatting");
	(*len)=atoi(&buf[1]);

	Mates *ret=(Mates*)malloc(sizeof(Mates)*(*len+2));
	int left=0, right=0;
	char *tok;
	int i=0;
	fgets(buf,1023,fp);//read a comment line
	while(fgets(buf, 1023, fp)!=NULL){
		if (buf[0]=='#') break; 
		if (strlen(buf)<2) continue;
		tok=strtok(buf, "\t");
		left=atoi(tok);
		tok=strtok(NULL, "\t");
		right=atoi(tok);
		if ((left > (*len)) || (right > (*len))) ioerror("Coordinates exceed index length");
		ret[left].mate_indx=right;
		if (right>=0){
			ret[right].mate_indx=left;
		}
	}
	fclose(fp);
	free(buf);
	return ret;
}

void update_strand(long *strands, int pos, int strand_len, int flag);

//fname is a bam file. mates is an array of n Mates variables. 
//The function reads fname and updates the loc and byte_offst variables of each
//entry of the table.
void fill_index(char *fname, Mates *mates, int n, long *strand_info, int strand_len){

	samfile_t *fp=samopen(fname, "rb", NULL);
	if (fp==NULL) ioerror("Cannot open bam file to read from");
	bam1_t *b=bam_init1();
	long long bo=0;
	int linelen=0;
	int i=1; //reserve the 0-th pos for the unmapped
	mates[0].loc=-1; mates[0].mate_indx=-1; mates[0].byte_offst=0;
	bo=bam_tell(fp->x.bam);
	while(samread(fp,b)>0){
		if (i>n) ioerror("More than the expected reads found");
		if ((b->core.flag & 0x4)!=0) mates[i].loc=-1; 
		else{
			mates[i].loc=b->core.pos;
			update_strand(strand_info, i, strand_len, b->core.flag);
		}
		mates[i].byte_offst=bo;
		mates[i].read_len=b->core.l_qseq;
		bo=bam_tell(fp->x.bam);
		i++;
	}
	if(i!=n){
		fprintf(stderr,"i: %d n: %d\n",i, n);
		ioerror("Discrepancy on the number of reads");
	}
	/*mates[i].loc=-1;
	mates[i].byte_offst=ttl_bytes;
	mates[i].mate_indx=-1;
	(*n)=(i+1);*/
	samclose(fp);
	bam_destroy1(b);

}

//stores integer n and n mates in fname. It also stores integer ttl_grps and so many
//groups of Ins_len_groups.
//void store_indx(char *fname, Mates *mates, int n, Ins_len_groups *grps, int ttl_grps){
void store_indx(char *fname, Mates *mates, int n, long *strand_info, int strand_len){
	/*int i=0;
	printf("!%d\n!%d\n",n, ttl_grps);
	printf("#indx\tmate_indx\tname\n");
	for(i=0;i<n;i++) printf("%d\t%d\t%d\n",i,mates[i].mate_indx, mates[i].group_indx);
	printf("#group_indx\tmedian\tabsolute_deviation\n");
	for(i=0;i<ttl_grps;i++) printf("%d\t%d\t%d\n", i, grps[i].len, grps[i].sd);*/
	FILE *fp=fopen(fname, "w");
	if (fp==NULL) ioerror("Cannot open the index file to write to");
	fwrite(&n, sizeof(int), 1, fp);
	fwrite(mates, sizeof(Mates), n, fp);
	fwrite(&strand_len, sizeof(int), 1, fp);
	fwrite(strand_info, sizeof(long), strand_len, fp);
	//fwrite(&ttl_grps, sizeof(int), 1, fp);
	//fwrite(grps, sizeof(Ins_len_groups), ttl_grps, fp);

	fclose(fp);
}

void print_indx(Mates *mates, int n){
	int i=0;
	for(i=0;i<n;i++)
		printf("i: %d loc: %d mate_i: %d bo: %lld\n",i, mates[i].loc, mates[i].mate_indx, mates[i].byte_offst);
}

//It creates a bitmap of length "ttl_reads". The bitmap is organized as an array of long values
//and its length is returned with the pointer len.
long *initialize_strands(int ttl_reads, int *len){
	int i=0;
	int ret_len=(ttl_reads/(sizeof(long)*8))+1; //there are 32/64 bits per line
	long *ret=(long*)malloc(sizeof(long)*ret_len);
	(*len)=ret_len;
	for (i=0;i<ret_len;i++)
		ret[i]=0;
	return ret;
}

//strands is the array of length strand_len that keeps the strand info.
//pos is the position that we need to update
//flag is the "flag" field of the sam. It needs to be masked with 0x10 to
//get the strand info
void update_strand(long *strands, int pos, int strand_len, int flag){
	if (pos >= strand_len*8*sizeof(long)) ioerror("Pos larger than strand array"); //don't forget the conversions from bits to bytes
	unsigned long st;
	if ((flag & 0x10) == 0) st=0;
	else st=1;
	int xpos=pos/(sizeof(long)*8);
	int ypos=pos%(sizeof(long)*8);
	unsigned long w=strands[xpos];
	st=st<<ypos;
	strands[xpos]=w|st;
	return;
}

	
int main(int argc, char **argv){
	if (argc!=4) ioerror("Usage: ./index_mates <mate_file> <bam_file> <output_file>");
	char *mate_file=argv[1];
	char *bam_file=argv[2];
	char *outpt_file=argv[3];
	int ttl_reads=0, ttl_groups=0;
	int strand_len=0;
	/*MatesGroups *mg;
	Ins_len_groups *grps;
	mg=init_index(mate_file, &ttl_reads, &ttl_groups);*/
	Mates *read_info=init_index(mate_file, &ttl_reads);//mg->mates;
	long *strand_info=initialize_strands(ttl_reads, &strand_len);
	//grps=mg->groups;
	fprintf(stderr, "ttl_reads: %d\n",ttl_reads);
	fill_index(bam_file, read_info, ttl_reads, strand_info, strand_len);
	store_indx(outpt_file, read_info, ttl_reads, strand_info, strand_len); //, grps, ttl_groups);
	//print_indx(read_info, ttl_reads);
	free(read_info);
	free(strand_info);
	//free(grps);
}

