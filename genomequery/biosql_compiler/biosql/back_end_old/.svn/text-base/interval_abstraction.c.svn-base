#include <stdio.h>
#include <stdlib.h>
#include "all_tools.h"


typedef struct{
	int loc;
	int intrvl_indx;//Interval location at the intrvl_list
	char direction; //0 for opening, 1 for closing.
} Unrolled_intervals;


//For every mate pair of indx that is in valid_lst it creates an interval
//with st=left.loc, nd=right.loc+len. The returned array has a length of 
//ttl_intrvl.
Intervals *mates2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl){
	int i=0;
	int cur_i=0;
	if(ttl_valid<=0){
		*ttl_intrvl=0;
		return NULL;
	}
	Intervals *ret=(Intervals*)malloc(sizeof(Intervals)*ttl_valid);
	int cnt=0;
	for (i=0;i<ttl_valid;i++){
		cur_i=valid_lst[i];
		if (cur_i<0) continue;
		if (cur_i > indx[cur_i].mate_indx) continue; //consider mates only once
		if (indx[cur_i].mate_indx<0) ioerror("Interval with dangling end found");
		ret[cnt].st=indx[cur_i].loc;
		ret[cnt].nd=indx[indx[cur_i].mate_indx].loc+indx[cur_i].read_len;
		ret[cnt].rd1=cur_i;
		ret[cnt].rd2=indx[cur_i].mate_indx;
		cnt++;
	}
	*ttl_intrvl=cnt;
	return ret;
}

//For every read of indx that is in valid_lst it creates an interval
//with st=loc, nd=loc+len-1. The returned array has a length of 
//ttl_intrvl.
Intervals *singles2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl){
	int i=0;
	int cur_i=0;
	Intervals *ret=(Intervals*)malloc(sizeof(Intervals)*ttl_valid);
	int cnt=0;
	for (i=0;i<ttl_valid;i++){
		cur_i=valid_lst[i];
		if (cur_i<0) continue;
		if(indx[cur_i].loc<0) ioerror("Unmapped read found. Please revise your filtering");
		ret[cnt].st=indx[cur_i].loc;
		ret[cnt].nd=indx[cur_i].loc+indx[cur_i].read_len;
		ret[cnt].rd1=cur_i;
		ret[cnt].rd2=-1;
		cnt++;
	}
	*ttl_intrvl=cnt;
	return ret;
}


/*lst is a pointer to an array of pointers of Intervals of length n.
The function appends val to lst while it updates n*/
void append_interval(Intervals **lst, Intervals val, int *n){
	Intervals *tmp;
	int cnt=(*n)+1;
	tmp=(Intervals*)realloc(*lst, cnt*sizeof(Intervals));
	if (tmp==NULL) ioerror ("Cannot find memory space to store an interval");
	tmp[cnt-1]=val;
	*lst=tmp;
	*n=cnt;
}

/*
Find the first interval between prev_intrvl_start and intrvl_end that overlaps with [region_st, region_nd]. The function returns the index of that interval in the
interval_lst.
*/
int get_starting_interval(Intervals *intrvl_lst, int prev_intrvl_start, int intrvl_end, int region_st, int region_nd){
	if(prev_intrvl_start<0) prev_intrvl_start=-1;
	int i=0;
	int st=0,nd=0;
	for(i=prev_intrvl_start;i<=intrvl_end;i++){ //verify whether i=prev or i=prev+1
		if(i<0) continue;
		st=intrvl_lst[i].st;
		nd=intrvl_lst[i].nd;
		if( (region_st<=st && st<=region_nd) || (st<region_st && nd > region_st))
			return i;
	}
}


/*
Find the last interval before intrvl_end that overlaps with [region_st, region_nd]. The function returns the index of that interval in the
interval_lst.
*/
int get_ending_interval(Intervals *intrvl_lst, int intrvl_end, int region_st, int region_nd){
	int i=0;
	int st=0,nd=0;
	for(i=intrvl_end-1;i>=0;i--){ //verify whether i=prev or i=prev+1
		st=intrvl_lst[i].st;
		nd=intrvl_lst[i].nd;
		if( (region_st<=st && st<=region_nd) || (st<region_st && nd > region_st))
			return i;
	}
	ioerror("Cannot find an ending interval that overlaps with region");
}

