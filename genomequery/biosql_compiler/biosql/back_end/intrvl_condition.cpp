#include "Intervals.hpp"

int Intervals::eval_condition(int i){
	return (vec[i].nd-vec[i].st)<1000;
}
