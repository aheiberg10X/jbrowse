#include<stdio.h>
#include<stdlib.h>
#include "all_tools.h"

//it returns: 
//	either the largest j s.t 0<=j<=i<ttl_reads for which indx[j]>=0 when mode=='-'
//  or the min j s.t. i<=j<ttl_reads for which indx[j]>=0 when mode=='+'
inline long get_closest_positive(Mates *indx, int i, int ttl_reads, char mode){
	if(i<0||i>=ttl_reads) ioerror("Index out of boundaries in the closest positive");
	long j;
	if(mode=='-'){
		for(j=i;j>=1;j--) //it is j>=1 instead of j>=0 because indx[0] is reserved for the unmapped
			if (indx[j].loc>=0) return j;
		ioerror("Cannot find a lower indexed mapped read");
	}
	else if(mode=='+'){
		for(j=i;j<ttl_reads;j++){
			if (indx[j].loc>=0) return j;
		}
		ioerror("Cannot find a higher indexed mapped read");
	}
	else ioerror("closest_positive mode is always + or -");
}


//It returns the minimum index of a read for which loc<=key
long get_closest_min_loc(Mates *indx, int ttl_reads, unsigned long key){
	int BIG_STEP=10000;//make a coarse grain search first
	unsigned long i=0;
	for(i=1;i<ttl_reads;i+=BIG_STEP){ //indx[0] is reserved for the unmapped
		if(indx[i].loc>=key){
			i=max(0, i-2*BIG_STEP); //no need for i to go negative
			break;
		}
	}
	for(i;i<ttl_reads;i++){
		if(indx[i].loc<0) continue;
		if(indx[i].loc>=key) return max(0, i-1);
		//if(indx[i].loc==key || indx[i].loc<key && indx[i].loc+indx[i].read_len>key)
		//	return i;
	}
	return -1; //failure.
}

//it returns an array of indexes to indx where the reads are mapped between 
//mloc and Mloc and satisfy the is_valid_single function. n is the length
//of the returned array.
//if mloc==-1 and Mloc==-1, the entire chromosome is considered.
int *find_valid_reads(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n, long mloc, long Mloc){
	int *ret;
	int cnt=0;
	long arg_min;
	long arg_max;
	long min_loc, max_loc;
	if(mloc>=0){
		min_loc=mloc;
		arg_min=get_closest_min_loc(indx, ttl_reads, min_loc); //get max i s.t indx[i].loc <= min_loc
	}
	else{
		arg_min=get_closest_positive(indx, 0, ttl_reads, '+'); //get the first mapped read
		min_loc=indx[arg_min].loc;
	}

	if(Mloc>=0){
		max_loc=Mloc;
	}
	else{
		arg_max=get_closest_positive(indx, ttl_reads-1, ttl_reads, '-');
		max_loc=indx[arg_max].loc;
	}


	int i=arg_min;
	if (arg_min==-1){
		*n=0;
		return NULL;
	}
	while (i<ttl_reads){
		if(max_loc<0) break; //it implies the end of the chromosome
		if(indx[i].loc > max_loc) break;
		if( (indx[i].loc+indx[i].read_len > min_loc) && (indx[i].loc<=max_loc))
			if (is_valid_single(indx, i, strand_indx, strand_len)>0) cnt+=1;
		i++;
	}
	if(Mloc>=0){
		arg_max=i;
	}
	ret=(int*)malloc(sizeof(int)*(cnt+1));
	cnt=0;
	for(i=arg_min;i<=arg_max;i++){
		if(indx[i].loc > max_loc) break;
		if( (indx[i].loc+indx[i].read_len > min_loc) && (indx[i].loc<=max_loc))
			if (is_valid_single(indx, i, strand_indx, strand_len)>0) ret[cnt++]=i;
	}

	*n=cnt;
	return ret;
}
