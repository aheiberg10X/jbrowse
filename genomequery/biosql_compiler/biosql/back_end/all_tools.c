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


void concatenate_valid_lists(int **lst1, int *lst2, int *ttl1, int ttl2){
	int *tmp=(int*)realloc(*lst1, (*ttl1+ttl2+2)*sizeof(int));
	if(tmp==NULL) ioerror("cannot concatenate arrays");
	int i=0;
	for(i=0;i<ttl2;i++) tmp[*ttl1+i]=lst2[i];
	*ttl1=*ttl1+ttl2;
	*lst1=tmp;
	return;
}


//It generates a valid list of ttl_valid entries and stores it to dst_file.
//If valid_lst_in (found in in_valid_file) is non NULL, the filtering considers only those reads that are present in valid_lst_in.
void filter_reads(char *indx_file, char *in_valid_file, char *dst_file){
	int ttl_reads;
	Mates *read_indx=get_mate_index(indx_file, &ttl_reads);
	int len_strand;
	long *strand_indx=get_strand_index(indx_file, &len_strand);
	int *valid_lst=NULL, ttl_valid=0; 
	int *valid_lst_in=NULL,	ttl_valid_in=0;

	int cnt=0;
	int i=0;
	int cur_i=0;
	
	if(in_valid_file!=NULL)
		valid_lst_in=(int*)retrieve_vec(in_valid_file, sizeof(int), &ttl_valid_in);

	///stupid method: First iterate read_indx to find n and then iterate indx again to populate ret
	for(i=1;i<ttl_reads;i++){ //iterate over either reads or valid_lst_in
		if(valid_lst_in!=NULL){
			if(i==ttl_valid_in) break;
			else cur_i=valid_lst_in[i];
		}
		else cur_i=i;
		if(is_valid(read_indx, cur_i, strand_indx, len_strand)>0) cnt+=1;
	}
	valid_lst=(int*)malloc(sizeof(int)*(cnt+1));
	ttl_valid=cnt;
	cnt=0;
	for(i=1;i<ttl_reads;i++){
		if(valid_lst_in!=NULL){
			if(i==ttl_valid_in) break;
			else cur_i=valid_lst_in[i];
		}
		else cur_i=i;
		if(is_valid(read_indx, cur_i, strand_indx, len_strand)>0){
			valid_lst[cnt]=cur_i;
			cnt+=1;
		}
	}	
	if(dst_file!=NULL)
		store_vec(dst_file, valid_lst, sizeof(int), ttl_valid);
	if (read_indx!=NULL) free(read_indx);
	if (strand_indx!=NULL) free(strand_indx);
	if (valid_lst!=NULL) free(valid_lst);
	if (valid_lst_in!=NULL) free(valid_lst_in);
}

