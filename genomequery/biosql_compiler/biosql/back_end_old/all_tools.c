#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "all_tools.h"

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

void interval_transaction(Mates *mate_indx, int ttl_reads, int *valid_lst, int ttl_valid, char *bam_file, Intervals *mts, int ttl_mts, Hist_info histinfo, int coverage_limit, int up_low, char *chr, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence){
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
	printf("out_m_evidence %s out_m_regions: %x\n",out_meta_evidence, out_meta_regions);
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



	free(rint);
	free(ev);

}

//valid_lst is loaded from valid_lst_file, when non null. 
//interval file contains intervals when non null.
//out_valid_file and out_interval_file store the valid list and the intervals when non NULL.
//all the rest files are used for the interval transaction. If all of them are null, the interval transaction does not happen.
void mate_transaction(char *indx_file, char *bam_file, int min_clones, Hist_info histinfo, char *chr_name, char *valid_lst_file, char *interval_file, char *out_valid_file, char *out_interval_file, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence){
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
	printf("valid ttl_valid: %d ttl_mts: %d\n",ttl_valid, ttl_mts);
	///decision: perform interval transaction vs store mts in files.
	printf("out_valid file: %x\n",out_valid_file);
	if(out_valid_file!=NULL)
		store_vec(out_valid_file, valid_lst, sizeof(int), ttl_valid);
	if(out_interval_file!=NULL)
		store_vec(out_interval_file, mts, sizeof(Intervals), ttl_mts);

	printf("out_interval_file: %s\n",out_interval_file);
	printf("min clones: %d\n",min_clones);
	if(in_regions!=NULL || in_evidence!=NULL || out_meta_regions!=NULL || out_meta_evidence!=NULL || out_regions!=NULL || out_evidence)
		interval_transaction(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, mts, ttl_mts, histinfo, min_clones, 0, chr_name, in_regions, in_evidence, out_meta_regions, out_meta_evidence, out_regions, out_evidence);
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
void single_read_coverage_transaction(char *indx_file, char *bam_file, Hist_info histinfo, int rd_limit, int up_low, char *chr_name, char *valid_lst_file, char *interval_file,char *out_valid_file, char *out_interval_file, char *in_regions, char *in_evidence, char *out_meta_regions, char *out_meta_evidence, char *out_regions, char *out_evidence){
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
		interval_transaction(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, mts, ttl_mts, histinfo, rd_limit, up_low, chr_name, in_regions, in_evidence, out_meta_regions, out_meta_evidence, out_regions, out_evidence);
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
void single_read_range_transaction(char *indx_file, char *bam_file, Hist_info histinfo, char *range_file, char *outbam_file, char *valid_lst_file, char *out_valid_file){
	int ttl_reads=0;
	int len_strand;
	int *valid_lst=NULL, ttl_valid=0;
	int *new_valid_lst, new_ttl_valid=0;
	int i=0;
	Mates *mate_indx=get_mate_index(indx_file, &ttl_reads);
	long *strand_indx=get_strand_index(indx_file, &len_strand);
	char *buf=(char*)malloc(2048*sizeof(char));
	char *tmp; //tokenizer
	long range_st;
	long range_nd;
	FILE *rangefp;
	if(range_file!=NULL)
		rangefp=open_file(range_file);

	//decision: find valid_lst vs get it from file
	//parse the range file
	if(valid_lst_file==NULL){
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
	}
	//decision: print rd list vs store it in a file.
	if(out_valid_file!=NULL)
		store_vec(out_valid_file, valid_lst, sizeof(int), ttl_valid);
	
	print_rd_list(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, outbam_file, 0, histinfo); //if outbam_file==NULL it creates only the histogram
	if(rangefp!=NULL) fclose(rangefp);
	if(valid_lst!=NULL) free(valid_lst);
	if(buf!=NULL) free(buf);
	if (mate_indx !=NULL) free(mate_indx);
	if (strand_indx !=NULL) free(strand_indx);

}

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
	sprintf(hist_file,"%s.hist.txt",dst_prefx);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();
	switch(mode){
		case 0: //nothing is preloaded, it performs the entire transaction and outputs all final products.
			mate_transaction(indx_file, bam_file, clone_limit, histinfo, chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence);
			break;
		case 1: //nothing is preloaded, it creates, and stores in binary format metadata about regions and evidence.
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, NULL, NULL);
			break;
		case 2: //nothing is preloaded, it creates stores and prints regions only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, NULL);
			break;
		case 3: //nothing is preloaded, it creates valid lst and intervals only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,NULL, NULL, NULL, out_valid_file, out_interval_file, NULL, NULL, NULL, NULL, NULL, NULL);
			break;
		
		case 4: //it preloads valid list and intervals and creates all final products
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence);
			break;
		case 5: //it preloads valid list, intervals and meta regions/evidence and prints regions, out evidence
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, out_evidence);
			break;
		case 6: //it preloads valid list intervals and meta regions/evidence and prints regions only
			mate_transaction(indx_file, bam_file, clone_limit, histinfo,chr_name, valid_lst_file, in_interval_file, NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, NULL);
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
	sprintf(hist_file,"%s.hist.txt",dst_prefx);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();


