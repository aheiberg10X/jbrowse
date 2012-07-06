#include "Reads.hpp"

int Reads::eval_rd_condition(int i){
	int mate=rindx[i].mate_indx;
	int ev = ( ( rindx[i].loc >= 0 ) && ( rindx[mate].loc >= 0 ) ) && ( ( ( ( ( rindx[mate].loc + 100 - rindx[i].loc > 250 ) && ( rindx[mate].loc + 100 - rindx[i].loc < 400 ) ) ) || ( ( ( rindx[i].loc + 100 - rindx[mate].loc > 250 ) && ( rindx[i].loc + 100 - rindx[mate].loc < 400 ) ) ) ) ) ;
	return ev;
}
