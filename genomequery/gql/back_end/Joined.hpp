#ifndef __JOINED_HPP__
#define __JOINED_HPP__
#include "Table.hpp"
#include "defs.hpp"
#include "Command.hpp"
#include "Intervals.hpp"
#include "Reads.hpp"
#include "Text_table.hpp"
#include <string>
#include <vector>
#include <list>

using namespace std;

class Joined: public Table{
	protected:
		string parent1; //file location of src1
		string parent2; //file location of src2
		string type1; //Reads - Text_table - Intervals - Joined
		string type2; 
		typedef struct{
			int indx1;
			int indx2;
		}Pair;
		vector<Pair> vec;

		/*It joins all non negative rd values of int1 with all non negative rd
		values of int2;*/
		void join_intervals(Intervals::Intrvl &int1, Intervals::Intrvl &int2);
		
		/*It evaluates cmd on the i-th record. vec_addr is the set of the
		addresses of the vectors that dereference the various variables of
		cmd. (they are returned by the dereference command*/
		void resolve_variables(int i, vector<long>& vec_addr, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos);

		int eval_condition(int i);

	public:
	/*if any of type1,2 are Reads, the respective first character of src caries
	the mates_on information*/
		Joined(string dst, string src1, string src2, string type1, string type2, Intervals &intrvl1, Intervals &intrvl2);

		Joined(string dst, string src1, string src2, string type1, string type2);
		Joined(string dst);

		void eval_expressions(int i, int& expr1, int &expr2);

		
		/*it stores the contents of the object to file_loc*/
		void store();
		
		/*It overwrites the contents of the object from the contents of 
		file_loc*/
		void load();

		//similar to load, but it doesn't update any object variables*/
		void load (string fname, vector<Pair>& vec, string &p1, string &t1, string &p2, string &t2) const;

		int size(){return vec.size();}
		//it gets the value of the command for each Pair of vec.
		///vector<int> dereference(list<Command>);

		/*It returns the value of the variable for each entry of the vec. The 
		variable string should be of the format file1+file2+...+filen.arg*/
		template <class T>
		vector<T>* dereference(string var, string type);

		//the resulted pairs are stored in the output file dst.
		void filter(string dst);
		
		//it isolates the unique entries of table table_name. If table_name
		//is of type Reads, the output contains pointers only. Otherwise the
		//output contains materialized data.
		void isolate_unique(string table_name, string dst_name);

		/*It isolates the data from the individual tables. flt contains the 
		entries that need to be considered.*/
		void print(string out_prefx, vector<int>& flt);
		
		/*When all entries of vec need to be considered*/
		void print(string out_prefx);

		vector<vector<int> > get_links(vector<int>& flt, vector<string>& tables);

			private:
		struct __iterator{
			typedef struct __iterator Self;

			typedef std::input_iterator_tag category;
			typedef int value_type;
			typedef int* pointer;
			typedef int& reference;
			
		
			int state;
			int max_state;

			explicit __iterator(int mx){
				max_state=mx;
				state=0; 
			}

			explicit __iterator(int mx_state, int init_state){
				max_state=mx_state;
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
			return iterator(vec.size());
		}
		iterator end(){ ///////Weird but it gets the job done.
			return iterator(vec.size(), vec.size());
		}


		void resolve_variables(iterator it, vector<long>& vec_addr, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
		resolve_variables(it, vec_addr, cmd, variables, types, symbol_pos);
		}
		/*void resolve_variables(iterator it, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
		resolve_variables(*it, cmd, variables, types, symbol_pos);
		}*/



};

#endif
