#ifndef __READS__HPP
#define __READS__HPP

#include "Table.hpp"
#include "Histogram.hpp"
#include "bam.h"
#include "sam.h"
#include "Command.hpp"
#include "exceptions.hpp"
#include "Bit_vector.hpp"
#include "defs.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


class Reads : public Table{
	protected:
		typedef struct{
			int loc;
			int mate_indx;
			unsigned long long byte_offst;
			unsigned short read_len;
		} Mates;
		int ttl_mates;
		string chromosome;
		int chr_len;
		vector<Mates> rindx;
		Bit_vector strand_vec;
		string indx_file;
		string bam_file;
		
		/*It returns the strand of the i'th position*/
		char get_strand(int i){
			if(i<=0 || i>ttl_mates) throw new out_of_bounds();
			if (strand_vec.get_bit(i)) return 'R';
			else return 'F';
		}

		/*It prints to the destination bam_file those reads that are present in 
		the input vector. It also generates the histogram info which is 
		necessary for the visualization.*/
		void print_bam (vector<int>&, string dst_file, Histogram& hist);
		/*it prints the meta data of the reads whose locations on indx are 
		stored in rd_lst. It sorts the rd_lst and prints the output sorted
		according to mapping location. If mate_flag is 1, it prints tuples:
		loc, loc+len-1, mate_loc, mate_loc+mlen-1. Otherwise it prints tuples:
		loc, loc+len-1*/
		void print_reduced(vector<int>& flt, string outfile, int mate_flag);
		/*It prints the association between the rank of a read in the original 
		bam file and the rank of the same read in the output bam file after 
		applying filter flt*/
		void print_metalinks(vector<int>& flt, string outfile);

		int eval_rd_condition(int i); //to be used by filtering

	
	public:
		/*it loads rindx, chr from the input file. */
		Reads(string &indx);

		void eval_expressions(int i, int& expr1, int &expr2);

		/*filter family: It applies a filter to the table or to a table that is
		a result of previous filters. In any case, the function either returns
		a vector of the addresses of reads that pass the filter or it stores
		this vector to an output destination file. */

		//void load(){cout << "I love you too" << endl;} //virtual from Table

		
		vector<int>* filter();
		vector<int>* filter(vector<int>& prev_filt);
		void filter(vector<int>& prev_filt, string dst_file);
		void filter(string prev_filt_file, string dst_file){
			ifstream infile(prev_filt_file.c_str(), ifstream::in);
			int len;
			vector<int> prev_filt=get_vector<int>(infile, len);
			filter(prev_filt, dst_file);
			prev_filt.clear();
			infile.close();
		}
		void filter(string dst_file);
		/*It updates the variables of cmd with their values from the i-th read.
		The input vectors are the outcome of the call cmd.get_variables*/
		void resolve_variables(int i, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos);


		/*It prints the reads that are indicated by flt. Note that the first
		character of prefx is always 0 or 1 and carries the mate_flag
		information. So the real prefix of the output products is 
		prefx[1:len(prefx)-1]. Output products: .bam, .short, .hist*/
		void print(string prefx, vector<int> &flt);
		
		/*It returns the total number of reads*/
		int size(){return ttl_mates;}

		/*It returns the mate of the i-th read*/
		int get_mate(int i){return rindx[i].mate_indx;}


	private:
		struct __iterator{
			typedef struct __iterator Self;

			typedef std::input_iterator_tag category;
			typedef int value_type;
			typedef int* pointer;
			typedef int& reference;
			
		
			int state;
			int max_state;

			explicit __iterator(int ttl_reads){
				max_state=ttl_reads;
				state=1; //read0 is always dummy
			}

			explicit __iterator(int ttl_reads, int init_state){
				max_state=ttl_reads;
				state=init_state; //read0 is always dummy
			}



			~__iterator(){}

			
			int operator*(){ return state;}

			Self& operator++(){ 
				if(state<max_state) state++;
				return (*this);
			}

			Self& operator ++(int k){
				if(state<max_state) state++;
				return (*this);
			}
			
			bool operator==(const Self& x){
				return state==x.state;
			}
			bool operator!=(const Self &x){ 
				return state!=x.state;
			}
		};
	public:
		typedef __iterator iterator;
	
		iterator begin(){ 
			return iterator(ttl_mates);
		}
		iterator end(){ ///////Weird but it gets the job done.
			return iterator(ttl_mates, ttl_mates);
		}

		void resolve_variables(iterator it, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
		resolve_variables(*it, cmd, variables, types, symbol_pos);
		}


		
};
#endif
