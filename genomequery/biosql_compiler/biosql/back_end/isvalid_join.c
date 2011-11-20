#include<stdio.h>
#include"all_tools.h"
inline int isvalid_join(Join *join_lst, int i, Mates *indx1, long *strand_indx1, Mates *indx2, long *strand_indx2, Imported_info *tbl1, Imported_info *tbl2){
 	Join jn=join_lst[i];
	int i1=jn.indx1;
	int i2=jn.indx2;

	int t2=indx2[i2].loc>0;
	return t2;
}
