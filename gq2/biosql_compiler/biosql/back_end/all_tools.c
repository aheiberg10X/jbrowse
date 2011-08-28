#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "all_tools.h"

//It returns 1 if the file exists. 0 otherwise
int file_exists(char *fname){
	FILE *fp=fopen(fname, "r");
	if(fp==NULL) return 0;
	fclose(fp);
	return 1;
}

//it stores vec_len and vec into fname. size is the size of entry of vec
void store_vec(char *fname, void *vec, size_t size, int vec_len){
	FILE *fp=fopen(fname, "w");
	if (fp==NULL) ioerror("Cannot open file to write");
	fwrite(&vec_len, sizeof(int), 1, fp);
	fwrite(vec, size, vec_len, fp);
	fclose(fp);
	return;
}

//it assumes the first 4 bytes of fname is the vec_len of the vector 
//that is stored immediately afterwards. It retrieves both elements.
void *retrieve_vec(char *fname, size_t size, int *vec_len){
	FILE *fp=fopen(fname, "r");
	if (fp==NULL) ioerror("Cannot open binary file");
	fread(vec_len, sizeof(int), 1, fp);
	void *vec=malloc((size+2)*(*vec_len));
	fread(vec, size, *vec_len, fp);
	fclose(fp);
	return vec;
}


//It performs an interval transaction.
//mate_indx is an array of reads of type mates and length ttl_reads.
//valid_lst is a set of indexes of mate_indx of length ttl_valid
//bam_file is the input raw file
//mts is the set of intervals (of length ttl_mts) that the function is going to be applied
//histinfo is going to be used for histogram related purposes.
//coverage_limit is the minimum or maximum desired set of clones.
//if up_low=0 we desire cases that count>=coverage_limit
//if up_low=1 we desire cases that count<=coverage_limit.

//chr is the chromosome name.

//if in_regions, in_evidence are NULL, they are ignored and intrev data is obtained by balanced parenthesis. 
//Othrwise the data is obtained from those files. 

//If any of out_regions, out_meta_regions, out_meta_evidence, out_evidence is NULL, the respective quantities are not stored.

void interval_transaction(Mates *mate_indx, int ttl_reads, int *valid_lst, int ttl_valid, char *bam_file, Intervals *mts, int ttl_mts, Hist_info histinfo, int coverage_limit, int up_low, char *chr, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence, char *out_evidence_reduced, long *strand_indx, int strand_len){
	Intrevidence intrev;
	Intervals *rint;
	int ttl_rint;
	int *ev; //evidence vector.
	int ttl_ev;
	int i=0;
	///decision: perform interval_coverage vs get intrev data from an input file
	printf("in evidence: %s in_regions: %s\n", in_evidence, in_regions);
	if(in_evidence==NULL && in_regions==NULL){
		if(up_low==0)
			intrev=min_interval_coverage(mate_indx, ttl_reads, mts, ttl_mts, coverage_limit);
		else if(up_low==1)
			intrev=max_interval_coverage(mate_indx, ttl_reads, mts, ttl_mts, coverage_limit);
		rint=intrev.regions;
		ttl_rint=intrev.ttl_regions;
		ev=intrev.evidence;
		ttl_ev=intrev.ttl_evidence;
	}
	else if(in_evidence!=NULL && in_regions!=NULL){
		rint=(Intervals*)retrieve_vec(in_regions, sizeof(Intervals), &ttl_rint);
		ev=(int*)retrieve_vec(in_evidence, sizeof(int), &ttl_ev);
	}
	else ioerror("in_evidence and in_regions should both be either NULL or non NULL at the same time");
	//decision: store intrev to a file vs creating a regions.txt file and (if desirable) an evidence file
	if(out_meta_evidence!=NULL && out_meta_regions!=NULL){
		store_vec(out_meta_evidence, ev, sizeof(int), ttl_ev);
		store_vec(out_meta_regions, rint, sizeof(Intervals), ttl_rint);
	}

	else if((out_meta_evidence==NULL && out_meta_regions!=NULL)||(out_meta_evidence!=NULL && out_meta_regions==NULL))
		ioerror("out_meta_evidence, out_meta_regions should both be either NULL or non NULL at the same time");

	printf("out_regions: %s\n",out_regions);

	if(out_regions!=NULL){
		FILE *fp=fopen(out_regions,"w");
		if(fp==NULL) ioerror("cannot open file out_regions to write to");
		for(i=0;i<ttl_rint;i++){
			fprintf(fp,">Region %d %d %d\n",rint[i].st, rint[i].nd, rint[i].nd-rint[i].st);
		}
		fclose(fp);
	}

	/*for(i=0;i<ttl_ev;i++){
		printf("indx: %d\n",ev[i]);
	}*/
	
	//decision: output reads to a bam file vs updating only the histinfo.
	//if out_evidence is NULL the function updates histinfo only.
	if(out_evidence!=NULL)
		print_rd_list(mate_indx, ttl_reads, ev, ttl_ev, bam_file, out_evidence, 1, histinfo);
	if(out_evidence_reduced!=NULL)
		print_reduced_rd_list(mate_indx, ttl_reads, strand_indx, strand_len, ev, ttl_ev, out_evidence_reduced, 1);



	free(rint);
	free(ev);

}

