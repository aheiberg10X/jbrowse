#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "all_tools.h"

/*void ioerror(char *msg){
	printf("FATAL ERROR!!! %s\n",msg);
	exit(1);
}

typedef struct{
	int st;
	int nd;
}Intervals;

struct it_node{
	int x_center;
	struct it_node *left; //left branch
	struct it_node *right; //right branch
	int ttl_intervals; //ttl intervals that overlap with x_center
	Intervals *ml; //intervals that overlap with x_center, sorted by 
		//their starting value in increasing order
	Intervals *mr; //intervals that overlap with x_center, sorted by 
		//their ending value in decreasing order.
};
typedef struct it_node Treenode;*/

void traverse_tree(Treenode *root){
	if(root==NULL) return;
	int i=0;
	printf("x_center: %d\n",root->x_center);
	for(i=0;i<root->ttl_intervals;i++){
		printf("interval st: %d nd: %d\n",root->ml[i].st, root->ml[i].nd);
	}
	printf("\n");
	traverse_tree(root->left);
	traverse_tree(root->right);
	return;
}

/*lst is a pointer to an array of pointers of Intervals of length n.
The function appends val to lst while it updates n*/
/*void append_interval(Intervals **lst, Intervals val, int *n){
	Intervals *tmp;
	int cnt=(*n)+1;
	tmp=(Intervals*)realloc(*lst, cnt*sizeof(Intervals));
	if (tmp==NULL) ioerror ("Cannot find memory space to store an interval");
	tmp[cnt-1]=val;
	*lst=tmp;
	*n=cnt;
}*/

/*typedef struct{
	Treenode *root;
	Intervals *intrvl;
	int ttl_intervals;
}Tree;*/

//It compares two intervals by their starting value
inline int intrvl_cmp_st_inc(const void *a, const void *b){
	Intervals *ia=(Intervals*)a;
	Intervals *ib=(Intervals*)b;
	return (ia->st) - (ib->st);
}

//it compares two intervals by their nd value. It returns >0 if a<b since
//we want a sorting by decreasing value.
inline int intrvl_cmp_nd_dec(const void *a, const void *b){
	Intervals *ia=(Intervals*)a;
	Intervals *ib=(Intervals*)b;
	return (ib->nd) - (ia->nd);
}

//It builds an interval tree for the input array intervals
Treenode *build_intrvl_tree(Intervals *intrvl, int ttl_intervals){
	if (ttl_intervals<=0)
		return NULL;
	int i=0;
	Treenode *ret=(Treenode*)malloc(sizeof(Treenode));
	Intervals *left_int, *center_int, *right_int;
	int ttl_left=0, ttl_center=0, ttl_right=0;
	int center=ttl_intervals/2;
	int x_center=intrvl[center].st;
	ret->x_center=x_center;
	left_int=(Intervals*)malloc(sizeof(Intervals)*ttl_intervals);
	center_int=(Intervals*)malloc(sizeof(Intervals)*ttl_intervals);
	right_int=(Intervals*)malloc(sizeof(Intervals)*ttl_intervals);
	//allocate intrvl to new arrays according to x_center
	for(i=0;i<ttl_intervals;i++){
		if(intrvl[i].nd<x_center) //left of x_center
			left_int[ttl_left++]=intrvl[i];
		else if(intrvl[i].st>x_center) //right of x_center
			right_int[ttl_right++]=intrvl[i];
		else
			center_int[ttl_center++]=intrvl[i];
	}
	ret->ml=(Intervals*)malloc(sizeof(Intervals)*ttl_center);
	ret->mr=(Intervals*)malloc(sizeof(Intervals)*ttl_center);
	//copy center_int to ml, mr
	for(i=0;i<ttl_center;i++){
		ret->ml[i]=center_int[i];
		ret->mr[i]=center_int[i];
	}
	ret->ttl_intervals=ttl_center;
	qsort(ret->ml, ttl_center, sizeof(Intervals), intrvl_cmp_st_inc);
	qsort(ret->mr, ttl_center, sizeof(Intervals), intrvl_cmp_nd_dec);
	free(center_int);//don't need it any more.
	ret->left=build_intrvl_tree(left_int, ttl_left);
	ret->right=build_intrvl_tree(right_int, ttl_right);
	free(left_int);
	free(right_int);
	return ret;
}

