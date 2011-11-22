#include<stdio.h>
#include"all_tools.h"
inline Intervals get_imported_interval(Imported_info *indx, int i){
	Intervals ret;
	ret.st=indx[i].start;
	ret.nd=indx[i].end;;
	ret.rd1=i;
	ret.rd2=-1;
	return ret;
}
