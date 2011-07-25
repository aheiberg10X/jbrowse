#include<stdio.h>
#include"all_tools.h"
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	int mate_indx=indx[i].mate_indx;
	int t0;
	t0 = indx[mate_indx].loc-indx[i].loc ;
	int t1;
	t1 = t0>1000 ;
	int t2;
	t2 = indx[i].loc-indx[mate_indx].loc ;
	int t3;
	t3 = t2>1000 ;
	int t4;
	t4 = t1||t3 ;
	return t4;
}
