#include<stdio.h>
#include"all_tools.h"
inline int isvalid_join(Join *join_lst, int i, Imported_info *tbl1, Imported_info *tbl2, Mates *mindx, long *strand_indx, int strand_len){
	union IU {
		Imported_info *tbl;
		Mates *mindx;
	};
	union IU indx1, indx2;
	int i1=join_lst[i].indx1;
	int i2=join_lst[i].indx2;
	int mate_indx;
	if(tbl1!=NULL)
		indx1.tbl=tbl1;
	else{
		indx1.mindx=mindx;
		mate_indx=indx1.mindx[i1].mate_indx;
	}
	if(tbl2!=NULL)
		indx2.tbl=tbl2;
	else{
		indx2.mindx=mindx;
		mate_indx=indx2.mindx[i2].mate_indx;
	}
	int t3;
	t3 = (strcmp(indx1.tbl[i1].chr,"chr1")==0) ;
	int t4;
	t4 = get_strand(strand_indx, strand_len, i2)=='F' ;
	int t5;
	t5 = t3&&t4 ;
	int t6;
	t6 = indx2.mindx[i2].loc>0 ;
	int t7;
	t7 = t5&&t6 ;
	int t8;
	t8 = indx2.mindx[mate_indx].loc<1000 ;
	int t9;
	t9 = t7&&t8 ;
	int t10;
	t10 = indx2.mindx[mate_indx].loc>0 ;
	int t11;
	t11 = t9&&t10 ;
	int t12;
	t12 = (strcmp(indx1.tbl[i1].id,"gene1")==0) ;
	int t13;
	t13 = t11&&t12 ;
	return t13;
}