#include<stdio.h>
#include"all_tools.h"
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	int mate_indx=indx[i].mate_indx;
	int t0=indx[i].loc>150000000;
	return t0;
}