//It takes intrvl_lst of ttl_intervl elements and returns the corresponding array of unrolled_intervals of size ret_size. The returned array
//is sorted by loc
typedef Unrolled_intervals Ui;
Unrolled_intervals *unroll_intervals(Intervals *intrvl_lst, int ttl_intervl, int *ret_size){
	inline int intrvl_cmp(const void *a, const void *b){
		Ui ua=*(Ui*)a;
		Ui ub=*(Ui*)b;
		if (ua.loc!=ub.loc)
			return (*(Ui*)a).loc-(*(Ui*)b).loc;
		else
			return ua.intrvl_indx-ub.intrvl_indx;
	}

	int n=ttl_intervl*2;
	*ret_size=n;
	Unrolled_intervals *ret=(Unrolled_intervals*)malloc((n+2)*sizeof(Unrolled_intervals));
	int i=0;
	int j=0;
	for(i=0;i<ttl_intervl;i++){
		ret[j].loc=intrvl_lst[i].st;
		ret[j].intrvl_indx=i;
		ret[j].direction=0;
		
		ret[j+1].loc=intrvl_lst[i].nd;
		ret[j+1].intrvl_indx=i;
		ret[j+1].direction=1;
		j+=2;
	}
	qsort(ret, n, sizeof(Unrolled_intervals),intrvl_cmp);
	return ret;
}

//It appends to ttl_evidence all rds found in intervals between first and last that overlap with region_st, region_nd. 
//The last call to this function should occur with intrvl_lst=NULL so that it releases the memory of the static bitvector
void collect_evidence(int *evidence, int *ttl_evidence, Intervals *intrvl_lst, int ttl_intrvls, int first_intrvl, int last_intrvl, int region_st, int region_nd){
	static long *log=NULL; //A bit vector that prevents collecting evidence from an interval more than once
	if (log==NULL) log=initialize_bit_vector(ttl_intrvls+2);
	if(intrvl_lst==NULL){
		if(log!=NULL) free(log);
		log=NULL;
		return;
	}
	int k=*ttl_evidence;
	Intervals cur;
	int i;
	for(i=first_intrvl;i<=last_intrvl;i++){
		if (get_bit(log, ttl_intrvls, i))//skip this interval if already visited
			continue;
		cur=intrvl_lst[i];
		if (((region_st<=cur.st) && (cur.st<=region_nd))  ||  ((cur.st<=region_st) && cur.nd>=region_st)){
			set_bit(log, ttl_intrvls, i); //mark this interval as visited.
			if(cur.rd1>=0){
				evidence[k++]=cur.rd1;
				//printf("Indx: %d\n",cur.rd1);
			}
			if(cur.rd2>=0){
				evidence[k++]=cur.rd2;
				//printf("Indx: %d\n", cur.rd2);
			}
		}
	}
	*ttl_evidence=k;

}


