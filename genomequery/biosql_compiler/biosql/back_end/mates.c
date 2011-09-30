#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include"defs.h"
#include<bam.h>
#include<sam.h>

typedef struct{
	Mates *mates;
	Ins_len_groups *groups;
} MatesGroups;

typedef struct{
	Mates *indx;
	int *valid_lst;
	int valid_st;
	int valid_nd;
	int region_st;
	int region_nd;
	samfile_t *fp;
	samfile_t *outfp;
	long *just_printed; //a bitmap with ones on the index locations of those reads that have been already printed.
	int len_just_printed;
} print_reads_t;
//It reads the index and the bas info from mate file. ttl_reads is the length of the index table. ttl_groups is the length of the 
//Ins_len_groups table
Mates *get_mate_index(char *mate_file, int *ttl_reads){
	time_t t1=time(NULL);
	FILE *fp=open_file(mate_file);
	Mates *ret;//=(MatesGroups*)malloc(sizeof(MatesGroups));
	fread(ttl_reads, sizeof(int), 1, fp);
	ret=(Mates*)malloc(sizeof(Mates)*(*ttl_reads+1));
	fread(ret, sizeof(Mates), *ttl_reads, fp);
	
	/*fread(ttl_groups, sizeof(int), 1, fp);
	ret->groups=(Ins_len_groups*)malloc(sizeof(Ins_len_groups)*(*ttl_groups+1));
	fread(ret->groups, sizeof(Ins_len_groups), *ttl_groups, fp);*/

	fclose(fp);
	time_t t2=time(NULL);
	int i;
	//printf("get_index time: %lf\n",difftime(t2,t1));
	return ret;
}

//Returns F if the respective bit of the strand_indx is 0, R otherwise
inline char get_strand(long *strand_indx, int strand_len, int pos){
	if (strand_len*sizeof(long)*8<=pos) ioerror("Strand position exceeds index boundaries");
	int x=pos/(sizeof(long)*8); //don't forget to convert the bytes to bits
	int y=pos%(sizeof(long)*8);
	unsigned long mask=1;
	mask=mask<<y;
	if ((strand_indx[x]&mask) == 0) return 'F';
	else return 'R';
}


long *get_strand_index(char *mate_file, int *len){
	FILE *fp=open_file(mate_file);
	long *ret;//=(MatesGroups*)malloc(sizeof(MatesGroups));
	int ttl_reads;
	fread(&ttl_reads, sizeof(int), 1, fp);
	fseek(fp, sizeof(Mates)*ttl_reads, SEEK_CUR);
	fread(len, sizeof(int), 1, fp);
	ret=(long*)malloc(sizeof(long)*(*len));
	fread(ret, sizeof(long), *len, fp);
	return ret;
}
//Checks if the clone with one end on indx[i] has a distance within med+min_dist_mult*sv and med+max_dist_mult*sv
//Returns 1 on success, 0 on failure. In case the clone is dangling it returns -1.
/*inline int valid_distance(Mates *indx, int i, int min_dist, int max_dist){
	int dist;
	int mate_indx=indx[i].mate_indx;
	//int grp_i=0;
	//int min_dist, max_dist;
	if(mate_indx<0 || indx[i].loc<0) return -1;
	if(indx[mate_indx].loc<0) return -1;
	if (indx[i].loc<indx[mate_indx].loc)
		dist=indx[mate_indx].loc + indx[mate_indx].read_len-indx[i].loc;
	else
		dist=indx[i].loc + indx[i].read_len-indx[mate_indx].loc;
	//fprintf(stderr, "dist: %d len: %d old_dist: %d min_dist: %d\n",dist, indx[i].read_len, indx[mate_indx].loc-indx[i].loc,min_dist);

	/////////////dist=indx[i].loc-indx[mate_indx].loc;
	//printf(">dist len: %d sd: %d min_dist: %d max_dist: %d\n", grps[grp_i].len, grps[grp_i].sd, min_dist, max_dist);
	if(dist<0) ioerror("Negative Distance");///////////dist=-dist;
	if (min_dist<=dist && dist<=max_dist) return 1;
	else return 0;
}*/

