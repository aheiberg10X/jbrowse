#include<stdio.h>
#include"all_tools.h"
inline Intervals get_rd_interval(Mates *indx, int i){
	int mate_indx=indx[i].mate_indx;
	Intervals ret;
	
	ret.st=indx[i].loc;
	ret.nd=indx[mate_indx].loc;;
	ret.rd1=i;
	ret.rd2=mate_indx;
	return ret;
}
