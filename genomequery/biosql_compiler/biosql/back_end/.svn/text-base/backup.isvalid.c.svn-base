#include<stdio.h>
#include "all_tools.h"

/*inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	int min_dist=500;
	int max_dist=10000;
	int dist;
	int mate_indx=indx[i].mate_indx;
	if(mate_indx<0 || indx[i].loc<0) return -1;
	if(indx[mate_indx].loc<0) return -1;
	if (indx[i].loc<indx[mate_indx].loc)
		dist=indx[mate_indx].loc + indx[mate_indx].read_len-indx[i].loc;
	else
		dist=indx[i].loc + indx[i].read_len-indx[mate_indx].loc;
	if(dist<0) ioerror("Negative Distance");///////////dist=-dist;
	if (min_dist<=dist && dist<=max_dist) return 1;
	else return 0;
}
*/
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len){
	char lstrand='F';
	char rstrand='R';
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
}