//checks if the clone with one end on indx[i] follows the orientation implied by lstrand and rstrand. 
//It returns 1 on FAILURE 0 on success -1 if the clone has a dangling end and -2 if it has a large insert size.
/*inline int invalid_orientation(Mates *indx, int i, long *strand_indx, int strand_len, char lstrand, char rstrand){
	int mate_indx=indx[i].mate_indx;
	int dist;
	if (mate_indx<0 || indx[i].loc<0) return -1;
	if(indx[mate_indx].loc < 0 ) return -1;
	if (indx[i].loc<indx[mate_indx].loc)
		dist=indx[mate_indx].loc + indx[mate_indx].read_len-indx[i].loc;
	else
		dist=indx[i].loc + indx[i].read_len-indx[mate_indx].loc;
	if(dist>1000000) return -2;
	char st1=get_strand(strand_indx, strand_len, i);
	char st2=get_strand(strand_indx, strand_len, mate_indx);
	//printf("st1: %c i: %d st2: %c mate: %d\n",st1, i, st2, mate_indx);
	if (i<mate_indx){
		if(st1==lstrand && st2==rstrand) return 0; else return 1;
	}
	else{
		if(st1==rstrand && st2==lstrand) return 0; else return 1;
	}
}*/

//checks if the clone with one end on indx[i] falls within [reg_st, reg_nd] and
//its mate is dangling. It returns 1 on success, 0 otherwise
/*inline int dangling_end(Mates *indx, int i, int reg_st, int reg_nd){
	int mate_indx=indx[i].mate_indx;
	if (mate_indx>=0 || indx[i].loc<0) return 0; //return 0 if either the 
		//current end is dangling or the mate maps somewhere.
	if (!clone_overlaps_with_region(reg_st, reg_nd, indx[i].loc, indx[i].loc+indx[i].read_len-1)) return 0; //clone out of region
	else return 1;
}*/



/*It returns an array of integers of length n. The array contains all indices of indx whose clones
succeed the valid_distance test.
*/
/*int *find_dist_valid_clones(Mates *indx, int ttl_reads, int min_dist, int max_dist, int *n){
	int i=0;
	int *ret;
	///stupid method: First iterate indx to find n and then iterate indx again to populate ret
	int cnt=0;
	time_t t1=time(NULL);
	for (i=0;i<ttl_reads;i++){
		if(valid_distance(indx, i, min_dist, max_dist)>0) cnt+=1;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	for (i=0;i<ttl_reads;i++){
		if(valid_distance(indx, i, min_dist, max_dist)>0){
			ret[cnt]=i;
			//printf("cnt: %d i: %d loc: %d mate: %d test:%d \n",cnt, i, indx[i].loc, indx[i].mate_indx, valid_distance(indx, i, min_dist, max_dist));
			cnt+=1;
		}
	}
	//exit(0);
	(*n)=cnt;
	time_t t2=time(NULL);
	fprintf(stderr, "valid clones time: %lf cnt: %d\n",difftime(t2,t1), cnt);
	return ret;
}*/

/*It returns an array of integers of length n. The array contains all indices of indx whose clones 
succeed the invalid_orientation test.
*/
/*int *find_ornt_invalid_clones(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, char left_strand, char right_strand, int *n){
	int i=0;
	int *ret;
	///stupid method: First iterate indx to find n and then iterate indx again to populate ret
	int cnt=0;
	time_t t1=time(NULL);
	for (i=0;i<ttl_reads;i++){
		if(invalid_orientation(indx, i, strand_indx, strand_len, left_strand, right_strand)>0) cnt+=1;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	for (i=0;i<ttl_reads;i++){
		if(invalid_orientation(indx, i, strand_indx, strand_len, left_strand, right_strand)>0){
			ret[cnt]=i;
			//printf("cnt: %d i: %d loc: %d mate: %d test:%d \n",cnt, i, indx[i].loc, indx[i].mate_indx, invalid_orientation(indx, i, strand_indx, strand_len, left_strand, right_strand));
			//exit(0);
			cnt+=1;
		}
	}
	(*n)=cnt;
	time_t t2=time(NULL);
	fprintf(stderr, "valid clones time: %lf cnt: %d\n",difftime(t2,t1), cnt);
	return ret;
}*/