//it creates a valid list from tbl_file and stores it to out_valid_lst_file
void filter_imported_transaction(char *tbl_file, char *out_valid_lst_file){
	int *valid_lst=NULL, ttl_valid=0;
	Imported_info *tbl=NULL;
	int ttl_table=0;

	tbl=retrieve_imported(tbl_file, &ttl_table);
	valid_lst=find_valid_imported(tbl, ttl_table, &ttl_valid);
	store_vec(out_valid_lst_file, valid_lst, sizeof(int), ttl_valid);
	return;
}



//indx_file1,2 are either mates.indx files that correspond to the input bam_file, or files that have been created by the load_table command.
//type1,2 specify the nature of the respective input files. The possible values of type are: "imported", "bam_single", "bam_mates".
//As always, the output is stored to whatever output files that are non NULL. 
//In case of "unknown" types, the output "out_txt1/2" are tab delimited text files.
void intersect_transaction(char *indx_file1, char *type1, char *valid_lst_file1, char *indx_file2, char *type2, char *valid_lst_file2, char *in_bam_file, char *out_bam_file, char *out_bam_reduced, char *out_txt1, char *out_txt2, Hist_info histinfo, char *in_join_metafile, char *out_join_metafile){
	Mates *mate_indx=NULL;
	int ttl_reads=0;
	long *strand_indx=NULL;
	int len_strand=0;
	Imported_info *tbl1=NULL;
	int ttl_tbl1=0;
	Imported_info *tbl2=NULL;
	int ttl_tbl2=0;

	Intervals *int1=NULL, *int2=NULL;
	int ttl_int1, ttl_int2;

	int *tmp1_valid_lst=NULL, ttl_tmp1_valid;
	int *tmp2_valid_lst=NULL, ttl_tmp2_valid;

	int *valid_lst1=NULL, ttl_valid1;
	int *valid_lst2=NULL, ttl_valid2;

	int *rd_lst=NULL, len_rd_lst=0;
	int mate_flag=0; //if 1, mates are enables

	Join *join_lst=NULL;
	int ttl_join;

	if (strcmp(type1,"imported")==0){
		///tbl1=(Imported_info*)retrieve_vec(indx_file1, sizeof(Imported_info), &ttl_tbl1);
		tbl1=retrieve_imported(indx_file1, &ttl_tbl1);

		//tmp1_valid_lst=find_valid_imported(tbl1, ttl_tbl1, &ttl_tmp1_valid);
		if(valid_lst_file1!=NULL){
			if(file_exists(valid_lst_file1)){
				tmp1_valid_lst=retrieve_vec(valid_lst_file1, sizeof(int), &ttl_tmp1_valid);
			}
			else{
				tmp1_valid_lst=NULL; ttl_tmp1_valid=0;
			}
			int1=imported2intervals(tbl1, ttl_tbl1, tmp1_valid_lst, ttl_tmp1_valid, &ttl_int1);
			if(tmp1_valid_lst!=NULL){ 
				free(tmp1_valid_lst); tmp1_valid_lst=NULL; ttl_tmp1_valid=0;
			}
		}
	}
	else if(strncmp(type1,"bam",3)==0){
		mate_indx=get_mate_index(indx_file1, &ttl_reads);
		strand_indx=get_strand_index(indx_file1, &len_strand);
		if(strcmp(type1, "bam_mates")==0){
			//tmp1_valid_lst=find_valid_clones(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_tmp1_valid);
			if(valid_lst_file1!=NULL){
				tmp1_valid_lst=retrieve_vec(valid_lst_file1, sizeof(int), &ttl_tmp1_valid);
				int1=mates2intervals(mate_indx, ttl_reads, tmp1_valid_lst, ttl_tmp1_valid, &ttl_int1);
				if(tmp1_valid_lst!=NULL){ free(tmp1_valid_lst); tmp1_valid_lst=NULL; ttl_tmp1_valid=0;}
			}
			mate_flag=1;
		}
		else{
			//tmp1_valid_lst=find_valid_reads(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_tmp1_valid, -1, -1);
			if(valid_lst_file1!=NULL){
				tmp1_valid_lst=retrieve_vec(valid_lst_file1, sizeof(int), &ttl_tmp1_valid);
				int1=singles2intervals(mate_indx, ttl_reads, tmp1_valid_lst, ttl_tmp1_valid, &ttl_int1);
				if(tmp1_valid_lst!=NULL){ free(tmp1_valid_lst); tmp1_valid_lst=NULL;ttl_tmp1_valid=0;}
			}
			mate_flag=0;
		}

	}
	else ioerror("Unknown value of type1");
	
	if (strcmp(type2,"imported")==0){
		//tbl2=(Imported_info*)retrieve_vec(indx_file2, sizeof(Imported_info), &ttl_tbl2);
		tbl2=retrieve_imported(indx_file2, &ttl_tbl2);
		//tmp2_valid_lst=find_valid_imported(tbl2, ttl_tbl2, &ttl_tmp2_valid);
		if(valid_lst_file2!=NULL){
			if(file_exists(valid_lst_file1)){
				tmp2_valid_lst=retrieve_vec(valid_lst_file2, sizeof(int), &ttl_tmp2_valid);
			}
			else{
				tmp2_valid_lst=NULL; ttl_tmp2_valid=0;
			}

			int2=imported2intervals(tbl2, ttl_tbl2, tmp2_valid_lst, ttl_tmp2_valid, &ttl_int2);
			if(tmp2_valid_lst!=NULL){ free(tmp2_valid_lst); tmp2_valid_lst=NULL;ttl_tmp2_valid=0;}
		}
	}
	else if(strncmp(type2,"bam",3)==0){
		mate_indx=get_mate_index(indx_file2, &ttl_reads);
		strand_indx=get_strand_index(indx_file2, &len_strand);
		if(strcmp(type2, "bam_mates")==0){
			//tmp2_valid_lst=find_valid_clones(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_tmp2_valid);
			if(valid_lst_file2!=NULL){
				tmp2_valid_lst=retrieve_vec(valid_lst_file2, sizeof(int), &ttl_tmp2_valid);
				int2=mates2intervals(mate_indx, ttl_reads, tmp2_valid_lst, ttl_tmp2_valid, &ttl_int2);
				if(tmp2_valid_lst!=NULL){ free(tmp2_valid_lst); tmp2_valid_lst=NULL;ttl_tmp2_valid=0;}
			}
			mate_flag=1;
		}
		else{
			//tmp2_valid_lst=find_valid_reads(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_tmp2_valid, -1, -1);
			if(valid_lst_file2!=NULL){
				tmp2_valid_lst=retrieve_vec(valid_lst_file2, sizeof(int), &ttl_tmp2_valid);
				int2=singles2intervals(mate_indx, ttl_reads, tmp2_valid_lst, ttl_tmp2_valid, &ttl_int2);
				if(tmp2_valid_lst!=NULL){ free(tmp2_valid_lst); tmp2_valid_lst=NULL;ttl_tmp2_valid=0;}
			}
			mate_flag=0;
		}
	}
	else ioerror("Unknown value of type2");
	
	if (in_join_metafile==NULL)
		join_lst=join_intervals(int1, int2, ttl_int1, ttl_int2, &ttl_join);
	else{
		join_lst=retrieve_vec(in_join_metafile, sizeof(Join), &ttl_join);
		filter_join_list(&join_lst, &ttl_join, tbl1, ttl_tbl1, tbl2, ttl_tbl2, mate_indx, ttl_reads, strand_indx, len_strand);////////////////////////
	}

	if(out_join_metafile!=NULL)
		store_vec(out_join_metafile, join_lst, sizeof(Join), ttl_join);

	/*if (strncmp(type1, "bam",3)==0){ rd_lst=valid_lst1; len_rd_lst=ttl_valid1;}
	if (strncmp(type2, "bam",3)==0){ rd_lst=valid_lst2; len_rd_lst=ttl_valid2;}*/


	/*if(out_valid_lst_file1!=NULL)
		store_vec(out_valid_lst_file1, valid_lst1, sizeof(int), ttl_valid1);
	if(out_valid_lst_file2!=NULL)
		store_vec(out_valid_lst_file1, valid_lst1, sizeof(int), ttl_valid1);*/
	if(out_bam_file!=NULL && in_bam_file!=NULL){
		rd_lst=isolate_join_indexes(join_lst, ttl_join, &len_rd_lst, type1, type2);////////////////////////////
		if(rd_lst!=NULL){
			print_rd_list(mate_indx, ttl_reads, rd_lst, len_rd_lst, in_bam_file, out_bam_file, mate_flag, histinfo);
			print_reduced_rd_list(mate_indx, ttl_reads, strand_indx, len_strand, rd_lst, len_rd_lst, out_bam_reduced, mate_flag);

			free(rd_lst);
			rd_lst=NULL;
			len_rd_lst=0;
		}
	}
	if(out_txt1!=NULL && strcmp(type1, "imported")==0){
		valid_lst1=isolate_join_indexes(join_lst, ttl_join, &ttl_valid1, type1, NULL);
		if(valid_lst1!=NULL){
			print_imported_list(out_txt1, tbl1, ttl_tbl1, valid_lst1, ttl_valid1);
			free(valid_lst1);
			valid_lst1=NULL;
			ttl_valid1=0;
		}
	}
	else if(out_txt2!=NULL && strcmp(type2, "imported")==0){
		valid_lst2=isolate_join_indexes(join_lst, ttl_join, &ttl_valid2, NULL, type2);
		if(valid_lst2!=NULL){
			print_imported_list(out_txt2, tbl2, ttl_tbl2, valid_lst2, ttl_valid2);
			free(valid_lst2);
			valid_lst2=NULL;
			ttl_valid2=0;
		}
	}
	else{ 
		//fprintf(stderr, "out_txt1: %x type1: %s out_txt2: %x type2: %s\n",out_txt1, type1, out_txt2, type2);
		ioerror("Unsupported combination of input and output params");
	}

	if(int1!=NULL) free(int1);
	if(int2!=NULL) free(int2);
	if(valid_lst1!=NULL) free(valid_lst1);
	if(valid_lst2!=NULL) free(valid_lst2);
	if (join_lst!=NULL) free(join_lst);
}

		

	

