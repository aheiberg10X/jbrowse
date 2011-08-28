#include<stdio.h>
#include "all_tools.h"

//The function returns 1 if join_lst[i] satisfies the customizable condition.
//If any of tbl1, tbl2, mate_indx are not necessary, they should be NULL
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
	return 1;
}