/*It returns an array of integers of length n. The array contains all indices 
of indx whose clones succeed the dangling_end test for the region [reg_st, reg_nd].
*/
/*int *find_dangling_clones(Mates *indx, int ttl_reads, int reg_st, int reg_nd, int *n){
	int i=0;
	int *ret;
	///stupid method: First iterate indx to find n and then iterate indx again to populate ret
	int cnt=0;
	time_t t1=time(NULL);
	for (i=0;i<ttl_reads;i++){
		if(dangling_end(indx, i, reg_st, reg_nd)) cnt+=1;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	for (i=0;i<ttl_reads;i++){
		if(dangling_end(indx, i, reg_st, reg_nd)){
			ret[cnt]=i;
			cnt+=1;
		}
	}
	(*n)=cnt;
	time_t t2=time(NULL);
	fprintf(stderr, "valid clones time: %lf cnt: %d\n",difftime(t2,t1), cnt);
	return ret;
}*/
/*Find the min index of valid_lst for which a clone of indx has loc in the 
range between region_st and region_nd. prev_valid_indx_start is the previous
value that this function returned.
*/
int get_starting_point(Mates *indx, int *valid_lst, int prev_valid_indx_start, int valid_indx_end, int region_st, int region_nd){
	if(prev_valid_indx_start<0) prev_valid_indx_start=-1;
	int i=0;
	int st=0,nd=0;
	int loc1=0, loc2=0;
	for(i=prev_valid_indx_start;i<=valid_indx_end;i++){ //verify whether i=prev or i=prev+1
		if(i<0) continue;
		loc1=indx[valid_lst[i]].loc;
		loc2=indx[indx[valid_lst[i]].mate_indx].loc;
		if(loc1<loc2){
			st=loc1;
			nd=loc2/*+indx[valid_lst[i]].read_len*/;
		}
		else{
			st=loc2;
			nd=loc1/*+indx[valid_lst[i]].read_len*/;
		}
		//st=min(indx[valid_lst[i]].loc,indx[indx[valid_lst[i]].mate_indx].loc);
		//nd=max(indx[valid_lst[i]].loc,indx[indx[valid_lst[i]].mate_indx].loc);
		if( (region_st<=st && st<=region_nd) || (st<region_st && nd > region_st))
			return i;
	}
}

//fp is a file pointer to raw data
//offst is the number of bytes to be skipped from the beginning of the file.
//ttl_reads is the total number of alignments to be fetched
//b is the buffer of an alignment which is initialized outsize
//of the function to avoid mallocing multimple times
//outfp is a file pointer to the output (default stdout)
void get_raw_data(samfile_t *fp, long long offst, int ttl_reads, bam1_t *b, samfile_t *outfp){
	int read_len;
	int back_offst;
	int i=0;
	bam_seek(fp->x.bam, offst, SEEK_SET);
	while(samread(fp,b)>0){
		samwrite(outfp, b);
		i++;
		if(i==ttl_reads) break;
	}
	//samclose(outfp);
}

//It returns 1 if clone overlaps with region, 0 otherwise
inline int clone_overlaps_with_region(int region_st, int region_nd, int clone_st, int clone_nd){
	if(region_st<=clone_st && clone_st<=region_nd) return 1;
	else if(clone_st<=region_st && clone_nd >= region_st) return 1;
	else return 0;
}