//valid_lst is loaded from valid_lst_file, when non null. 
//interval file contains intervals when non null.
//out_valid_file and out_interval_file store the valid list and the intervals when non NULL.
//all the rest files are used for the interval transaction. If all of them are null, the interval transaction does not happen.
void mate_transaction(char *indx_file, char *bam_file, int min_clones, Hist_info histinfo, char *chr_name, char *valid_lst_file, char *interval_file, char *out_valid_file, char *out_interval_file, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence, char *out_evidence_reduced){
	int ttl_reads=0;
	int len_strand;
	int *valid_lst,ttl_valid;
	Intervals *mts;
	int ttl_mts;
	int i=0;
	Mates *mate_indx=get_mate_index(indx_file, &ttl_reads);
	long *strand_indx=get_strand_index(indx_file, &len_strand);
	//printf(">chrom chr1\n");
	//decision: find_valid_clones vs get em from a file
	if(valid_lst_file==NULL)
		valid_lst=find_valid_clones(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_valid);
	else
		valid_lst=(int*)retrieve_vec(valid_lst_file, sizeof(int), &ttl_valid);
	/*for(i=0;i<ttl_valid;i++){
		int j=valid_lst[i];
		Mates cur=mate_indx[j];
		int mt=cur.mate_indx;
		printf("valid_lst i: %d mate_indx[i].loc: %d mate_indx[i].mate: %d mate_indx[i].mate.loc: %d\n", j, cur.loc, mt, mate_indx[mt].loc);
	}
	exit(0);*/

	//decision: get intervals vs get em from a file
	if(interval_file==NULL)
		mts=mates2intervals(mate_indx, ttl_reads, valid_lst, ttl_valid, &ttl_mts);
	else
		mts=(Intervals*)retrieve_vec(interval_file, sizeof(Intervals), &ttl_mts);
	///decision: perform interval transaction vs store mts in files.
	if(out_valid_file!=NULL)
		store_vec(out_valid_file, valid_lst, sizeof(int), ttl_valid);
	if(out_interval_file!=NULL)
		store_vec(out_interval_file, mts, sizeof(Intervals), ttl_mts);

	if(in_regions!=NULL || in_evidence!=NULL || out_meta_regions!=NULL || out_meta_evidence!=NULL || out_regions!=NULL || out_evidence)
		interval_transaction(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, mts, ttl_mts, histinfo, min_clones, 0, chr_name, in_regions, in_evidence, out_meta_regions, out_meta_evidence, out_regions, out_evidence, out_evidence_reduced, strand_indx, len_strand);


	//clone_coverage(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, min_clones);
	free(mate_indx);
	free(strand_indx);
	free(valid_lst);
	free(mts); //free(rint); free(ev);
}

