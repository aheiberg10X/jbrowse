#ifndef __INTERVAL_TREE__HPP
#define __INTERVAL_TREE__HPP
#include "Intervals.hpp"
#include <iostream>

using namespace std;

class Interval_tree{
	private:
		struct it_node{
			int x_center;
			Interval_tree *left;
			Interval_tree *right;
			//struct it_node *left; //left branch
			//struct it_node *right; //right branch
			int ttl_intervals; //ttl intervals that overlap with x_center
			Intervals ml; //intervals that overlap with x_center, sorted by 
			//their starting value in increasing order
			Intervals mr; //intervals that overlap with x_center, sorted by 
			//their ending value in decreasing order.

			/*it_node(int k){
				x_center=k;
				left=NULL;
				right=NULL;
				ttl_intervals=0;
				ml=Intervals("__ml");
				mr=Intervals("__mr");
			}*/

		};
		typedef struct it_node Treenode;

		Treenode *root;
	public:
		Interval_tree(Intervals& intrvl);
		~Interval_tree();
		/* When function returns, ret is going to contain the set of intervals
		that overlap with range (rng_st, rnd_nd)*/
		void search_range(int rng_st, int rng_nd, Intervals& ret);
};



			

#endif
