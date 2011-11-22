
struct Read_str{
	char *name;
	char *read;
	char *qvals;
	char *chromo;
	int loc;
	char strand;
	char *match_descr;
	char *partner_chromo;
	int partner_loc;
	char partner_strand;
};
typedef struct Read_str Read;

//It parses a line of a solexa file and it returns
//the respective Read variable.
Read parse_solexa_read(char *str);
void ioerror(char *msg);
FILE *open_file(char *fname);

//the information of match descriptor.
struct Instr_data{ 
	char type;//'s', 'd', 'i'
	char ref_ltr;
	char rd_ltr;
	int repeat; //only when read contains multiple N's
	int offset; //the reference offset.
};

typedef struct Instr_data Instr;

//it finds all offsets of dscr
//it returns the total number of bits that are
//required for each instruction.
//Return value: An array of instructions.
//Data of *len will contain the length of that array.
Instr *parse_descriptor(char *rd, char *dscr, int *len, int rd_length);
//It takes whatever is necessary in order to initialize the variable members
//of instr_arr[i]
void add_new_instruction(Instr *instr_ar, int i, char type, char ref_ltr, char rd_ltr,int repeat, int offset, int instr_array_length);

//recovers the read based on the input chromosome and the info encoded on olg.
void print_differences(Instr *instr_ar, Read rd, int total_instr, int rd_length);

//Struct that assumes that reads are on an array with the same order as found
//on the raw data. 
struct Mates_t{
	int loc;
	int mate_indx;
	unsigned long long byte_offst;
	unsigned short read_len;
	//unsigned short group_indx;
};
typedef struct Mates_t Mates;

typedef struct{
	int len;
	int sd;
} Ins_len_groups;

inline int min(int a, int b);
inline int max(int a, int b);

//histogram related definitions
typedef int hist_t;
//multiple histograms of different number of bins
typedef struct{
	hist_t **hist;
	int *ttl_bins; //the number of bins per histogram
	int ttl_histograms; //the number of rows of hist
}Mult_hist;

//combine histogram bins with chromosome length.
typedef struct{
	Mult_hist mhist;
	//int ttl_bins;
	int chromo_length;
}Hist_info; 


//it initializes and returns a Mult_hist variable. The bin sizes are the ones
//that Andrew specified in his email on May 5th 2011.
Mult_hist create_histogram();
//For all histograms of hist it increases the bin value that pos falls to. 
extern inline void update_histogram_solo(Mult_hist *mhist, int pos, int chromo_len);
void print_histogram(FILE *fp, Mult_hist mhist, int chromo_len);
void destroy_histogram(Mult_hist *mhist);

//bitmap related functions.

//It returns the value of the pos-th bit of the bitvector. size is the length
//of the bit vector (and not of the array).
inline int get_bit(long *vec, int vec_len, int pos);

//it sets to 1 the pos-th position of vec. Size is the length of the 
//bit vector.
inline void set_bit(unsigned long *vec, int vec_len, int pos);

//it initializes and returns a bit vector of vec_len bits.
unsigned long *initialize_bit_vector(int vec_len);