inline int int_cmp(const void *a, const void *b){return (*(int*)a-*(int*)b);}

//It returns 1 if the pos'th bit of vec is one, 0 otherwise
inline int already_visited(long *vec, int vec_len, int pos){
	//fprintf(stderr, "vec_len: %d, pos: %d\n", vec_len, pos);
	if (pos>(vec_len*sizeof(long)*8)) ioerror("position greater than the length of bitvector");
	int x_pos=pos/(sizeof(long)*8); //??????????????????????
	int y_pos=pos%(sizeof(long)*8);
	unsigned long w=vec[x_pos];
	w=w>>y_pos;
	return w&1;
	
}

inline void currently_printing(long *vec, int vec_len, int pos){
	if ((pos>vec_len*sizeof(long)*8)) ioerror("position greater than the length of bitvector");
	int x_pos=pos/(sizeof(long)*8); //convert bytes to bits
	int y_pos=pos%(sizeof(long)*8);
	unsigned long w=1;
	w=w<<y_pos;
	vec[x_pos]=vec[x_pos]|w;
	return;
}

//It initializes a bitvector of "size" bits. Len is the length of the returned
//long array
long *initialize_bitvector(unsigned int size, int *len){
	if (size<0) ioerror("Negative size of bit vector");
	*len=size/(sizeof(long)*8);
	long *ret=(long*)malloc(sizeof(long)*(*len+1));
	int i=0;
	for (i=0;i<*len;i++) ret[i]=0;
	return ret;
}

///prints all clones whose index of indx is found between valid_lst[st] and
///valid_lst[nd]. fp is a file pointer to the raw data file. outfp is a file 
//pointer to the sam output file (stdout most of the time)
//The output includes reads that only cover the region between region_st and region_nd
void print_reads(print_reads_t args){//Mates *indx, int *valid_lst, int valid_st, int valid_nd, int region_st, int region_nd, samfile_t *fp, samfile_t *outfp){
	Mates *indx=args.indx;
	int *valid_lst=args.valid_lst;
	int valid_st=args.valid_st;
	int valid_nd=args.valid_nd;
	int region_st=args.region_st;
	int region_nd=args.region_nd;
	long *just_printed=args.just_printed;
	long len_just_printed=args.len_just_printed;
	samfile_t *fp=args.fp;
	samfile_t *outfp=args.outfp;
	int i=0;
	int j=0;
	int k=0;
	bam1_t *b=bam_init1();
	long long offst=0;
	int whence;
	int prev_i=-10;
	int ttl_reads=0;
	int *indx_lst=(int*)malloc(sizeof(int)*((valid_nd-valid_st)*2+4));
	Mates cur_end, mate_end;
	//populate the index of all reads and mates into one list in order to print
	//output sorted.
	//printf(">valid_range: %d\n",valid_nd-valid_st);
	for(j=valid_st;j<=valid_nd;j++){
		i=valid_lst[j];
		cur_end=indx[i];
		mate_end=indx[indx[i].mate_indx];
		if(i>cur_end.mate_indx)
			continue; //avoid entering mate pairs twice
		if (!clone_overlaps_with_region(region_st, region_nd, cur_end.loc, mate_end.loc))
			continue;
		if (already_visited(just_printed, len_just_printed, i)) 
			continue; //no need to print the same read again
		else currently_printing(just_printed, len_just_printed, i);
			
		indx_lst[k++]=i;
		indx_lst[k++]=cur_end.mate_indx;
		//printf("%d %d (%d)\n",cur_end.loc, mate_end.loc, -cur_end.loc+mate_end.loc);
	}
	//free(indx_lst);bam_destroy1(b);return;
	//qsort(indx_lst, k, sizeof(int), int_cmp);//print output sorted.
	for(j=0;j<k;j++){
		i=indx_lst[j];
		printf("indx: %d\n",i); continue;
		if (j==0) offst=indx[i].byte_offst; //initial offset
		//fprintf(stderr,"i: %d prev_i %d offst: %lld\n",i, prev_i, indx[i].byte_offst);
		if(prev_i<0 || i==prev_i+1)
			ttl_reads+=1;//(indx[i+1].byte_offst-indx[i].byte_offst);
		else{
			get_raw_data(fp, offst, ttl_reads, b, outfp); //print previous group of reads
			offst=indx[i].byte_offst; //start a new group of reads
			ttl_reads=1;
		}
		prev_i=i;

	}
	//get_raw_data(fp, offst, ttl_reads, b, outfp);
	bam_destroy1(b);
	free(indx_lst);
	return;
}


