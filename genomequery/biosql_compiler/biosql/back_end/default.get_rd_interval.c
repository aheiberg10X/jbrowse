#include<stdio.h>
#include "all_tools.h"

inline Intervals get_rd_interval(Mates *read_indx, int i){
	Intervals ret;
	ret.st=read_indx[i].loc;
	ret.nd=read_indx[i].loc+read_indx[i].read_len;
	ret.rd1=i;
	ret.rd2=-1;
	return ret;
}