//if up_low=0 we desire cases that count>=rd_limit
//if up_low=1 we desire cases that count<=rd_limit.
//
//valid_lst is loaded from valid_lst_file, when non null. 
//interval file contains intervals when non null.
//out_valid_file and out_interval_file store the valid list and the intervals when non NULL.
//all the rest files are used for the interval transaction. If all of them are null, the interval transaction does not happen.
void single_read_coverage_transaction(char *indx_file, char *bam_file, Hist_info histinfo, int rd_limit, int up_low, char *chr_name, char *valid_lst_file, char *interval_file,char *out_valid_file, char *out_interval_file, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence, char *out_evidence_reduced){
	int ttl_reads=0;
	int len_strand;

	Intrevidence intrev;
	int *valid_lst,ttl_valid;
	Intervals *mts;
	int ttl_mts;
	int i=0;
	Mates *mate_indx=get_mate_index(indx_file, &ttl_reads);
	long *strand_indx=get_strand_index(indx_file, &len_strand);
	//printf(">chrom chr1\n");
	//decision: find_valid_reads vs get em from a file
	if(valid_lst_file==NULL)
		valid_lst=find_valid_reads(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_valid, -1, -1); ///-- update valid_lst here??????????
	else
		valid_lst=(int*)retrieve_vec(valid_lst_file, sizeof(int), &ttl_valid);

	//decision: find singles2intervals vs get em from a file
	if(interval_file==NULL)
		mts=singles2intervals(mate_indx, ttl_reads, valid_lst, ttl_valid, &ttl_mts);
	else
		mts=(Intervals*)retrieve_vec(interval_file, sizeof(Intervals), &ttl_mts);

	//decision: perform interval_transaction vs store the above two in a file
	if(out_valid_file!=NULL)
		store_vec(out_valid_file, valid_lst, sizeof(int), ttl_valid);
	if(out_interval_file!=NULL)
		store_vec(out_interval_file, mts, sizeof(Intervals), ttl_mts);

	if(in_regions!=NULL || in_evidence!=NULL || out_meta_regions!=NULL || out_meta_evidence!=NULL || out_regions!=NULL || out_evidence)
		interval_transaction(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, mts, ttl_mts, histinfo, rd_limit, up_low, chr_name, in_regions, in_evidence, out_meta_regions, out_meta_evidence, out_regions, out_evidence, out_evidence_reduced, strand_indx, len_strand);
	free(mate_indx);
	free(strand_indx);
	free(valid_lst);
	free(mts);// free(rint); free(ev);

}

//it concatenates lst1 and lst2. ttl_1 is going to be updated with the length of the new list.
//lst1 is a double pointer because its address might change after calls to realloc.
void concatenate_valid_lists(int **lst1, int *lst2, int *ttl1, int ttl2){
	int *tmp=(int*)realloc(*lst1, (*ttl1+ttl2+2)*sizeof(int));
	if(tmp==NULL) ioerror("cannot concatenate arrays");
	int i=0;
	for(i=0;i<ttl2;i++) tmp[*ttl1+i]=lst2[i];
	*ttl1=*ttl1+ttl2;
	*lst1=tmp;
	return;
}