switch (mode){
		case 0: //nothing is preloaded, it performs the entire transaction and outputs all final products.
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL,NULL, NULL, NULL, NULL, NULL, NULL, out_regions, out_evidence);



			break;
		case 1: //nothing is preloaded, it creates, and stores in binary format regions and evidence
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, NULL, NULL);

			break;
		case 2: //nothing is preloaded, it creates stores and prints regions only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, NULL, NULL, NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, NULL);

			break;
		case 3: //nothing is preloaded, it creates valid lst and intervals only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,NULL, NULL, NULL, out_valid_file, out_interval_file, NULL, NULL, NULL, NULL, NULL, NULL);
			break;
		
		case 4: //it preloads valid list and intervals and creates all final products
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, NULL, NULL, out_meta_regions, out_meta_evidence, out_regions, out_evidence);

			break;
		case 5: //it preloads valid list, intervals and meta regions/evidence and prints regions, out evidence
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, out_evidence);

			break;
		case 6: //it preloads valid list intervals and meta regions/evidence and prints regions only
			single_read_coverage_transaction(indx_file, bam_file, histinfo, rd_limit, up_low,chr_name, valid_lst_file, in_interval_file,NULL, NULL, in_regions, in_evidence, NULL, NULL, out_regions, NULL);
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
	if(argc!=8) ioerror("usage: all_tools single_range <bam_file> <indx_file> <range_file> <chromo_length> <dest_prefx> <mode>");
	
	char *bam_file=argv[2];
	char *indx_file=argv[3];
	int chromo_length=atoi(argv[5]);
	char *range_file=argv[4];
	char *prefx=argv[6];
	int mode=atoi(argv[7]);

	char *out_valid_file=(char*)malloc(2048); 
	char *valid_lst_file=(char*)malloc(2048);
	char *outbam_file=(char*)malloc(2048);
	char *hist_file=(char*)malloc(2048);

	sprintf(out_valid_file,"%s.vld",prefx);
	sprintf(valid_lst_file,"%s.vld",prefx);
	sprintf(outbam_file,"%s.bam",prefx);
	sprintf(hist_file,"%s.hist.txt",prefx);


	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();
	
	switch(mode){
		case 0: //nothing is preloaded, it generates a bam file and a valid lst file
			single_read_range_transaction(indx_file, bam_file, histinfo, range_file, outbam_file, NULL, out_valid_file);
			break;
		case 1: //nothing is preloaded, it generates a valid lst file only
			single_read_range_transaction(indx_file, bam_file, histinfo, range_file, outbam_file,NULL, out_valid_file);
			break;
		case 2: //It preloads a valid lst and generates a bam file
			single_read_range_transaction(indx_file, bam_file, histinfo, NULL, outbam_file,valid_lst_file, NULL);
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


int main(int argc, char **argv){
	if(argc<2){
		ioerror("usage: all_tools mates|single_coverage|single_range");

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
	else ioerror("usage: all_tools mates|single_coverage|single_range");


	/*char *bam_file=argv[2];
	char *indx_file=argv[3];
	int chromo_length=atoi(argv[4]);

	Hist_info histinfo;
	histinfo.chromo_length=chromo_length;
	histinfo.mhist=create_histogram();

	mate_transaction(indx_file, bam_file, 5, histinfo);*/


	
	return 0;
}