//The function returns the areas that are covered by at least a number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence min_interval_coverage(Mates *indx, int len_indx, Intervals *intrvl_lst, int ttl_intrvl, int min_cov){
	int coverage=0;
	int first_intrvl=-1; //the first interval of intrvl_lst that covers a region
	int last_intrvl=0; //the last intervlal of intrvl_lst that covers a region.
	int region_st=0, region_nd=0;
	int i=0;
	Intrevidence ret;
	ret.ttl_regions=0;
	ret.regions=NULL;
	ret.evidence=NULL;
	Intervals stored_intrvl;
	Unrolled_intervals cur;

	int *evidence=NULL;
	int ttl_evidence=0;

	int ttl_unr;
	Unrolled_intervals *unr_intr=unroll_intervals(intrvl_lst, ttl_intrvl, &ttl_unr);
	evidence=(int*)malloc(sizeof(int)*(ttl_unr+4));
	//fprintf(stderr, "ttl_unr %d\n",ttl_unr);
	for(i=0;i<ttl_unr;i++){
		cur=unr_intr[i];
		//printf("loc: %d indx: %d ",cur.loc, cur.intrvl_indx);
		if(cur.direction==0){ //the opening of an interval
			if (intrvl_lst[cur.intrvl_indx].st != cur.loc) ioerror("Discrepancy on an opening unrolled"); //The opening of an unrolled interval should match with the st of the corresponding interval.
			//printf("+\n");
			if(last_intrvl>cur.intrvl_indx){
				fprintf(stderr, "last: %d cur: %d\n",last_intrvl, cur.intrvl_indx);
			}

			//last_intrvl=max(last_intrvl,cur.intrvl_indx); //????????//
			last_intrvl=cur.intrvl_indx;
			//last_intrvl=i;
			if(coverage==min_cov-1){
				region_st=cur.loc;
				//fprintf(stderr, "region_st: %d coverage: %d\n",region_st, coverage); ioerror("");
			}
			coverage+=1;
		}
		else if(cur.direction==1){ //the closing end of an interval
			if(intrvl_lst[cur.intrvl_indx].nd!=cur.loc) ioerror("Discrepancy on a closing unrolled"); //the closing of an unrolled interval should match with the nd of the corresponding interval.
			//printf("-\n");
			coverage-=1;
			if(coverage==min_cov-1){
				region_nd=cur.loc;
				first_intrvl=get_starting_interval(intrvl_lst, first_intrvl, last_intrvl, region_st, region_nd);
				//printf(">region: %d %d %d\n",region_st, region_nd, region_nd-region_st);
				stored_intrvl.st=region_st; 
				stored_intrvl.nd=region_nd; 
				stored_intrvl.rd1=-1;
				stored_intrvl.rd2=-1;
				append_interval(&(ret.regions), stored_intrvl, &(ret.ttl_regions));
				collect_evidence(evidence, &ttl_evidence, intrvl_lst, ttl_intrvl, first_intrvl, last_intrvl, region_st, region_nd);
				
			}
			if(coverage<0) ioerror("Negative coverage\n");
		}
		else ioerror("Unknown direction number");
	}
	if(coverage>=min_cov){
		region_nd=cur.loc;
		first_intrvl=get_starting_interval(intrvl_lst, first_intrvl, last_intrvl, region_st, region_nd);
		//printf(">region: %d %d %d\n",region_st, region_nd, region_nd-region_st);

		stored_intrvl.st=region_st; 
		stored_intrvl.nd=region_nd; 
		stored_intrvl.rd1=-1;
		stored_intrvl.rd2=-1;
		append_interval(&(ret.regions), stored_intrvl, &(ret.ttl_regions));
		collect_evidence(evidence, &ttl_evidence, intrvl_lst, ttl_intrvl, first_intrvl, last_intrvl, region_st, region_nd);

	}

	//fprintf(stderr, "ttl_intrvl: %d ttl_unr: %d ttl_evidence: %d\n",ttl_intrvl, ttl_unr, ttl_evidence);
	if (ttl_evidence>0) ret.evidence=(int*)realloc(evidence, (ttl_evidence+2)*sizeof(int));
	else ret.evidence=NULL;
	if(ret.evidence!=NULL)	ret.ttl_evidence=ttl_evidence;
	else ret.ttl_evidence=0;
	free(unr_intr);
	collect_evidence(NULL, NULL, NULL, -1, -1, -1, -1, -1);
	return ret;
}

