#include "Reads.hpp"

int Reads::eval_rd_condition(int i){
	int mate=rindx[i].mate_indx;
	int ev = ( ( ( ( ( rindx[i].loc >= 102599611 ) && ( rindx[i].loc <= 102603213 ) ) ) || ( ( ( rindx[mate].loc >= 102599611 ) && ( rindx[mate].loc <= 102603213 ) ) ) ) ) ;
	return ev;
}
