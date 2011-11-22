#include<stdio.h>
#include"all_tools.h"
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	int mate_indx=indx[i].mate_indx;
	if(i<=0 || mate_indx <= 0) return 0;
	int loc=indx[i].loc;
	int mate_loc=indx[mate_indx].loc;
	if(loc<0 || mate_loc<0) return 0;
	int dst=loc-mate_loc;
	if(dst<0)
		dst=-dst;
	if (dst>1000 && dst<1000000)
		return 1;
	else
		return 0;
}
