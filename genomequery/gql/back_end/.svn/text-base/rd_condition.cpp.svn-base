#include "Reads.hpp"

int Reads::eval_rd_condition(int i){
	int mate=rindx[i].mate_indx;
	int ev = ( ( rindx[i].loc >= 0 ) && ( rindx[mate].loc >= 0 ) ) && ( ( ( ( ( rindx[mate].loc - rindx[i].loc > 1000 ) && ( rindx[mate].loc - rindx[i].loc < 100000 ) ) ) || ( ( ( rindx[i].loc - rindx[mate].loc > 1000 ) && ( rindx[i].loc - rindx[mate].loc < 100000 ) ) ) ) ) ;
	return ev;
}