//It generates a valid list of ttl_valid entries and stores it to dst_file.
//If valid_lst_in (found in in_valid_file) is non NULL, the filtering considers only those reads that are present in valid_lst_in.
void filter_imported(char *f_tbl_indx, char *in_valid_file, char *dst_file){	
	int ttl_table=0;
	Imported_info *tbl=(Imported_info*)retrieve_vec(f_tbl_indx, sizeof(Imported_info),&ttl_table);
	int *valid_lst_in=NULL, ttl_valid_in=0;

	if(in_valid_file!=NULL)
		valid_lst_in=(int*)retrieve_vec(in_valid_file, sizeof(int), &ttl_valid_in);

	int *ret;
	int cnt=0;
	int i=0;
	int cur_i=0;

	for(i=0;i<ttl_table;i++){
		if(valid_lst_in!=NULL){
			if(i==ttl_valid_in) break;
			else cur_i=valid_lst_in[i];
		}
		else cur_i=i;
		if (isvalid_imported(tbl, cur_i, ttl_table)>0) cnt+=1;
		i++;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	i=0;
	for(i=0;i<ttl_table;i++){
		if(valid_lst_in!=NULL){
			if(i==ttl_valid_in) break;
			else cur_i=valid_lst_in[i];
		}
		else cur_i=i;
		if(isvalid_imported(tbl, i, ttl_table)>0) ret[cnt++]=i;
		i++;
	}
	ttl_table=cnt;
	store_vec(dst_file, ret, sizeof(int), ttl_table);
	if (tbl!=NULL) free(tbl);
	if (ret!=NULL) free(ret);
	if (valid_lst_in!=NULL) free(valid_lst_in);

}

//It generates an array of Join of ttl_valid entries and stores it to dst_file.
//fjoin is the file that contains a join list, findx1 contains the index 
//of pointers indx1, and findx2 contains the index of pointers indx2.
void filter_join(char *fjoin, char *findx1, char *findx2, char *dst_file, char *type1, char *type2){
	int ttl_join=0;
	Join *join_lst=(Join*)retrieve_vec(fjoin, sizeof(Join), &ttl_join);
	Mates *read_indx1=NULL, *read_indx2=NULL;
	long *strand_indx1=NULL, *strand_indx2=NULL;
	int ttl_strand1=0, ttl_strand2=0;
	Imported_info *imp1=NULL, *imp2=NULL;
	int ttl_rd1=0, ttl_rd2=0, ttl_imp1=0, ttl_imp2=0;
	Join *ret=NULL;
	int ttl_ret=0;

	if(strcmp(type1, "reads")==0){
		read_indx1=get_mate_index(findx1, &ttl_rd1);
		strand_indx1=get_strand_index(findx1, &ttl_strand1);
	}
	if(strcmp(type2, "reads")==0){
		read_indx2=get_mate_index(findx2, &ttl_rd2);
		strand_indx2=get_strand_index(findx2, &ttl_strand2);
	}
	if (strcmp(type1, "imported")==0){
		imp1=(Imported_info*)retrieve_vec(findx1, sizeof(Imported_info), &ttl_imp1);
	}
	if(strcmp(type2, "imported")==0){
		imp2=(Imported_info*)retrieve_vec(findx2, sizeof(Imported_info), &ttl_imp2);
	}
	int i=0;
	int cnt=0;
	for(i=0;i<ttl_join;i++){
		if(isvalid_join(join_lst, i, read_indx1, strand_indx1, read_indx2, strand_indx2, imp1, imp2)>0)
			cnt++;
	}
	ret=(Join*)malloc(sizeof(Join)*(cnt+1));
	cnt=0;
	for(i=0;i<ttl_join;i++){
		if(isvalid_join(join_lst, i, read_indx1, strand_indx1, read_indx2, strand_indx2, imp1, imp2)>0)
			ret[cnt++]=join_lst[i];
	}
	ttl_ret=cnt;
	//if the implementation changes and the function returns ret, we should also reallocate the memory of ret
	//in case ret ttl_ret<ttl_join.
	store_vec(dst_file, ret, sizeof(Join), ttl_ret);
	if(read_indx1!=NULL) free(read_indx1);
	if(read_indx2!=NULL) free(read_indx2);
	if(strand_indx1!=NULL) free(strand_indx1);
	if(imp1!=NULL) free(imp1);
	if(strand_indx2!=NULL) free(strand_indx2);
	if(imp2!=NULL) free(imp2);
	if(ret!=NULL) free(ret);
}
		

//it creates intervals for those reads that appear in the valid_lst.
void reads2intervals(char *indx_file, char *in_valid_file, char *dst_file){
	int ttl_reads;
	Mates *read_indx=get_mate_index(indx_file, &ttl_reads);
	int len_strand;
	long *strand_indx=get_strand_index(indx_file, &len_strand);
	int *valid_lst=NULL, ttl_valid=0;
	valid_lst=(int*)retrieve_vec(in_valid_file, sizeof(int), &ttl_valid);
	int i=0, cur_i=0;
	
	Intervals *ret=(Intervals*)malloc(sizeof(Intervals)*ttl_valid);
	int ttl_intrvl=0;
	Intervals tmp;
	for(i=0;i<ttl_valid;i++){
		cur_i=valid_lst[i];
		tmp=get_rd_interval(read_indx, cur_i);
		if(tmp.rd1<0 && tmp.rd2<0)
			continue;
		if(tmp.st>tmp.nd)
			continue;
		ret[ttl_intrvl++]=tmp;
	}
	//if the implementation changes and the function returns ret, we should also reallocate the memory of ret
	//in case ret ttl_intrvl<ttl_valid
	store_vec(dst_file, ret, sizeof(Intervals), ttl_intrvl);
	if (read_indx!=NULL) free(read_indx);
	if (strand_indx!=NULL) free(strand_indx);
	if (valid_lst!=NULL) free(valid_lst);
	if (ret!=NULL) free(ret);
}

//it creates intervals for those imported entries that appear in dst_file
void imported2intervals(char *imported_index_file, char *in_valid_file, char *dst_file){
	int ttl;
	Imported_info *indx=(Imported_info*)retrieve_imported(imported_index_file, &ttl);
	int *valid_lst=NULL, ttl_valid=0;
	int ttl_intrvl=0, ttl_iter=0;
	int i, cur_i;
	if (in_valid_file!=NULL){
		valid_lst=(int*)retrieve_vec(in_valid_file, sizeof(int), &ttl_valid);
		ttl_iter=ttl_valid;
	}
	else ttl_iter=ttl;

	Intervals *ret=(Intervals*)malloc(sizeof(Intervals)*(ttl_iter+2));
	Intervals tmp;
	ttl_intrvl=0;
	for(i=0;i<ttl_iter;i++){
		if (valid_lst!=NULL)
			cur_i=valid_lst[i];
		else cur_i=i;
		tmp=get_imported_interval(indx, cur_i);
		if(tmp.rd1<0 && tmp.rd2<0)
			continue;
		if(tmp.st>tmp.nd)
			continue;
		ret[ttl_intrvl++]=tmp;
	}
	//if the implementation changes and the function returns ret, we should also reallocate the memory of ret
	//in case ret ttl_intrvl<ttl_iter.
	store_vec(dst_file, ret, sizeof(Intervals), ttl_intrvl);
	if (indx!=NULL) free(indx);
	if (valid_lst!=NULL) free(valid_lst);
	if (ret!=NULL) free(ret);

}

/*intrvl_file contains an array of intervals. The function calls the balance parenthesis
*routine and the obtained products are stored to the output files specifically. 
*Note that the contents of fdst_valid_lst contain a valid list with pointers that come from
*the rd fields of the intervals (e.g. pointers to Mates). fdst_regions stores 
*the resulted intervals*/
void filter_intervals(char *intrvl_file, char *fdst_valid_lst, char *fdst_regions){
	int ttl_intrvl;
	int coverage;
	int up_low;
	Intervals *intrvl=retrieve_vec(intrvl_file, sizeof(Intervals), &ttl_intrvl);
	Intrevidence intrev;

	coverage=get_interval_coverage_param(&up_low); //customizable from the input file
	
	if(up_low==0)
		intrev=min_interval_coverage(intrvl, ttl_intrvl, coverage);
	else if(up_low==1)
		intrev=max_interval_coverage(intrvl, ttl_intrvl, coverage);

	store_vec(fdst_valid_lst, intrev.evidence, sizeof(int), intrev.ttl_evidence);
	store_vec(fdst_regions, intrev.regions, sizeof(Intervals), intrev.ttl_regions);

	if(intrvl!=NULL) free(intrvl);
	if(intrev.evidence!=NULL) free(intrev.evidence);
	if(intrev.regions!=NULL) free(intrev.regions);
}

//It performs mapjoin (i.e. call to intersect function) with the sets of intervals
//from the two input files. The resulted array is stored to dst.
void mapjoin_intervals(char *fintrvl1, char *fintrvl2, char *dst){
	Intervals *int1=NULL, *int2=NULL;
	int ttl_int1=0, ttl_int2=0;

	Join *join_lst=NULL;
	int ttl_join=0;

	int1=retrieve_vec(fintrvl1, sizeof(Intervals), &ttl_int1);
	int2=retrieve_vec(fintrvl2, sizeof(Intervals), &ttl_int2);
	join_lst=join_intervals(int1, int2, ttl_int1, ttl_int2, &ttl_join);

	store_vec(dst, join_lst, sizeof(Join), ttl_join);


	if(int1!=NULL) free(int1);
	if(int2!=NULL) free(int2);
	if (join_lst!=NULL) free(join_lst);

}

//It prints those reads from bam_file that are contained at the valid_lst_file.
//indx_file contains the mates indx. The function also produces a short version
//of the output, called out_visual_file, which contains only chromosome 
//coordinates of the reads. We also create a histogram file. If print_mates=1
//the out_visual_file will have mate information in a single column.
void print_bam(char *bam_file, char *indx_file, char *valid_lst_file, char *out_bam_file, char *out_visual_file, char *histogram_file, int print_mates, int chr_len){
	int ttl_reads=0;
	Mates *read_indx=get_mate_index(indx_file, &ttl_reads);
	int strand_len=0;
	long *strand_indx=get_strand_index(indx_file, &strand_len);
	int ttl_valid=0;
	int *valid_lst=(int*)retrieve_vec(valid_lst_file, sizeof(int), &ttl_valid);

	Hist_info histinfo;
	histinfo.chromo_length=chr_len;
	histinfo.mhist=create_histogram();
	FILE *histfp=fopen(histogram_file,"w");
	print_rd_list(read_indx, ttl_reads, valid_lst, ttl_valid, bam_file, out_bam_file, print_mates, histinfo);
	print_reduced_rd_list(read_indx, ttl_reads, strand_indx, strand_len, valid_lst, ttl_valid, out_visual_file, print_mates);

	print_histogram(histfp, histinfo.mhist, histinfo.chromo_length);
	fclose(histfp);
	
	destroy_histogram(&(histinfo.mhist));
	if (read_indx!=NULL) free(read_indx);
	if (strand_indx!=NULL) free(strand_indx);
	if (valid_lst!=NULL) free(valid_lst);
}

void print_imported(char *tbl_file, char *fvalid_lst, char *out_file){
	int ttl_tbl;
	Imported_info *tbl=retrieve_imported(tbl_file, &ttl_tbl);
	int ttl_valid;
	int *valid_lst=retrieve_vec(fvalid_lst, sizeof(int), &ttl_valid);
	print_imported_list(out_file, tbl, ttl_tbl, valid_lst, ttl_valid);

	if(tbl!=NULL) free(tbl);
	if(valid_lst!=NULL) free(valid_lst);
}

//it creates a valid_lst with all unique values of either indx1 or indx2, 
//depending on the value of side, of the join lst.
void isolate_join(char *fjoin_lst, int side, char *out_file){
	int ttl_join=0;
	Join *join_lst=retrieve_vec(fjoin_lst, sizeof(Join), &ttl_join);
	int ttl_valid=0;
	int *valid_lst=isolate_join_indexes(join_lst, ttl_join, &ttl_valid, side);

	store_vec(out_file, valid_lst, sizeof(int), ttl_valid);
	if(join_lst!=NULL) free(join_lst);
	if(valid_lst!=NULL) free(valid_lst);
}
void load_new_table(char *src_file, char *outfile, int id_col, int chr_col, int st_col, int nd_col, char *chr){
	int ttl_tbl=0;
	Imported_info *tbl=load_table(src_file, id_col, st_col, nd_col, chr_col, &ttl_tbl, chr);
	store_imported(outfile, tbl, ttl_tbl);
}


int main(int argc, char **argv){
	if(argc<2){
		ioerror("usage: all_tools filter_intervals|filter_reads|filter_join|filter_imported|reads2intervals|imported2intervals|mapjoin|print_reads|print_imported|isolate_valid|load_table\n");
	}
	char *cmd=argv[1];
	if(strcmp(argv[1],"filter_intervals")==0){
		if(argc!=5) ioerror("usage: all_tools filter_intervals <intrvl_file> <dest_valid_file> <dest_region_file>\n");
		char *intrvl_file=argv[2];
		char *dst_vld_file=argv[3];
		char *dst_reg_file=argv[4];
		filter_intervals(intrvl_file, dst_vld_file, dst_reg_file);
		return;
	}
	if(strcmp(argv[1],"filter_reads")==0){
		if(argc!=5 && argc!=4) ioerror("usage: all_tools filter_reads <indx_file> <dest_valid_file> [<in_valid_file>]\n");
		char *indx_file=argv[2];
		char *dst_vld_file=argv[3];
		char *in_vld_file=NULL;
		if(argc==5) in_vld_file=argv[4];
		filter_reads(indx_file, in_vld_file, dst_vld_file);
		return;
	}
	if(strcmp(argv[1],"filter_imported")==0){
		if(argc!=5 && argc!=4) ioerror("usage: all_tools filter_imported <indx_file> <dest_valid_file> [<in_valid_file>]\n");
		char *indx_file=argv[2];
		char *dst_vld_file=argv[3];
		char *in_vld_file=NULL;
		if(argc==5) in_vld_file=argv[4];
		filter_imported(indx_file, in_vld_file, dst_vld_file);
		return;
	}
	if(strcmp(argv[1],"filter_join")==0){
		if(argc!=8)ioerror("usage: all_tools filter_join <join_indx_file> <indx1_file> <indx2_file> <dst_file> <type1> <type2>\n");
		char *join_indx=argv[2];
		char *indx1=argv[3];
		char *indx2=argv[4];
		char *dst=argv[5];
		char *type1=argv[6];
		char *type2=argv[7];
		filter_join(join_indx, indx1, indx2, dst, type1, type2);
		return;
	}
	if(strcmp(argv[1],"reads2intervals")==0){
		if(argc!=5) ioerror("usage: all_tools reads2intervals <indx_file> <valid_file> <dest_file>\n");
		char *indx_file=argv[2];
		char *vld_file=argv[3];
		char *dst_file=argv[4];
		reads2intervals(indx_file, vld_file, dst_file);
		return;
	}
	if(strcmp(argv[1],"imported2intervals")==0){
		if(argc!=5 && argc!=4) ioerror("usage: all_tools imported2intervals <imported_indx_file> <dest_file> [<valid_file>]\n");
		char *indx_file=argv[2];
		char *dst_file=argv[3];
		char *vld_file=NULL;
		if(argc==5) vld_file=argv[4];
		imported2intervals(indx_file, vld_file, dst_file);
		return;
	}
	if(strcmp(argv[1],"mapjoin")==0){
		if(argc!=5) ioerror("usage: all_tools mapjoin <intrvl1_file> <intrvl2_file> <dest_file>\n");
		char *intrvl1_file=argv[2];
		char *intrvl2_file=argv[3];
		char *dst_file=argv[4];
		mapjoin_intervals(intrvl1_file, intrvl2_file, dst_file);
		return;
	}
	if(strcmp(argv[1],"print_imported")==0){
		if(argc!=5) ioerror("usage: all_tools print_imported <imported_indx_file> <valid_file> <dest_file>\n");
		char *tbl_file=argv[2];
		char *vld_file=argv[3];
		char *dst_file=argv[4];
		print_imported(tbl_file, vld_file, dst_file);
		return;
	}
	if(strcmp(argv[1],"print_reads")==0){
		if(argc!=10) ioerror("usage: all_tools print_reads <bam_file> <indx_file> <valid_lst_file> <dest_file> <dest_reduced_file> <hist_file> <mates_en> <chr_len>\n");
		char *bam_file=argv[2];
		char *indx_file=argv[3];
		char *vld_file=argv[4];
		char *dst_file=argv[5];
		char *dst_reduced=argv[6];
		char *hist_file=argv[7];
		int mates_en=atoi(argv[8]);
		int chr_len=atoi(argv[9]);
		print_bam(bam_file, indx_file, vld_file, dst_file, dst_reduced, hist_file, mates_en, chr_len);
		return;
	}
	if(strcmp(argv[1],"isolate_join")==0){
		if(argc!=5) ioerror("usage: all_tools isolate_join <join_indx_file> <1/2> <dest_file>\n");
		char *indx_file=argv[2];
		int side=atoi(argv[3]);
		char *dst_file=argv[4];
		isolate_join(indx_file, side, dst_file);
		return;
	}

	if(strcmp(argv[1],"load_table")==0){
		if(argc!=9) ioerror("usage: all_tools load_table <table_src> <dest_file> <id_col> <chr_col> <start_col> <end_col> <chr>\n");
		char *src_file=argv[2];
		char *dst_file=argv[3];
		int id_col=atoi(argv[4]);
		int chr_col=atoi(argv[5]);
		int start_col=atoi(argv[6]);
		int end_col=atoi(argv[7]);
		char *chr=argv[8];
		load_new_table(src_file, dst_file, id_col, chr_col, start_col, end_col, chr);
		return;
	}
}