//valid_lst is the return array of find_valid_clone. ttl_valid is the length
//of that array. raw_file is the file that contains the raw data. Min_cov is
//the minimum coverage of clones that is desired for an area to be reported.
void clone_coverage(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, char *bam_file, int min_cov){
	int coverage=0;
	int valid_indx_start=-1;
	int valid_indx_end=0;
	int region_st=0, region_nd=0;
	int i=0, cur_i=0;
	print_reads_t prargs;
	prargs.just_printed=initialize_bitvector(len_indx, &prargs.len_just_printed);
	samfile_t *fp=samopen(bam_file, "rb",NULL);
	bam_header_t *bhr=fp->header;
	samfile_t *outfp=samopen("evidence.bam", "wb", bhr);
	////samfile_t *outfp=samopen("-","w", bhr);
	for(i=0;i<ttl_valid;i++){
		cur_i=valid_lst[i];
		if (indx[cur_i].mate_indx>cur_i){ //the first mate of a pair
			printf("loc: %d i: %d cur_i: %d +\n",indx[cur_i].loc, i, cur_i);
			valid_indx_end=i;
			if(coverage==min_cov-1){
				region_st=indx[cur_i].loc;//fprintf(stderr, "coverage: %d\n",coverage);ioerror("");
			}
			coverage+=1;
		}
		else if(indx[cur_i].mate_indx<cur_i){ //the closing mate of a pair
			printf("loc: %d i: %d cur_i: %d -\n",indx[cur_i].loc/*+indx[cur_i].read_len*/, i, cur_i);
			coverage-=1;
			//valid_indx_end=cur_i; ////////MAKE SURE THAT THIS ALWAYS WORKS
			if(coverage==min_cov-1){
				region_nd=indx[cur_i].loc/*+indx[cur_i].read_len*/;
				valid_indx_start=get_starting_point(indx, valid_lst, valid_indx_start, valid_indx_end, region_st, region_nd);
				//fprintf(outfp->x.tamw, ">Region %d %d %d\n",region_st, region_nd, region_nd-region_st);
				fprintf(stdout, ">Region %d %d %d\n",region_st, region_nd, region_nd-region_st);
				prargs.indx=indx;
				prargs.valid_lst=valid_lst;
				prargs.valid_st=valid_indx_start;
				prargs.valid_nd=valid_indx_end;
				prargs.region_st=region_st;
				prargs.region_nd=region_nd;
				prargs.fp=fp;
				prargs.outfp=outfp;
				print_reads(prargs);//indx, valid_lst, valid_indx_start, valid_indx_end, region_st, region_nd, fp, outfp);
			}
			if(coverage<0) ioerror("Negative coverage\n");
		}
	}
	if(coverage>=min_cov){
		region_nd=indx[cur_i].loc/*+indx[cur_i].read_len*/;
		valid_indx_start=get_starting_point(indx, valid_lst, valid_indx_start, valid_indx_end, region_st, region_nd);
		//fprintf(outfp->x.tamw,">Region %d %d %d\n",region_st, region_nd, region_nd-region_st);
		fprintf(stdout, ">Region %d %d %d\n",region_st, region_nd, region_nd-region_st);
		prargs.indx=indx;
		prargs.valid_lst=valid_lst;
		prargs.valid_st=valid_indx_start;
		prargs.valid_nd=valid_indx_end;
		prargs.region_st=region_st;
		prargs.region_nd=region_nd;
		prargs.fp=fp;
		prargs.outfp=outfp;
		print_reads(prargs);//indx, valid_lst, valid_indx_start, valid_indx_end, region_st, region_nd, fp, outfp);

	}
	free(prargs.just_printed);
	samclose(fp);
	samclose(outfp);
}

