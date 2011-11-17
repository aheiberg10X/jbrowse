#include "defs.h"


//file mates.c
//It reads the index and the bas info from mate file. ttl_reads is the length of the index table. ttl_groups is the length of the 
//Ins_len_groups table
Mates *get_mate_index(char *mate_file, int *ttl_reads);

//It reads the strand vector from the index file.
long *get_strand_index(char *mate_file, int *len);

//Returns F if the respective bit of the strand_indx is 0, R otherwise
inline char get_strand(long *strand_indx, int strand_len, int pos);

/*It returns an array of integers of length n. The array contains all indices 
of indx whose clones succeed the is_valid() test.
*/
//int *find_valid_clones(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n);

//Checks if the clone with one end on indx[i] satisfies the input condition(???)
//Returns 1 on success, 0 on failure. In case the clone is dangling it returns -1.
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len);

//valid_lst is an array of indices of the array indx. ttl_valid is the length
//of that array. raw_file is the file that contains the raw data. Min_cov is
//the minimum coverage of clones that is desired for an area to be reported.
void clone_coverage(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, char *bam_file, int min_cov);

//It initializes a bitvector of "size" bits. Len is the length of the returned
//long array
long *initialize_bitvector(int size, int *len);

//It returns 1 if the pos'th bit of vec is one, 0 otherwise
inline int already_visited(long *vec, int vec_len, int pos);

//It sets the pos-th bit of vec to 1.
inline void currently_printing(long *vec, int vec_len, int pos);

///prints all clones whose index of indx is found between valid_lst[st] and
///valid_lst[nd]. fp is a file pointer to the raw data file. outfp is a file 
//pointer to the sam output file (stdout most of the time)
//The output includes reads that only cover the region between region_st and region_nd
/////////void print_reads(print_reads_t args);//Mates *indx, int *valid_lst, int valid_st, int valid_nd, int region_st, int region_nd, samfile_t *fp, samfile_t *outfp)

//it prints all reads whose locations on indx are stored in rd_lst. If mate_flag is 1, it prints reads in the same order
//as the rd_lst suggests. Otherwise it sorts rd_lst first and prints all reads sorted according to mapping location.
//BEWARE that if rd_lst gets sorted, its original order of reads gets lost.
//Finally histinfo contains the histogram information.
void print_rd_list(Mates *indx, int len_indx, int *rd_lst, int len_rd_lst, char *inpt_bam_file, char *outfile, int mate_flag, Hist_info histinfo);

//it prints the meta data of the reads whose locations on indx are stored in rd_lst. 
//It sorts the rd_lst and prints the output sorted according to mapping location
//If mate_flag is 1, it prints tuples: loc, loc+len-1, mate_loc, mate_loc+mlen-1
//Otherwise it prints tuples: loc, loc+len-1
//BEWARE that if rd_lst gets sorted, its original order of reads gets lost.

//if outfile==NULL it returns without doing anything.
void print_reduced_rd_list(Mates *indx, int len_indx, long *strand_indx, int strand_len, int *rd_lst, int len_rd_lst, char *outfile, int mate_flag);


typedef struct{
	int st;
	int nd;
	int rd1; //index location of corresponding read (-1 if N/A)
	int rd2; //index location of corresponding read (-1 if N/A)
}Intervals;

typedef struct{
	Intervals *regions;
	int ttl_regions;
	int *evidence;
	int ttl_evidence;
} Intrevidence;


typedef struct{ //the imported table
	char *id;
	int start;
	int end;
	char *chr;
}Imported_info;

typedef struct{ //the join indexes
	int indx1;
	int indx2;
} Join;

//For every mate pair of indx that is in valid_lst it creates an interval
//with st=left.loc, nd=right.loc+len. The returned array has a length of 
//ttl_intrvl.
Intervals *mates2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl);

//For every read of indx that is in valid_lst it creates an interval
//with st=loc, nd=loc+len-1. The returned array has a length of 
//ttl_intrvl.
Intervals *singles2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl);

//For every entry i of imp that is in valid_lst it creates in interval with st=start, nd=end and rd1=i. The
//returned array has a length of ttl_intrvl.
/////Intervals *imported2intervals(Imported_info *imp, int len, int *valid_lst, int ttl_valid, int *ttl_intrvl);

//The function returns the areas that are covered by at least a number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence min_interval_coverage(Intervals *intrvl_lst, int ttl_intrvl, int min_cov);

//The function returns the areas that are covered by at most a positive 
//number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence max_interval_coverage(Intervals *intrvl_lst, int ttl_intrvl, int max_cov);

//int1 and int2 are the arrays of intervals to be intersected of length ttl_int1, ttl_int2 respectively. valid_lst1, valid_lst2 are going to contain
//the (unique) non negative rd values of the intersected intervals.
//////void intersect_intervals(Intervals *int1, Intervals *int2, int ttl_int1, int ttl_int2, int **valid_lst1, int **valid_lst2, int *ttl_valid1, int *ttl_valid2);