//if outbam_file is NULL it updates only the histogram
//if valid_lst_file is not null it loads the valid list from it
//if out_valid_file is not null it stores the valid list to it.
void single_read_range_transaction(char *indx_file, char *bam_file, Hist_info histinfo, /*char *range_file,*/ char *outbam_file, char *outbam_reduced, char *valid_lst_file, char *out_valid_file){
	int ttl_reads=0;
	int len_strand;
	int *valid_lst=NULL, ttl_valid=0;
	int *new_valid_lst, new_ttl_valid=0;
	int i=0;
	Mates *mate_indx=get_mate_index(indx_file, &ttl_reads);
	long *strand_indx=get_strand_index(indx_file, &len_strand);

	if(valid_lst_file==NULL){
		valid_lst=find_valid_reads(mate_indx, ttl_reads, strand_indx, len_strand, &ttl_valid, -1, -1);
	}
	else
		valid_lst=(int*)retrieve_vec(valid_lst_file, sizeof(int), &ttl_valid);
	////////when there is an outside range file the code should look like this:
	/*char *buf=(char*)malloc(2048*sizeof(char));
	char *tmp; //tokenizer
	long range_st;
	long range_nd;
	FILE *rangefp;
	if(range_file!=NULL)
		rangefp=open_file(range_file);*/

	//decision: find valid_lst vs get it from file
	//parse the range file 
	/****if(valid_lst_file==NULL){
		while(fgets(buf, 1024, rangefp)){
			if(buf[0]=='>') continue; //ignore comments 
			tmp=strtok(buf, " ");
			range_st=atol(tmp);
			tmp=strtok(NULL, " ");
			range_nd=atol(tmp);
			new_valid_lst=find_valid_reads(mate_indx, ttl_reads, strand_indx, len_strand, &new_ttl_valid, range_st, range_nd);
			concatenate_valid_lists(&valid_lst, new_valid_lst, &ttl_valid, new_ttl_valid);
			free(new_valid_lst);
			new_ttl_valid=0;
		}
	}
	else{
		valid_lst=(int*)retrieve_vec(valid_lst_file, sizeof(int), &ttl_valid);
	}*/
	//decision: print rd list vs store it in a file.
	if(out_valid_file!=NULL)
		store_vec(out_valid_file, valid_lst, sizeof(int), ttl_valid);
	
	print_rd_list(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, outbam_file, 0, histinfo); //if outbam_file==NULL it creates only the histogram
	print_reduced_rd_list(mate_indx, ttl_reads, strand_indx, len_strand, valid_lst, ttl_valid, outbam_reduced, 0);
	/*if(rangefp!=NULL) fclose(rangefp);*/
	if(valid_lst!=NULL) free(valid_lst);
	/*if(buf!=NULL) free(buf);*/
	if (mate_indx !=NULL) free(mate_indx);
	if (strand_indx !=NULL) free(strand_indx);

}

//parse the command line and call filter_imported_transaction
void parse_and_call_filter_imported_transaction(char **argv, int argc){
	if(argc!=4) ioerror("usage: all_tools filter_imported <in_prefx> <dest_prefx>");
	char *in_prefx=argv[2];
	char *dst_prefx=argv[3];

	char *in_file=malloc(1024);
	char *out_valid_lst_file=malloc(1024);

	sprintf(in_file, "%s.table",in_prefx);
	sprintf(out_valid_lst_file, "%s.vld", dst_prefx);

	filter_imported_transaction(in_file, out_valid_lst_file);

	free(in_file);
	free(out_valid_lst_file);
}

