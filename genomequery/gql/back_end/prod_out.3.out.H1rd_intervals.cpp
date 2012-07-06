#include "Reads.hpp"

void Reads::eval_expressions(int i, int& expr1, int& expr2){
	int mate=rindx[i].mate_indx;
	expr1= rindx[i].loc ;
	expr2= rindx[i].loc + rindx[i].read_len ;
}
