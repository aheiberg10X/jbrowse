#include "Reads.hpp"

void Reads::eval_expressions(int i, int& expr1, int& expr2){
	int mate=rindx[i].mate_indx;
	expr1= rindx[i].loc + 1000 ;
	expr2= rindx[mate].loc + 1000 ;
}
