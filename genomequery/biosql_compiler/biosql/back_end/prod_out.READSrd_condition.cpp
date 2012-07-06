#include "Reads.hpp"

int Reads::eval_rd_condition(int i){
	int mate=rindx[i].mate_indx;
	int ev = ( ( ( ( ( ( ( ( rindx[i].loc >= 72537704 ) && ( rindx[i].loc <= 72585028 ) ) || ( ( rindx[mate].loc >= 72537704 ) && ( rindx[mate].loc <= 72585028 ) ) ) ) || ( ( ( ( rindx[i].loc >= 102599611 ) && ( rindx[i].loc <= 102603213 ) ) || ( ( rindx[mate].loc >= 102599611 ) && ( rindx[mate].loc <= 102603213 ) ) ) ) ) || ( ( ( ( rindx[i].loc >= 147303994 ) && ( rindx[i].loc <= 147313602 ) ) || ( ( rindx[mate].loc >= 147303994 ) && ( rindx[mate].loc <= 147313602 ) ) ) ) ) || ( ( ( ( rindx[i].loc >= 147373047 ) && ( rindx[i].loc <= 147395259 ) ) || ( ( rindx[mate].loc >= 147373047 ) && ( rindx[mate].loc <= 147395259 ) ) ) ) ) || ( ( ( ( rindx[i].loc >= 161154612 ) && ( rindx[i].loc <= 161166987 ) ) || ( ( rindx[mate].loc >= 161154612 ) && ( rindx[mate].loc <= 161166987 ) ) ) ) ) ;
	return ev;
}
