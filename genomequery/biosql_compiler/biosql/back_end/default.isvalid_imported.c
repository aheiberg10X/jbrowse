#include<stdio.h>
#include"all_tools.h"
inline int isvalid_imported(Imported_info *indx, int i, int ttl_table){
if (i<0 || i>=ttl_table) ioerror("Isvalid call is out of bounds");
	return 1;
}
