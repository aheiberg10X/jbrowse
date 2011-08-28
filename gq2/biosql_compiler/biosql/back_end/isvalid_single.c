#include<stdio.h>
#include"all_tools.h"
inline int is_valid_single(Mates *indx, int i, long *strand_indx, int strand_len){
	int t0;
	t0 = indx[i].loc>0 ;
	int t1;
	t1 = indx[i].loc<1000 ;
	int t2;
	t2 = t0&&t1 ;
	return t2;
}
