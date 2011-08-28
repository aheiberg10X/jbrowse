#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "all_tools.h"

//it stores n entries of tble to fname. Since each entry of tbl contains
//two strings (id, chr), it stores those strings immediately afterwards
//one under the other.
void store_imported(char *fname, Imported_info *tbl, int n){
	FILE *fp=fopen(fname, "w");
	if (fp==NULL) ioerror("Cannot open file to write");
	fwrite(&n, sizeof(int), 1, fp);
	fwrite(tbl, sizeof(Imported_info), n, fp);
	int i=0;
	for(i=0;i<n;i++){
		fprintf(fp, "%s\n",tbl[i].id);
		fprintf(fp, "%s\n",tbl[i].chr);
	}
	fclose(fp);
	return;
}

//it assumes the first 4 bytes of fname is the vec_len of the vector 
//that is stored immediately afterwards. It also recreates strings id and chr
//of each entry which are stored immediately after the vector.
Imported_info *retrieve_imported(char *fname, int *vec_len){
	FILE *fp=fopen(fname, "r");
	if (fp==NULL) ioerror("Cannot open binary file");
	fread(vec_len, sizeof(int), 1, fp);
	Imported_info *vec=malloc((sizeof(Imported_info)+2)*(*vec_len));
	fread(vec, sizeof(Imported_info), *vec_len, fp);
	char *buf=malloc(2048);
	int i=0;
	for(i=0;i<(*vec_len);i++){
		if(fgets(buf,2048,fp)==NULL) ioerror("missing id from imported");
		buf[strlen(buf)-1]='\0'; //get rid of \n
		vec[i].id=strdup(buf);
		if(fgets(buf,2048,fp)==NULL) ioerror("missing chr from imported");
		buf[strlen(buf)-1]='\0'; //get rid of \n
		vec[i].chr=strdup(buf);
	}

	fclose(fp);
	return vec;
}

/*lst is a pointer to an array of pointers of Imported_info of length n.
The function appends val to lst while it updates n*/
void append_element(Imported_info **lst, Imported_info val, int *n){
	Imported_info *tmp;
	int cnt=(*n)+1;
	tmp=(Imported_info*)realloc(*lst, cnt*sizeof(Imported_info));
	if (tmp==NULL) ioerror ("Cannot find memory space to store an interval");
	tmp[cnt-1]=val;
	*lst=tmp;
	*n=cnt;
}

//fname is a tab separated file whose lines contain the information to 
//construct an array of Imported_info objects. The location of id, start, end
//and chr columns are denoted by id_col, st_col, nd_col and chr_col respectively.
//n is the number of the returned array.
//The function ignores all entries of the file whose chromosome field does not
//match the input chr.
Imported_info *load_table(char *fname, int id_col, int st_col, int nd_col, int chr_col, int *n, char *chr){
	Imported_info *ret=(Imported_info*)malloc(sizeof(Imported_info)*2);
	FILE *fp=open_file(fname);
	char *buf=(char*)malloc(2048);
	char *tmp;
	int col_no=1;
	Imported_info new_val;
	*n=0;
	while(fgets(buf,2048,fp)!=NULL){
		if (buf[0]=='#') continue;
		if (strlen(buf)<3) continue;
		tmp=strtok(buf, "\t");
		col_no=1;
		while(tmp!=NULL){
			if(col_no==id_col)
				new_val.id=strdup(tmp);
			else if(col_no==st_col)
				new_val.start=atoi(tmp);
			else if(col_no==nd_col)
				new_val.end=atoi(tmp);
			else if(col_no==chr_col)
				new_val.chr=strdup(tmp);
			col_no++;
			tmp=strtok(NULL,"\t");
		}
		if(strcmp(new_val.chr, chr)!=0) //ignore entries from different chr
			continue;
		append_element(&ret, new_val, n);
	}
	fclose(fp);
	free(buf);
	return ret;
}

//it destroys tbl and releases the memory that is reserved by the strings
//of its entries.
void destroy_imported(Imported_info *tbl, int ttl_tbl){
	int i=0;
	for(i=0;i<ttl_tbl;i++){
		free(tbl[i].id);
		free(tbl[i].chr);
	}
	free(tbl);
}


//It returns an array of indexes to those entries of tbl each of which
//satisfies the isvalid_imported test. ttl_valid is the length of the
//returned array.
int *find_valid_imported(Imported_info *tbl, int ttl_tbl, int *ttl_valid){
	int *ret;
	int cnt=0;
	int i=0;

	while (i<ttl_tbl){
		if (isvalid_imported(tbl, i, ttl_tbl)>0) cnt+=1;
		i++;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	i=0;
	while(i<ttl_tbl){
		if(isvalid_imported(tbl, i, ttl_tbl)>0) ret[cnt++]=i;
		i++;
	}
	*ttl_valid=cnt;
	return ret;
}

//It prints to fname those entries of tbl that are located in valid_lst.
//ttl_tbl is the length of the table, ttl_valid is the length of the valid_lst
void print_imported_list(char *fname, Imported_info *tbl, int ttl_tbl, int* valid_lst, int ttl_valid){
	int j=0, i=0;
	FILE *fp=fopen(fname, "w");
	for(j=0;j<ttl_valid;j++){
		i=valid_lst[j];
		fprintf(fp, "%s\t%s\t%d\t%d\n",tbl[i].id, tbl[i].chr, tbl[i].start, tbl[i].end);
	}
	fclose(fp);
	return;
}