//parse the command line and call intersect_transaction with the appropriate comments
void parse_and_call_intersect_transaction(char **argv, int argc){
	if(argc!=13) ioerror("usage: all_tools intersect <indx_file1> <type1> <indx_file2> <type2> <bam_file> <dest_prefix> <mode> <chromo_length> <in_prefx_1> <in_prefx_2> <in_join_prefx>");
	char *indx_file1=argv[2];
	char *type1=argv[3]; //imported vs bam_mates vs bam_single
	char *indx_file2=argv[4];
	char *type2=argv[5];
	char *in_bam_file=argv[6];
	char *dst_prefx=argv[7];
	int mode=atoi(argv[8]);
	int chromo_length=atoi(argv[9]);
	char *in_prefx_1=argv[10];
	char *in_prefx_2=argv[11];
	char *in_join_prefx=argv[12];

	char *in_valid_lst_f1=(char*)malloc(2048);
	char *in_valid_lst_f2=(char*)malloc(2048);

	char *out_valid_lst_f1=(char*)malloc(2048);
	char *out_valid_lst_f2=(char*)malloc(2048);
	char *out_bam_file=(char*)malloc(2048);
	char *out_bam_reduced=(char*)malloc(2048);
	char *out_txt1=(char*)malloc(2048);
	char *out_txt2=(char*)malloc(2048);
	char *hist_file=(char*)malloc(2048);
	char *in_join_metafile=(char*)malloc(2048);
	char *out_join_metafile=(char*)malloc(2048);

	sprintf(in_valid_lst_f1,"%s.vld",in_prefx_1);
	sprintf(in_valid_lst_f2,"%s.vld",in_prefx_2);
	sprintf(out_valid_lst_f1,"%s1.vld",dst_prefx);
	sprintf(out_valid_lst_f2,"%s2.vld",dst_prefx);
	sprintf(out_bam_file,"%s.evidence.bam",dst_prefx);
	sprintf(out_bam_reduced,"%s.evidence.bam.short",dst_prefx);
	sprintf(out_txt1,"%s.joined1.txt",dst_prefx);
	sprintf(out_txt2,"%s.joined2.txt",dst_prefx);
	sprintf(hist_file,"%s.hist.txt",dst_prefx);
	
	sprintf(in_join_metafile,"%s.join.meta",in_join_prefx);
	sprintf(out_join_metafile,"%s.join.meta",dst_prefx);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();
	switch(mode){
		/*case 0: //It produces the valid list files only
			intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, NULL, out_valid_lst_f1, out_valid_lst_f2, NULL, NULL, NULL, histinfo);
			break;*/
		case 1: //type1 is bam and type2 is not. It joins and produces meta file, bam and txt
			if( strncmp(type1,"bam",3)!=0 || strcmp(type2, "imported")!=0) ioerror("Invalid type combination for mode 1");
			//intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, in_bam_file, out_valid_lst_f1, out_valid_lst_f2, out_bam_file, NULL, out_txt2, histinfo);
			intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, in_bam_file, out_bam_file, out_bam_reduced, NULL, out_txt2, histinfo, NULL, out_join_metafile);

			break;
		case 2: //type2 is bam and type1 is not. It joins and produces meta file, bam and txt
			if( strncmp(type2,"bam",3)!=0 || strcmp(type1, "imported")!=0) ioerror("Invalid type combination for mode 1");
			//intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, in_bam_file, out_valid_lst_f1, out_valid_lst_f2, out_bam_file, out_txt1, NULL, histinfo);
			intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, in_bam_file, out_bam_file, out_bam_reduced, out_txt1, NULL, histinfo, NULL, out_join_metafile);
			break;
		case 3: //none of type1,2 is bam. It joins and produces out_txt files.
			if( strcmp(type2,"imported")!=0 || strcmp(type1, "imported")!=0) ioerror("Invalid type combination for mode 1");
			//intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, NULL, out_valid_lst_f1, out_valid_lst_f2, NULL, out_txt1, out_txt2, histinfo);
			intersect_transaction(indx_file1, type1, in_valid_lst_f1, indx_file2, type2, in_valid_lst_f2, in_bam_file, out_bam_file, out_bam_reduced, out_txt1, out_txt2, histinfo, NULL, out_join_metafile);
			break;
		case 4: //type1 is bam and type2 is not. It loads a join and produces a new meta file bam and txt.
			if( strncmp(type1,"bam",3)!=0 || strcmp(type2, "imported")!=0) ioerror("Invalid type combination for mode 4");
			intersect_transaction(indx_file1, type1, NULL, indx_file2, type2, NULL, in_bam_file, out_bam_file, out_bam_reduced, NULL, out_txt2, histinfo, in_join_metafile, out_join_metafile);
			break;
		case 5:
			//type2 is bam and type1 is not. It loads a join and produces meta file, bam and txt
			if( strncmp(type2,"bam",3)!=0 || strcmp(type1, "imported")!=0) ioerror("Invalid type combination for mode 1");
			intersect_transaction(indx_file1, type1, NULL, indx_file2, type2, NULL, in_bam_file, out_bam_file, out_bam_reduced, out_txt1, NULL, histinfo, in_join_metafile, out_join_metafile);
			break;
		case 6: 
			//none of type1,2 is bam. It loads a join and produces out_txt files.
			if( strcmp(type2,"imported")!=0 || strcmp(type1, "imported")!=0) ioerror("Invalid type combination for mode 1");
			intersect_transaction(indx_file1, type1, NULL, indx_file2, type2, NULL, in_bam_file, out_bam_file, out_bam_reduced, out_txt1, out_txt2, histinfo, in_join_metafile, out_join_metafile);
			break;
		default:
			ioerror("mode is not supported");
	 }
	
	if(mode==1 || mode==2){
		FILE *histfp=fopen(hist_file,"w");
		print_histogram(histfp, histinfo.mhist, histinfo.chromo_length);
		fclose(histfp);
		destroy_histogram(&(histinfo.mhist));
	}
	return;
}




//(char *indx_file1, char *type1, char *indx_file2, char *type2, char *in_bam_file, char *out_valid_lst_file1, char *out_valid_lst_file2, char *out_bam_file, char *out_txt1, char *out_txt2, Hist_info histinfo)