//it prints all reads whose locations on indx are stored in rd_lst. If mate_flag is 1, it prints reads in the same order
//as the rd_lst suggests. Otherwise it sorts rd_lst first and prints all reads sorted according to mapping location.
//BEWARE that if rd_lst gets sorted, its original order of reads gets lost.
//Finally histinfo contains the histogram information.

//if outfile==NULL it only updates histinfo without producing any output
void print_rd_list(Mates *indx, int len_indx, int *rd_lst, int len_rd_lst, char *inpt_bam_file, char *outfile, int mate_flag, Hist_info histinfo){
	inline int cmp(const void *a, const void *b){return *(int*)a-*(int*)b;}
	//if (mate_flag==0){
	qsort(rd_lst, len_rd_lst, sizeof(int), cmp); //it always gives the output sorted.
	//}

	samfile_t *fp=samopen(inpt_bam_file, "rb",NULL);
	bam_header_t *bhr=fp->header;
	samfile_t *outfp;
	if(outfp!=NULL)
		outfp=samopen(outfile, "wb", bhr);
	if (len_rd_lst<=0) {
		if(outfp!=NULL) samclose(outfp); 
		samclose(fp);
		return;
	}
	bam1_t *b=bam_init1();
	int i,j;
	long long offst;
	int prev_i=-1;
	int ttl_reads=0;
	for(j=0;j<len_rd_lst;j++){
		i=rd_lst[j];
		update_histogram_solo(&(histinfo.mhist), indx[i].loc, histinfo.chromo_length);
		if(outfp==NULL) continue;

		if (j==0) offst=indx[i].byte_offst; //initial offset
		//fprintf(stderr,"i: %d prev_i %d offst: %lld\n",i, prev_i, indx[i].byte_offst);
		if(prev_i<0 || i==prev_i+1)
			ttl_reads+=1;//(indx[i+1].byte_offst-indx[i].byte_offst);
		else{
			get_raw_data(fp, offst, ttl_reads, b, outfp); //print previous group of reads
			offst=indx[i].byte_offst; //start a new group of reads
			ttl_reads=1;
		}
		prev_i=i;

	}
	get_raw_data(fp, offst, ttl_reads, b, outfp);
	bam_destroy1(b);

	samclose(fp);
	if(outfp!=NULL)	samclose(outfp);
}

//it prints the meta data of the reads whose locations on indx are stored in rd_lst. 
//It sorts the rd_lst and prints the output sorted according to mapping location
//If mate_flag is 1, it prints tuples: loc, loc+len-1, mate_loc, mate_loc+mlen-1
//Otherwise it prints tuples: loc, loc+len-1
//BEWARE that if rd_lst gets sorted, its original order of reads gets lost.

