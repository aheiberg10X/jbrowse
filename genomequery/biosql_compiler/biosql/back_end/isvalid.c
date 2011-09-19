#include<stdio.h>
#include"all_tools.h"
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	int mate_indx=indx[i].mate_indx;
	int t0;
	t0 = indx[i].loc>=0 ;
	int t1;
	t1 = indx[mate_indx].loc>=0 ;
	int t2;
	t2 = t0&&t1 ;
	int t3;
	t3 = indx[mate_indx].loc-indx[i].loc ;
	int t4;
	t4 = t3>1000 ;
	int t5;
	t5 = indx[mate_indx].loc-indx[i].loc ;
	int t6;
	t6 = t5<2000000 ;
	int t7;
	t7 = t4&&t6 ;
	int t8;
	t8 = indx[i].loc-indx[mate_indx].loc ;
	int t9;
	t9 = t8>1000 ;
	int t10;
	t10 = indx[i].loc-indx[mate_indx].loc ;
	int t11;
	t11 = t10<2000000 ;
	int t12;
	t12 = t9&&t11 ;
	int t13;
	t13 = t7||t12 ;
	int t14;
	t14 = t2&&t13 ;
	return t14;
}