//parse the command line and call mate_transaction with the appropriate comments
void parse_and_call_mate_transaction(char **argv, int argc){
	if(argc!=10) ioerror("usage: all_tools mates <bam_file> <indx_file> <chromo_length> <chr_name> <mode> <src_prefx> <dest_prefx> <clone_limit>");
	char *bam_file=argv[2];
	char *indx_file=argv[3];
	int chromo_length=atoi(argv[4]);
	char *chr_name=argv[5];
	int mode=atoi(argv[6]);
	char *src_prefx=argv[7];
	char *dst_prefx=argv[8];
	int clone_limit=atoi(argv[9]);
	
	char *out_valid_file=(char*)malloc(2048); 
	char *valid_lst_file=(char*)malloc(2048);
	char *out_interval_file=(char*)malloc(2048);
	char *in_interval_file=(char*)malloc(2048);
	char *in_regions=(char*)malloc(2048); 
	char *in_evidence=(char*)malloc(2048); 
	char *out_meta_regions=(char*)malloc(2048);
	char *out_meta_evidence=(char*)malloc(2048); 
	char *out_regions=(char*)malloc(2048); 
	char *out_evidence=(char*)malloc(2048);
	char *out_evidence_reduced=(char*)malloc(2048);
	char *hist_file=(char*)malloc(2048);


	sprintf(out_valid_file,"%s.vld",dst_prefx);
	sprintf(valid_lst_file,"%s.vld",src_prefx);
	sprintf(in_interval_file,"%s.intrvl",src_prefx);
	sprintf(out_interval_file,"%s.intrvl",dst_prefx);
	sprintf(in_regions,"%s.regs.bit",src_prefx);
	sprintf(in_evidence,"%s.ev.bit",src_prefx);
	sprintf(out_meta_regions,"%s.regs.bit",dst_prefx);
	sprintf(out_meta_evidence,"%s.ev.bit",dst_prefx);
	sprintf(out_regions,"%s.regs.txt",dst_prefx);
	sprintf(out_evidence,"%s.evidence.bam",dst_prefx);
	sprintf(out_evidence_reduced,"%s.evidence.bam.short",dst_prefx);
	sprintf(hist_file,"%s.hist.txt",dst_prefx);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();
	switch(mode){
		case 0: //nothing is preloaded, it performs the entire transaction and outputs all final products.
			mate_transaction(indx_file, bam_file, clone_limit, histinfo, chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence, out_evidence_reduced);
			break;
		case 1: //nothing is preloaded, it creates, and stores in binary format metadata about regions and evidence.
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, NULL, NULL, NULL);
			break;
		case 2: //nothing is preloaded, it creates stores and prints regions only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, NULL, NULL);
			break;
		case 3: //nothing is preloaded, it creates valid lst and intervals only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,NULL, NULL, NULL, out_valid_file, out_interval_file, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			break;
		
		case 4: //it preloads valid list and intervals and creates all final products
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence,out_evidence_reduced);
			break;
		case 5: //it preloads valid list, intervals and meta regions/evidence and prints regions, out evidence
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, out_evidence,out_evidence_reduced);
			break;
		case 6: //it preloads valid list intervals and meta regions/evidence and prints regions only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, NULL, NULL);
			break;
		default:
			ioerror("mode not supported");
			break;
	}


	//mate_transaction(indx_file, bam_file, 5, histinfo);
	FILE *histfp=fopen(hist_file,"w");
	print_histogram(histfp, histinfo.mhist, histinfo.chromo_length);
	fclose(histfp);
	destroy_histogram(&(histinfo.mhist));

}

//parse the command line and call single_coverage_transaction with the appropriate comments
void parse_and_call_single_coverage_transaction(char **argv, int argc){
	if(argc!=11) ioerror("usage: all_tools single_coverage up|low <bam_file> <indx_file> <read_limit> <chromo_length> <chr_name> <mode> <src_prefx> <dest_prefx>");
	char *bam_file=argv[3];
	char *indx_file=argv[4];
	int chromo_length=atoi(argv[6]);
	int up_low;
	int rd_limit=atoi(argv[5]);

	if(strcmp(argv[2], "up")==0)
		up_low=1;
	else if (strcmp(argv[2], "low")==0)
		up_low=0;
	else ioerror("Please choose between up or low");
	char *chr_name=argv[7];
	int mode=atoi(argv[8]);
	char *src_prefx=argv[9];
	char *dst_prefx=argv[10];

	char *out_valid_file=(char*)malloc(2048); 
	char *valid_lst_file=(char*)malloc(2048);
	char *in_interval_file=(char*)malloc(2048);
	char *out_interval_file=(char*)malloc(2048);
	char *in_regions=(char*)malloc(2048); 
	char *in_evidence=(char*)malloc(2048); 
	char *out_meta_regions=(char*)malloc(2048);
	char *out_meta_evidence=(char*)malloc(2048); 
	char *out_regions=(char*)malloc(2048); 
	char *out_evidence=(char*)malloc(2048);
	char *out_evidence_reduced=(char*)malloc(2048);
	char *hist_file=(char*)malloc(2048);

	sprintf(out_valid_file,"%s.vld",dst_prefx);
	sprintf(valid_lst_file,"%s.vld",src_prefx);
	sprintf(in_interval_file,"%s.intrvl",src_prefx);
	sprintf(out_interval_file,"%s.intrvl",dst_prefx);
	sprintf(in_regions,"%s.regs.bit",src_prefx);
	sprintf(in_evidence,"%s.ev.bit",src_prefx);
	sprintf(out_meta_regions,"%s.regs.bit",dst_prefx);
	sprintf(out_meta_evidence,"%s.ev.bit",dst_prefx);
	sprintf(out_regions,"%s.regs.txt",dst_prefx);
	sprintf(out_evidence,"%s.evidence.bam",dst_prefx);
	sprintf(out_evidence_reduced,"%s.evidence.bam.short",dst_prefx);
	sprintf(hist_file,"%s.hist.txt",dst_prefx);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();


switch (mode){
		case 0: //nothing is preloaded, it performs the entire transaction and outputs all final products.
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL,NULL, NULL, NULL, NULL, NULL, NULL, out_regions, out_evidence, out_evidence_reduced);



			break;
		case 1: //nothing is preloaded, it creates, and stores in binary format regions and evidence
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, NULL, NULL, NULL);

			break;
		case 2: //nothing is preloaded, it creates stores and prints regions only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, NULL, NULL);

			break;
		case 3: //nothing is preloaded, it creates valid lst and intervals only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,NULL, NULL, NULL, out_valid_file, out_interval_file, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			break;
		
		case 4: //it preloads valid list and intervals and creates all final products
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence, out_evidence_reduced);

			break;
		case 5: //it preloads valid list, intervals and meta regions/evidence and prints regions, out evidence
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, out_evidence, out_evidence_reduced);

			break;
		case 6: //it preloads valid list intervals and meta regions/evidence and prints regions only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, NULL, NULL);
			break;
		default:
			ioerror("mode not supported");
			break;
	}


	
	//single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, interval_file,out_valid_file, out_interval_file, in_regions, in_evidence, out_meta_regions, out_meta_evidence, out_regions, out_evidence);
	FILE *histfp=fopen(hist_file,"w");
	print_histogram(histfp, histinfo.mhist, histinfo.chromo_length);
	fclose(histfp);
	destroy_histogram(&(histinfo.mhist));

}


