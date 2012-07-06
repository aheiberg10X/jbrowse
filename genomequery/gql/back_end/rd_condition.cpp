#include "Reads.hpp"

int Reads::eval_rd_condition(int i){
	int mate=rindx[i].mate_indx;
	int ev = ( ( rindx[i].loc >= 0 ) && ( rindx[mate].loc >= 0 ) ) && ( ( ( ( ( ( ( rindx[mate].loc - rindx[i].loc > 1000 ) && ( rindx[mate].loc - rindx[i].loc < 2000000 ) ) && ( get_strand(i) == 'R' ) ) && ( get_strand(mate) == 'F' ) ) ) || ( ( ( ( ( rindx[i].loc - rindx[mate].loc > 1000 ) && ( rindx[i].loc - rindx[mate].loc < 2000000 ) ) && ( get_strand(i) == 'F' ) ) && ( get_strand(mate) == 'R' ) ) ) ) ) ;
	return ev;
}