void destroy_intrvl_tree(Treenode *root){
	if(root==NULL) return;
	if(root->left!=NULL) destroy_intrvl_tree(root->left);
	root->left=NULL;
	if(root->right=NULL) destroy_intrvl_tree(root->right);
	root->right=NULL;
	if(root->ml!=NULL) free(root->ml);
	if(root->mr!=NULL) free(root->mr);
	free(root);
}


inline int overlaps(int st1, int nd1, int st2, int nd2){
	if(st1<=nd2 && st2<=nd1) return 1;
	else return 0;
}

void search_point(Treenode *intrvl_tree, int key, Intervals **ret, int *ttl_ret){
	int i=0;
	Treenode *cur=intrvl_tree;
	if (intrvl_tree==NULL){
		*ttl_ret=0;
		return;
	}
	if (key < cur->x_center){
		search_point(cur->left, key, ret, ttl_ret);
		for(i=0; i< cur->ttl_intervals; i++){
			if(cur->ml[i].st > key)
				break;
			else if (overlaps(cur->ml[i].st, cur->ml[i].nd,key, key))
				append_interval(ret, cur->ml[i], ttl_ret);
		}
	}
	else if (key > cur->x_center){
		search_point(cur->right, key, ret, ttl_ret);
		for(i=0; i< cur->ttl_intervals; i++){
			if(cur->mr[i].nd < key)
				break;
			else if (overlaps(cur->mr[i].st, cur->mr[i].nd, key, key))
				append_interval(ret, cur->mr[i], ttl_ret);
		}
	}
	else{
		for(i=0; i< cur->ttl_intervals; i++)
			append_interval(ret, cur->mr[i], ttl_ret);
	}
	return;
}

//intrvl_tree is the interval tree to be searched
//rng_st, rng_nd is the query range
//ret is the returned array of intervals of length ttl_ret
//ttl_ret should be initialized to 0 before calling the function.
void search_range(Treenode *intrvl_tree, int rng_st, int rng_nd, Intervals **ret, int *ttl_ret){
	int i=0;
	Treenode *cur=intrvl_tree;
	if (intrvl_tree==NULL){
		return;
	}
	if( (rng_st<=(cur->x_center)) && (rng_nd>=(cur->x_center))){ //x_range is in the middle of the range
		if(rng_st!=(cur->x_center)) //no need to go deeper if rng_st==x_center
			search_range(cur->left, rng_st, rng_nd, ret, ttl_ret);
		if(rng_nd!=(cur->x_center)) //similarly no need to go deeper if rng_nd==x_center
			search_range(cur->right, rng_st, rng_nd, ret, ttl_ret);
	}
	else if((rng_st<= (cur->x_center)) && (rng_nd<=(cur->x_center))){ //both rng_st, rng_nd "left" of x_center
		search_range(cur->left, rng_st, rng_nd, ret, ttl_ret);
	}
	else if((rng_st >= (cur->x_center)) && (rng_nd>=(cur->x_center))){ //both rng_st, rng_nd "right" of x_center
		search_range(cur->right, rng_st, rng_nd, ret, ttl_ret);
	}
	else
		ioerror("interval tree search_range missed a case");

	for(i=0;i< cur->ttl_intervals; i++){
		if(overlaps(cur->mr[i].st, cur->mr[i].nd, rng_st, rng_nd))
			append_interval(ret, cur->mr[i], ttl_ret);
	}
}






/*int main(){
	Intervals *intr=(Intervals*)malloc(sizeof(intr)*100);
	intr[0].st=1; intr[0].nd=4;
	intr[1].st=2; intr[1].nd=3;
	intr[2].st=5; intr[2].nd=7;
	intr[3].st=8; intr[3].nd=10;
	int ttl_intervals=4;

	Treenode *tree=build_intrvl_tree(intr, ttl_intervals);

	Intervals *ret=(Intervals*)malloc(sizeof(intr)*10);
	int ttl_ret=0;
	//traverse_tree(tree);
	//search_point(tree, 4, &ret, &ttl_ret);
	search_range(tree, 5, 9, &ret, &ttl_ret);
	printf("ttl_ret: %d\n",ttl_ret);

	int i=0;
	for(i=0;i<ttl_ret;i++){
		printf("st: %d nd: %d\n",ret[i].st, ret[i].nd);
	}

}*/
	
	

