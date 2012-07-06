#include "Interval_tree.hpp"
#include <algorithm>
#include "exceptions.hpp"

using namespace std;

/*void ioerror(string msg){
	cout << "FATAL ERROR!!! " << msg << endl;
	exit(1);
}*/
/*It compares two intervals by their starting value*/
template <class T>
inline int intrvl_cmp_st_inc(T a, T b){ return a.st>b.st; }

//it compares two intervals by their nd value. It returns >0 if a<b since
//we want a sorting by decreasing value.
template <class T>
inline int intrvl_cmp_nd_dec(T a, T b){ return b.nd>a.nd; }


Interval_tree::Interval_tree(Intervals& in){
	if (in.size()<=0){
		root=NULL;
		return;
	}
	root=new Treenode;
	Intervals left_int("left"), center_int("center"), right_int("right");
	int center=in.size()/2;
	int x_center=in.vec[center].st;
	root->x_center=x_center;
	//allocate intrvl to new arrays according to x_center
	for(int i=0;i<in.size();i++){
		if(in.vec[i].nd < x_center) //left of x_center
			left_int.vec.push_back(in.vec[i]);
		else if(in.vec[i].st > x_center) //right of x_center
			right_int.vec.push_back(in.vec[i]);
		else
			center_int.vec.push_back(in.vec[i]);
	}

	///root->ml=new Intervals;
	///root->mr=new Intervals;
	//copy center_int to ml, mr
	root->ml.vec.assign(center_int.vec.begin(), center_int.vec.end());
	root->mr.vec.assign(center_int.vec.begin(), center_int.vec.end());

	sort(root->ml.vec.begin(), root->ml.vec.end(), intrvl_cmp_st_inc<Intervals::Intrvl>	);
	sort(root->mr.vec.begin(), root->mr.vec.end(), intrvl_cmp_nd_dec<Intervals::Intrvl> );
	center_int.clear();
	root->left=new Interval_tree(left_int);
	root->right=new Interval_tree(right_int);
}

Interval_tree::~Interval_tree(){
	if (root==NULL) return;
	if (root->left!=NULL) delete root->left;
	root->left=NULL;
	if(root->right!=NULL) delete root->right;
	root->right=NULL;
	if(root->ml.size()>0) root->ml.clear();
	if(root->mr.size()>0) root->mr.clear();
	delete root;
	root=NULL;
}


inline int overlaps(int st1, int nd1, int st2, int nd2){
	if(st1<=nd2 && st2<=nd1) return 1;
	else return 0;
}

void Interval_tree::search_range(int rng_st, int rng_nd, Intervals& ret){
	int i=0;
	Treenode *cur=root;
	if(root==NULL) return;// ret;
	if( (rng_st<=(cur->x_center)) && (rng_nd>=(cur->x_center))){ //x_range is in the middle of the range
		if(rng_st!=(cur->x_center)) //no need to go deeper if rng_st==x_center
			cur->left->search_range(rng_st, rng_nd, ret);
		if(rng_nd!=(cur->x_center)) //similarly no need to go deeper if rng_nd==x_center
			cur->right->search_range(rng_st, rng_nd, ret);
	}

	else if((rng_st<= (cur->x_center)) && (rng_nd<=(cur->x_center))){ //both rng_st, rng_nd "left" of x_center
		cur->left->search_range(rng_st, rng_nd, ret);
	}
	else if((rng_st >= (cur->x_center)) && (rng_nd>=(cur->x_center))){ //both rng_st, rng_nd "right" of x_center
		cur->right->search_range(rng_st, rng_nd, ret);
	}
	else
		throw new interval_tree_search();

	for(i=0;i< cur->mr.vec.size(); i++){
		if(overlaps(cur->mr.vec[i].st, cur->mr.vec[i].nd, rng_st, rng_nd))
			ret.vec.push_back(cur->mr.vec[i]);
	}
	return; //ret
}



	