//if outfile==NULL it returns without doing anything.
void print_reduced_rd_list(Mates *indx, int len_indx, long *strand_indx, int strand_len, int *rd_lst, int len_rd_lst, char *outfile, int mate_flag){
	inline int cmp(const void *a, const void *b){return *(int*)a-*(int*)b;}
	if(outfile==NULL)
		return;
	qsort(rd_lst, len_rd_lst, sizeof(int), cmp);
	FILE *fp=fopen(outfile, "w");
	int i=0, j=0;
	int mate_indx=0;
	int loc, mloc, len, mlen=0;
	char strand, mstrand;
	for(j=0;j<len_rd_lst;j++){
		i=rd_lst[j];
		loc=indx[i].loc;
		len=indx[i].read_len;
		strand=get_strand(strand_indx, strand_len, i);
		mate_indx=indx[i].mate_indx;
		if(mate_indx<i && mate_flag) continue; //don't print pairs twice.
		fprintf(fp, "%d\t%d\t%c\t",loc, loc+len-1, strand);
		if(mate_flag){
			if(mate_indx>0){
				mloc=indx[mate_indx].loc;
				mlen=indx[mate_indx].read_len;
				mstrand=get_strand(strand_indx, strand_len, mate_indx);
				fprintf(fp, "%d\t%d\t%c\t",mloc, mloc+mlen-1, mstrand);
			}
			else ioerror("An unpaired read found while printing mates\n");
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

	

/*int main(int argc, char **argv){
	if(argc!=7) ioerror("usage: ./discordant_coverage <indexed_mate_file> <bam_file> <min_clone_coverage> distance|orientation|dangling <min_dist|left_strand|region_start> <max_dist|right_strand|region_end>\n");
	char *indx_file=argv[1];
	char *bam_file=argv[2];
	int min_clones=atoi(argv[3]);
	char *mode=argv[4];
	int min_dist, max_dist;
	char lstrand, rstrand;

	//MatesGroups *mg;
	Mates *mate_indx;
	long *strand_indx;
	int len_strand;
	//Ins_len_groups *groups;
	int ttl_reads; //, ttl_groups;
	int *valid_lst, ttl_valid;
	int reg_st, reg_nd;
	//mg=get_index(indx_file, &ttl_reads, &ttl_groups);
	mate_indx=get_mate_index(indx_file, &ttl_reads);
	strand_indx=get_strand_index(indx_file, &len_strand);
	
	//mate_indx=mg->mates;
	//groups=mg->groups;
	printf(">chrom chr1\n");
	if (strcmp(mode, "distance")==0){
		min_dist=atoi(argv[5]);
		max_dist=atoi(argv[6]);
		valid_lst=find_dist_valid_clones(mate_indx, ttl_reads, min_dist, max_dist, &ttl_valid);
	}
	else if(strcmp(mode, "orientation")==0){
		lstrand=argv[5][0];
		rstrand=argv[6][0];
		if (lstrand=='+' || lstrand=='f') lstrand='F';
		if (lstrand=='-' || lstrand=='r') lstrand='R';
		if (rstrand=='+' || rstrand=='f') rstrand='F';
		if (rstrand=='-' || rstrand=='r') rstrand='R';
		valid_lst=find_ornt_invalid_clones(mate_indx, ttl_reads, strand_indx, len_strand, lstrand, rstrand, &ttl_valid);
	}
	else if (strcmp(mode, "dangling")==0){
		reg_st=atoi(argv[5]);
		reg_nd=atoi(argv[6]);
		valid_lst=find_dangling_clones(mate_indx, ttl_reads, reg_st, reg_nd, &ttl_valid);
		print_entire_vld_list(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, "dangling_reads.bam");
		return 0;
	}
	else ioerror("Mode should be either \"distance\" or \"orientation\"");

	clone_coverage(mate_indx, ttl_reads, valid_lst, ttl_valid, bam_file, min_clones);
	free(mate_indx);
	//free(mg->groups);
	//free(mg);
	return 0;
}*/


int *find_valid_clones(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n){
	int i=0;
	int *ret;
	///stupid method: First iterate indx to find n and then iterate indx again to populate ret
	int cnt=0;
	time_t t1=time(NULL);
	for (i=1;i<ttl_reads;i++){ //indx[0] is reserved
		if(is_valid(indx, i, strand_indx, strand_len)>0) cnt+=1;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	for (i=1;i<ttl_reads;i++){ //indx[0] is reserved
		if(is_valid(indx, i, strand_indx, strand_len)>0){
			ret[cnt]=i;
			cnt+=1;
		}
	}
	(*n)=cnt;
	time_t t2=time(NULL);
	return ret;
}
