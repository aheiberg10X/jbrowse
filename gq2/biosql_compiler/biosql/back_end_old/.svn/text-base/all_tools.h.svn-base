#include "defs.h"


//file mate_coverage.c
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
int *find_valid_clones(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n);

//Checks if the clone with one end on indx[i] satisfies the input condition(???)
//Returns 1 on success, 0 on failure. In case the clone is dangling it returns -1.
inline int is_valid(Mates *indx, int i, long *strand_indx, int strand_len);

//valid_lst is an array of indices of the array indx. ttl_valid is the length
//of that array. raw_file is the file that contains the raw data. Min_cov is
//the minimum coverage of clones that is desired for an area to be reported.
void clone_coverage(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, char *bam_file, int min_cov);



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

//For every mate pair of indx that is in valid_lst it creates an interval
//with st=left.loc, nd=right.loc+len. The returned array has a length of 
//ttl_intrvl.
Intervals *mates2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl);

//For every read of indx that is in valid_lst it creates an interval
//with st=loc, nd=loc+len-1. The returned array has a length of 
//ttl_intrvl.
Intervals *singles2intervals(Mates *indx, int len_indx, int *valid_lst, int ttl_valid, int *ttl_intrvl);


//The function returns the areas that are covered by at least a number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence min_interval_coverage(Mates *indx, int len_indx, Intervals *intrvl_lst, int ttl_intrvl, int min_cov);

//The function returns the areas that are covered by at most a positive 
//number of intervals.
//Intrvl_list is an array of Intervals sorted by starting location. ttl_intrvl is the length
//of that array. Min_cov is the minimum coverage of clones that is desired for an area to be reported.
Intrevidence max_interval_coverage(Mates *indx, int len_indx, Intervals *intrvl_lst, int ttl_intrvl, int max_cov);


/////file single_reads.c

//it returns an array of indexes to indx where the reads are mapped between 
//min_loc and max_loc and satisfy the is_valid_single function. n is the length
//of the returned array;
//if min_loc<==-1 max_loc==-1 the entire chromosome is considered.
int *find_valid_reads(Mates *indx, int ttl_reads, long *strand_indx, int strand_len, int *n, long min_loc, long max_loc);

//Checks if the read indx[i] satisfies the input condition(???)
//Returns 1 on success, 0 on failure. In case the clone is dangling it returns -1.
inline int is_valid_single(Mates *indx, int i, long *strand_indx, int strand_len);

