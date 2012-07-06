#ifndef __TEXT_TABLE__HPP
#define __TEXT_TABLE__HPP

#include "Table.hpp"
#include "defs.hpp"
#include "Command.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

class Text_table: public Table{
	protected:
		//the get function suite fetches a column from a line of the data
		//and converts it to the respective type each time.
		/*int get_int(int column, string line);
		string get_string(int column, string line);
		char get_char(int column, string line);
		int get_int(int column, string line);*/
		/*It updates the variables of cmd with their values from the input line.
The input vectors are the outcome of the call cmd.get_variables*/
		void resolve_variables(string ln, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos);
		int eval_condition(string line);


	public:
		//it updates file_loc
		Text_table(string file):Table(file){;}
	
		void eval_expressions(string line, int& expr1, int &expr2);

		/*it creates a new file that contains only the lines that 
		pass the filtering*/
		void filter(string dst_file);
	
		struct __iterator{
			typedef struct __iterator Self;

			typedef std::input_iterator_tag category;
			typedef int value_type;
			typedef int* pointer;
			typedef int& reference;
			
		
			string buf;
			int max_length;
			int ttl;
			ifstream *inf;
			string name;
			int isopen;

			explicit __iterator(string &file){
				inf=new ifstream();
				inf->open(file.c_str());
				inf->seekg(0, ios::end);
				max_length=inf->tellg();
				inf->seekg(0, ios::beg);
				ttl=0;
				name=file;
				isopen=1;
				while(getline(*inf, buf)) //read the first line
					if(buf.size()>0 && buf[0]!='#' && buf[0]!='>') break;
				ttl+=buf.size();
			}

			explicit __iterator(int k){
				ttl=-1;
				max_length=-1;
				inf=new ifstream();
				name="k";
				isopen=0;
			}

			explicit __iterator(){
				ttl=-1;
				max_length=-1;
				isopen=0;
			}

			~__iterator(){ 
				if(isopen)
					inf->close(); 
				delete inf;
			}

			
			string operator*(){ return buf;}

			Self& operator++(){
				if(isopen){
					while(getline(*inf, buf))
						if(buf.size()>0 && buf[0]!='#' && buf[0]!='>') break;
					ttl+=buf.size();
				}
				return (*this);
			}

			Self& operator ++(int k){
				if(isopen){
					while(getline(*inf, buf))
						if(buf.size() && buf[0]!='#' && buf[0]!='>') break;
					ttl+=buf.size();
				}	
				return (*this);
			}
			
			bool operator==(const Self& x){
				if (ttl==x.ttl) return 1;
				if ( (!(*(inf)) && x.ttl<0) || (!(*(x.inf)) && ttl<0)) return 1;
				else return 0;
			}
			bool operator!=(const Self &x){ 
				if (ttl==x.ttl) return 0;
				if ( (!(*inf) && x.ttl<0) )
					return 0;
				else if (!(*(x.inf)) && ttl<0) 
					return 0;
				else return 1;
			}
		};
		typedef __iterator iterator;

		iterator& begin(){ 
			iterator *it=new iterator(file_loc);
			return *it;
			//return iterator(file);

		}
		iterator& end(){ ///////Weird but it gets the job done.
			static iterator *it=NULL;
			if (it==NULL){
				it=new iterator(-1);
			}
			return *it;
		}


		void resolve_variables(iterator &it, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
			resolve_variables(*it, cmd, variables, types, symbol_pos);
		}
		
		/*It prints the contents of the table that are indicated by flt*/
		void print(string prefx, vector<int>& flt);
	
		/*It prints all contents of the table*/
		void print(string prefx);

		/*It prints the association between the rank of an entry in the
		original file and the rank of the same entry in the output txt file 
		after applying filter flt*/
		void print_metalinks(string prefix, vector<int> &flt);



};
#endif
