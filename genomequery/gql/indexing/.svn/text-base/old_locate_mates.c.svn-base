//this product gets as input a file of reads and for each read outputs its 
//"name", its location and the offset of the bam file at which it begins.

#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include "defs.h"
#include<sys/sysinfo.h>
#include <bam.h>
#include <sam.h>

struct Strippedread{
	char *name;
	int indx;
	int mate_indx;
	struct Strippedread *next;
};

typedef struct Strippedread Sr;

struct Hashtable{
	int size;
	Sr **table; //An array of pointers of Sr which are heads of lists of Sr
				//objects
};

typedef struct Hashtable HTable;

//If a mate has been already found, it updates the mate_indx information. 
//Otherwise it creates a new node and it appends it in the end of the list.
void add_element(Sr **head, bam1_t *b, int indx){
	Sr *tmp;
	Sr *nr;
	char *rd_name=bam1_qname(b);
	if ((*head)==NULL){
		nr=(Sr *)malloc(sizeof(Sr));
		nr->name=strdup(rd_name);
		nr->indx=indx;
		nr->mate_indx=0; //the 0-th mate is the dangling mate
		nr->next=NULL;
		(*head)=nr;
		return;
	}
	for(tmp=(*head);tmp->next!=NULL;tmp=tmp->next){//This loop ignores the last
													//element of each list. 
		if (strcmp(tmp->name, rd_name)==0) break;
	}
	if (strcmp(tmp->name, rd_name)==0){
		tmp->mate_indx=indx;
		return;
	}
	nr=(Sr *)malloc(sizeof(Sr));
	nr->name=strdup(rd_name);
	nr->indx=indx;
	nr->mate_indx=0; //the 0-th mate is the dangling mate

	nr->next=NULL;
	tmp->next=nr;
	return;
}

unsigned int sdbm_hash(const char *key)
{
    unsigned int h=0;
    while(*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
    return h;
}

unsigned int APHash(const char* str)
{
   unsigned int hash = 0xAAAAAAAA;
   unsigned int i    = 0;
   unsigned int len = strlen(str);

   for(i = 0; i < len; str++, i++)
   {
      hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*str) * (hash >> 3)) :
                               (~((hash << 11) + (*str) ^ (hash >> 5)));
   }

   return hash;
}

void initialize_hashtable(HTable *ht, int size){
	ht->size=size;
	int i;
	ht->table=(Sr**)malloc(sizeof(Sr*)*size);
	for(i=0;i<size;i++) ht->table[i]=NULL;
}

void print_all(HTable ht){
	int i=0;
	//int cnt=0;
	Sr *tmp=NULL;
	printf("#indx\tmate_indx\n");
	for (i=0;i<ht.size;i++){
		if (ht.table[i]==NULL) continue;
		tmp=ht.table[i];
		//cnt=0;
		while(tmp!=NULL){
			printf("%d\t%d\t\n",tmp->indx, tmp->mate_indx);
			tmp=tmp->next;
		//	cnt+=1;
		}
		//printf("#cnt: %d\n",cnt);
	}
}

void destroy_all(HTable ht){
	int i=0;
	Sr *tmp, *vic;
	for (i=0;i<ht.size;i++){
		if (ht.table[i]==NULL) continue;
		tmp=ht.table[i];
		while(tmp!=NULL){
			vic=tmp;
			tmp=tmp->next;
			free(vic->name);
			free(vic);
		}
		//free(ht.table[i]);
	}
	free(ht.table);
}
	
int main(int argc, char **argv){
	if (argc!=2) ioerror("usage: ./locate_mates <bam_file>");
	char *fname=argv[1];
	long long byte_offset=0;
	int linelen=0;
	int i=1; //Reserve i=0 for the dangling pairs
	struct mallinfo mf;
	mf=mallinfo();
	fprintf(stderr,"Initially: uordblocks: %d\n",mf.uordblks);
	samfile_t *fp=samopen(fname, "rb", NULL);
	bam1_t *b=bam_init1();
	HTable ht;
	initialize_hashtable(&ht, 1000000);
	unsigned int hash_i=0;
	char *s1=NULL, *s2=NULL;
	unsigned int hash_outcome=0;
	char *rd_name=NULL;
	while(samread(fp,b)>0){
		rd_name=bam1_qname(b);
		hash_outcome=sdbm_hash(rd_name);
		hash_i=hash_outcome%(ht.size);
		add_element(&(ht.table[hash_i]), b, i);
		i+=1;
	}
	printf("!%d\n",i); //first line shows total number of reads.
	mf=mallinfo();
	fprintf(stderr,"uordblocks: %d\n",mf.uordblks);
	bam_destroy1(b);
	samclose(fp);
	print_all(ht);
	destroy_all(ht);
	mf=mallinfo();
	fprintf(stderr,"After freeing...uordblocks: %d\n",mf.uordblks);

}

	