//The function returns the areas that are covered by at most a positive 
//number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence max_interval_coverage(Mates *indx, int len_indx, Intervals *intrvl_lst, int ttl_intrvl, int max_cov){
	int coverage=0;
	int first_intrvl=-1; //the first interval of intrvl_lst that covers a region
	int last_intrvl=0; //the last intervlal of intrvl_lst that covers a region.
	int region_st=0, region_nd=0;
	int i=0;
	Intrevidence ret;
	ret.ttl_regions=0;
	ret.regions=NULL;
	ret.evidence=NULL;
	Intervals stored_intrvl;
	Unrolled_intervals cur;

	int *evidence=NULL;
	int ttl_evidence=0;

	int ttl_unr;
	Unrolled_intervals *unr_intr=unroll_intervals(intrvl_lst, ttl_intrvl, &ttl_unr);
	evidence=(int*)malloc(sizeof(int)*(ttl_unr+4));
	//fprintf(stderr, "ttl_unr %d\n",ttl_unr);
	for(i=0;i<ttl_unr;i++){
		cur=unr_intr[i];
		//printf("loc: %d indx: %d ",cur.loc, cur.intrvl_indx);
		if(cur.direction==0){ //the opening of an interval
			if (intrvl_lst[cur.intrvl_indx].st != cur.loc) ioerror("Discrepancy on an opening unrolled"); //The opening of an unrolled interval should match with the st of the corresponding interval.
			//printf("+\n");
			if(last_intrvl>cur.intrvl_indx){
				fprintf(stderr, "last: %d cur: %d\n",last_intrvl, cur.intrvl_indx);
			}

			//last_intrvl=max(last_intrvl,cur.intrvl_indx); //????????//
			last_intrvl=cur.intrvl_indx;
			//last_intrvl=i;
			if(coverage==max_cov){
				region_nd=cur.loc;
				first_intrvl=get_starting_interval(intrvl_lst, first_intrvl, last_intrvl, region_st, region_nd);
				last_intrvl=get_ending_interval(intrvl_lst, cur.intrvl_indx, region_st, region_nd);
				//fprintf(stderr, "region_st: %d coverage: %d\n",region_st, coverage); ioerror("");
				stored_intrvl.st=region_st; 
				stored_intrvl.nd=region_nd; 
				stored_intrvl.rd1=-1;
				stored_intrvl.rd2=-1;
				append_interval(&(ret.regions), stored_intrvl, &(ret.ttl_regions));
				collect_evidence(evidence, &ttl_evidence, intrvl_lst, ttl_intrvl, first_intrvl, last_intrvl, region_st, region_nd);

			}
			coverage+=1;
		}
		else if(cur.direction==1){ //the closing end of an interval
			if(intrvl_lst[cur.intrvl_indx].nd!=cur.loc) ioerror("Discrepancy on a closing unrolled"); //the closing of an unrolled interval should match with the nd of the corresponding interval.
			//printf("-\n");
			coverage-=1;
			if(coverage==max_cov){
				region_st=cur.loc;
				
			}
			if(coverage<0) ioerror("Negative coverage\n");
		}
		else ioerror("Unknown direction number");
	}
	if(coverage<=max_cov){
		region_nd=cur.loc;
		first_intrvl=get_starting_interval(intrvl_lst, first_intrvl, last_intrvl, region_st, region_nd);
		last_intrvl=get_ending_interval(intrvl_lst, cur.intrvl_indx, region_st, region_nd); //here cur=unr_intr[ttl_unr-1];
		//printf(">region: %d %d %d\n",region_st, region_nd, region_nd-region_st);

		stored_intrvl.st=region_st; 
		stored_intrvl.nd=region_nd; 
		stored_intrvl.rd1=-1;
		stored_intrvl.rd2=-1;
		append_interval(&(ret.regions), stored_intrvl, &(ret.ttl_regions));
		collect_evidence(evidence, &ttl_evidence, intrvl_lst, ttl_intrvl, first_intrvl, last_intrvl, region_st, region_nd);

	}

	//fprintf(stderr, "ttl_intrvl: %d ttl_unr: %d ttl_evidence: %d\n",ttl_intrvl, ttl_unr, ttl_evidence);
	if (ttl_evidence>0) ret.evidence=(int*)realloc(evidence, (ttl_evidence+2)*sizeof(int));
	else ret.evidence=NULL;
	if(ret.evidence!=NULL)	ret.ttl_evidence=ttl_evidence;
	else ret.ttl_evidence=0;
	free(unr_intr);
	collect_evidence(NULL, NULL, NULL, -1, -1, -1, -1, -1);
	return ret;
}



