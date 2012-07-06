#ifndef __INTERVALS__HPP
#define __INTERVALS__HPP
#include "Table.hpp"
//#include "defs.hpp"
#include "Reads.hpp"
#include "Text_table.hpp"
#include <string>
#include <vector>
#include <iostream>
//#include "Interval_tree.hpp"

class Interval_tree;
class Joined;

using namespace std;



class Intervals: public Table{
	public:
		typedef struct{
			int st;
			int nd;
			int rd1;
			int rd2;
		} Intrvl;

	protected:

		vector<Intrvl> vec;
		
		
		/*It updates the variables of cmd with their values from the input
		Intrvl. The input vectors are the outcome of the call
		cmd.get_variables*/
		void resolve_variables(Intrvl in, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos);

		int eval_condition(int i);
		void eval_expressions(int i, int& expr1, int& expr2);


	public:

	/*constructor suite: It calls intervals on each of the other table types*/
		Intervals(Reads &tbl, string filtered_file, int mates_on, string dst_file);
		Intervals(Text_table& tbl, string dst_file);
		Intervals(Joined& tbl, string dst_file);

		Intervals(string dst_file):Table(dst_file){;}

		Intervals ():Table("tmp"){;}

		Intervals(Intervals& tbl, string dst_file);

		/*it creates another set of intervals, subset of the current one and 
		stores in a file. It either returns it or it stores it in a file*/
		Intervals filter(string dst_file);

		
		/*It creates a new set of intervals that are covered by at least coverage
		intervals of the current set*/
		Intervals merge_low_bound(unsigned coverage, string dst);
		/*It creates a new set of intervals that are covered by at most coverage
		intervals of the current set*/
		Intervals merge_upper_bound(unsigned coverage, string dst);

		

		void print();
		void print(string prefx, vector<int>& flt);
		void print(string prefx);
		/*It prints the association between the rank of an entry in the original 
		file and the rank of the same entry in the output file after applying 
		filter flt*/
		void print_metalinks(string outfile, vector<int> &flt);

		void store();

		/*It loads intervals from file_loc. Its use is recommended immediately 
		after the constructor call:Intervals(file_loc).*/
		void load();

		void clear(){vec.clear();}


		void append(int st, int nd, int rd1, int rd2){
			Intrvl in;
			in.st=st;
			in.nd=nd;
			in.rd1=rd1;
			in.rd2=rd2;
			vec.push_back(in);
			return;
		}
		Intrvl& operator [](const int i){ return vec[i];}

		int size(){return vec.size();}

		friend class Interval_tree;

	private:
		struct __iterator{
			typedef struct __iterator Self;

			typedef std::input_iterator_tag category;
			typedef int value_type;
			typedef int* pointer;
			typedef int& reference;
			
		
			int state;
			vector<Intrvl> vec;

			explicit __iterator(vector<Intrvl>& vectr, int init_state){
				state=init_state;
				this->vec=vectr;
			}


			~__iterator(){}

			
			Intrvl operator*(){ return vec[state];}

			Self& operator++(){ 
				if(state<vec.size()) state++;
				return (*this);
			}

			Self& operator ++(int k){
				if(state<vec.size()) state++;
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
			return iterator(vec, 0);
		}
		iterator end(){ ///////Weird but it gets the job done.
			return iterator(vec, vec.size());
		}

		void resolve_variables(iterator it, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
		resolve_variables(*it, cmd, variables, types, symbol_pos);
		}

};

#endif
