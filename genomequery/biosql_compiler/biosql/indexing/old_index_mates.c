#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
#include"defs.h"
#include<bam.h>
#include<sam.h>

//It reads the file that is produced by the program locate_mates and returns
//an array of mates of length len.
Mates *init_index(char *fname, int *len){
	FILE *fp=open_file(fname);
	char *buf=(char*)malloc(1024*sizeof(char));
	fgets(buf, 1023, fp);
	if (buf[0]!='!') ioerror("Bad file formatting");
	(*len)=atoi(&buf[1]);
	Mates *ret=(Mates*)malloc(sizeof(Mates)*(*len+2));
	int left=0, right=0;
	char *tok;
	while(fgets(buf, 1023, fp)!=NULL){
		if (buf[0]=='#') continue; 
		if (strlen(buf)<2) continue;
		tok=strtok(buf, "\t");
		left=atoi(tok);
		tok=strtok(NULL, "\t");
		right=atoi(tok);
		if ((left > (*len)) || (right > (*len))) ioerror("Coordinates exceed index length");
		ret[left].mate_indx=right;
		if (right>=0)
			ret[right].mate_indx=left;
	}
	fclose(fp);
	free(buf);
	return ret;
}

//fname is a bam file. mates is an array of n Mates variables. 
//The function reads fname and updates the loc and byte_offst variables of each
//entry of the table.
void fill_index(char *fname, Mates *mates, int n){

	samfile_t *fp=samopen(fname, "rb", NULL);
	if (fp==NULL) ioerror("Cannot open bam file to read from");
	bam1_t *b=bam_init1();
	long long bo=0;
	int linelen=0;
	int i=1; //reserve 0-th pos for the unmapped
	mates[0].loc=-1; mates[0].mate_indx=-1; mates[0].byte_offst=0;
	bo=bam_tell(fp->x.bam);
	while(samread(fp,b)>0){
		if (i>n) ioerror("More than the expected reads found");
		if ((b->core.flag & 0x4)!=0) mates[i].loc=-1; 
		else mates[i].loc=b->core.pos;
		mates[i].byte_offst=bo;
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

//stores integer n and n mates in fname.
void store_indx(char *fname, Mates *mates, int n){
	FILE *fp=fopen(fname, "w");
	if (fp==NULL) ioerror("Cannot open the index file to write to");
	fwrite(&n, sizeof(int), 1, fp);
	fwrite(mates, sizeof(Mates), n, fp);
	fclose(fp);
}

void print_indx(Mates *mates, int n){
	int i=0;
	for(i=0;i<n;i++)
		printf("i: %d loc: %d mate_i: %d bo: %lld\n",i, mates[i].loc, mates[i].mate_indx, mates[i].byte_offst);
}
	
int main(int argc, char **argv){
	if (argc!=4) ioerror("Usage: ./index_mates <mate_file> <bam_file> <output_file>");
	char *mate_file=argv[1];
	char *bam_file=argv[2];
	char *outpt_file=argv[3];
	int ttl_mates=0;
	Mates *read_info=init_index(mate_file, &ttl_mates);
	fprintf(stderr, "ttl_mates: %d\n",ttl_mates);
	fill_index(bam_file, read_info, ttl_mates);
	store_indx(outpt_file, read_info, ttl_mates);
	//print_indx(read_info, ttl_mates);
	free(read_info);
}

