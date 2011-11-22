#include<stdio.h>
#include "all_tools.h"

inline Intervals get_imported_interval(Imported_info *tbl, int i){
	Intervals ret;
	ret.st=tbl[i].start;
	ret.nd=tbl[i].end;
	ret.rd1=i;
	ret.rd2=-1;
	return ret;
}