//int1 and int2 are the arrays of intervals to be intersected of length ttl_int1, ttl_int2 respectively. The return array contains all
//pairs of rd values of the intersected intervals. ttl_join is the length of the returned array.
Join *join_intervals(Intervals *int1, Intervals *int2, int ttl_int1, int ttl_int2, int *ttl_join);

//it reduces the size of join_lst by applying the filter isvalid_join.
//The joined quantities are either imported tables or an imported table
//and a set of reads. The input contains all possible pointers and the
//ones that is redundant needs to be NULL.
/*void filter_join_list(Join **join_lst, int *ttl_join, Imported_info *tbl1, int ttl_tbl1, Imported_info *tbl2, int ttl_tbl2, Mates *mate_indx, int ttl_reads, long *strand_indx, int strand_len);*/

//The function returns 1 if join_lst[i] satisfies the customizable condition.
//If any of tbl1, tbl2, mate_indx are not necessary, they should be NULL
//inline int isvalid_join(Join *join_lst, int i, Imported_info *tbl1, Imported_info *tbl2, Mates *mate_indx, long *strand_indx, int strand_len);
inline int isvalid_join(Join *join_lst, int i, Mates *mate_indx1, long *strand_indx1, Mates *mate_indx2, long *strand_indx2, Imported_info *imp1, Imported_info *imp2);

//The function returns a sorted list of the uniq indx1 or indx2 of the values 
//of the join_lst according to the value of the side.
int* isolate_join_indexes(Join *join_lst, int ttl_join, int *len_rd_lst, int side);

/////file single_reads.c

//it returns an array of indexes to indx where the reads are mapped between 
//min_loc and max_loc and satisfy the is_valid_single function. n is the length
//of the returned array;
//if min_loc<==-1 max_loc==-1 the entire chromosome is considered.
//int *find_valid_reads(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n, long min_loc, long max_loc);

//Checks if the read indx[i] satisfies the input condition(???)
//Returns 1 on success, 0 on failure. In case the clone is dangling it returns -1.
inline int is_valid_single(Mates *indx, int i, long *strand_indx, int strand_len);


/////file load_table.c

//fname is a tab separated file whose lines contain the information to 
//construct an array of Imported_info objects. The location of id, start, end
//and chr columns are denoted by id_col, st_col, nd_col and chr_col respectively.
//n is the number of the returned array.
//The function ignores all entries of the file whose chromosome field does not
//match the input chr.
Imported_info *load_table(char *fname, int id_col, int st_col, int nd_col, int chr_col, int *n, char *chr);

//it destroys tbl and releases the memory that is reserved by the strings
//of its entries.
void destroy_imported(Imported_info *tbl, int ttl_tbl);

//It returns an array of indexes to those entries of tbl each of which
//satisfies the is_valid_imported test. ttl_valid is the length of the
//returned array.
//int *find_valid_imported(Imported_info *tbl, int ttl_tbl, int *ttl_valid);

//It prints to fname those entries of tbl that are located in valid_lst.
//ttl_tbl is the length of the table, ttl_valid is the length of the valid_lst
void print_imported_list(char *fname, Imported_info *tbl, int ttl_tbl, int* valid_lst, int ttl_valid);

//Checks if the record at tbl[i] satisfies the input condition(???)
//Returns 1 on success, 0 on failure. 
inline int isvalid_imported(Imported_info *tbl, int i, int ttl_table);

//it stores n entries of tble to fname. Since each entry of tbl contains
//two strings (id, chr), it stores those strings immediately afterwards
//one under the other.
void store_imported(char *fname, Imported_info *tbl, int n);

//it assumes the first 4 bytes of fname is the vec_len of the vector 
//that is stored immediately afterwards. It also recreates strings id and chr
//of each entry which are stored immediately after the vector.
Imported_info *retrieve_imported(char *fname, int *vec_len);


///////file interval_tree.c
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
typedef struct it_node Treenode;

//It builds an interval tree for the input array intervals
Treenode *build_intrvl_tree(Intervals *intrvl, int ttl_intervals);

//intrvl_tree is the interval tree to be searched
//rng_st, rng_nd is the query range
//ret is the returned array of intervals of length ttl_ret
//ttl_ret should be initialized to 0 before calling the function.
void search_range(Treenode *intrvl_tree, int rng_st, int rng_nd, Intervals **ret, int *ttl_ret);

//it releases all memory taken by the input interval tree.
void destroy_intrvl_tree(Treenode *root);

//create an interval from the data of read_indx[i]
inline Intervals get_rd_interval(Mates *read_indx, int i);

//create an interval from the data of read_indx[i]
inline Intervals get_imported_interval(Imported_info *tbl, int cur_i);

//it returns an upper or lower bound for the coverage queries. if up_low==0 the
//returned coverage is a lower bound and if up_low==1 the returned coverage is
//an upper bound.
int get_interval_coverage_param(int *up_low);