void parse_and_call_single_range_transaction(char **argv, int argc){
	//if(argc!=8) ioerror("usage: all_tools single_range <bam_file> <indx_file> <range_file> <chromo_length> <dest_prefx> <mode>"); //this was the initial version
	if (argc!=7) ioerror("usage: all_tools single_range <bam_file> <indx_file> <chromo_length> <dest_prefx> <mode>"); 

	
	char *bam_file=argv[2];
	char *indx_file=argv[3];
	int chromo_length=atoi(argv[4]);
	//char *range_file=argv[4];
	char *prefx=argv[5];
	int mode=atoi(argv[6]);

	char *out_valid_file=(char*)malloc(2048); 
	char *valid_lst_file=(char*)malloc(2048);
	char *outbam_file=(char*)malloc(2048);
	char *outbam_reduced=(char*)malloc(2048);
	char *hist_file=(char*)malloc(2048);

	sprintf(out_valid_file,"%s.vld",prefx);
	sprintf(valid_lst_file,"%s.vld",prefx);
	sprintf(outbam_file,"%s.bam",prefx);
	sprintf(outbam_reduced,"%s.bam.short",prefx);
	sprintf(hist_file,"%s.hist.txt",prefx);


	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();
	
	switch(mode){
		case 0: //nothing is preloaded, it generates a bam file and a valid lst file
			single_read_range_transaction(indx_file, bam_file, histinfo, /*range_file,*/ outbam_file, outbam_reduced, NULL, out_valid_file);
			break;
		case 1: //nothing is preloaded, it generates a valid lst file only
			single_read_range_transaction(indx_file, bam_file, histinfo, /*range_file,*/ outbam_file, outbam_reduced, NULL, out_valid_file);
			break;
		case 2: //It preloads a valid lst and generates a bam file
			single_read_range_transaction(indx_file, bam_file, histinfo, /*NULL,*/ outbam_file, outbam_reduced, valid_lst_file, NULL);
			break;
		default:
			ioerror("mode not supported");
	}
	//single_read_range_transaction(indx_file, bam_file, histinfo, range_file, outbam_file,valid_lst_file, out_valid_file);

	FILE *histfp=fopen(hist_file,"w");
	print_histogram(histfp, histinfo.mhist, histinfo.chromo_length);
	fclose(histfp);
	destroy_histogram(&(histinfo.mhist));

}

void parse_and_call_load_table(char **argv, int argc){
	if(argc!=9) ioerror("usage: all_tools load_table <table_src> <output_prefx> <id_column> <chr_column> <start_column> <end_column> <chr>");
	char *table_fname=argv[2];
	char *outpt_prefx=argv[3];
	int id_col=atoi(argv[4]);
	int chr_col=atoi(argv[5]);
	int st_col=atoi(argv[6]);
	int nd_col=atoi(argv[7]);
	char *out_fname=malloc(2048);
	char *chr=argv[8];
	
	sprintf(out_fname,"%s.table",outpt_prefx);
	
	Imported_info *tbl;
	int ttl_tbl;
	tbl=load_table(table_fname, id_col, st_col, nd_col, chr_col, &ttl_tbl, chr);

	store_imported(out_fname, tbl, ttl_tbl);

	destroy_imported(tbl, ttl_tbl);
	free(out_fname);
	return;
}


int main(int argc, char **argv){
	if(argc<2){
		ioerror("usage: all_tools mates|single_coverage|single_range|load_table|intersect|filter_imported");

	}
	char *command=argv[1];
	if (strcmp(command, "mates")==0){
		parse_and_call_mate_transaction(argv, argc);
	}
	else if (strcmp(command, "single_coverage")==0){
		parse_and_call_single_coverage_transaction(argv, argc);
	}
	else if(strcmp(command, "single_range")==0){
		parse_and_call_single_range_transaction(argv, argc);
	}
	else if(strcmp(command, "load_table")==0){
		parse_and_call_load_table(argv, argc);
	}
	else if(strcmp(command, "intersect")==0){
		parse_and_call_intersect_transaction(argv, argc);
	}
	else if(strcmp(command, "filter_imported")==0){
		parse_and_call_filter_imported_transaction(argv, argc);
	}

	else ioerror("usage: all_tools mates|single_coverage|single_range|load_table|intersect|filter_imported");


	/*char *bam_file=argv[2];
	char *indx_file=argv[3];
	int chromo_length=atoi(argv[4]);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();

	mate_transaction(indx_file, bam_file, 5, histinfo);*/


	
	return 0;
}





