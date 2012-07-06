#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <stdint.h>
#include "exceptions.hpp"
#include "Fvector.hpp"

using namespace std;

enum Types{INT, FLOAT, CHAR, STRING};
enum Ops{ASSIGN, EQ, PLUS, MINUS, MUL, DIV, MOD, ABS, NEQ, GT, GEQ, LEQ, LT, UNR, AND, OR, NOT};

class Command{
	private:

		Fvector<uint8_t> cmd; /*the main command queue. Each symbol of the 
		command consists of 2 8-bit long integers. The first integer shows 
		which vector actually contains the symbol and the second integer is
		the address of the symbol in that vector. Correspondence between the
		first integer and the "physical" vectors: 0 const_int, 1 const_float, 
		2 const_char, 3 operators, 4 str_vec - const string,  
		5 variable of type int, 6 variable of type float, 7
		variable of type char, 8 variable of type string- */

		Fvector<int> int_vec; //unique integers
		Fvector<float> float_vec; //unique floats
		Fvector<char> char_vec; //unique const characters
		Fvector<string> str_vec; //unique variable names and const strings
		//vector<string> type_vec; //unique types
		Fvector<Ops> operators; //unique operator symbols.

		/*Set symbol family. It updates the i-th command of lst 
		with either an operator, or a variable, or a const. 
		If i<0 it appends the symbol at the end of lst*/
		void set_operator(string, int i, Fvector<uint8_t>& lst);
		void set_variable(string type, string name, int i, Fvector<uint8_t>& lst);
		void set_const_str(string, int i, Fvector<uint8_t>& lst);
		void set_int(int, int i, Fvector<uint8_t>& lst);
		void set_float(float, int i, Fvector<uint8_t>& lst);
		void set_char(char, int i, Fvector<uint8_t>& lst);
		
		typedef struct{
			int len_int, len_fl, len_char, len_str, len_op;
		}State_length;

		State_length get_state(){
			State_length ret;
			ret.len_int=int_vec.size();
			ret.len_fl=float_vec.size();
			ret.len_char=char_vec.size();
			ret.len_str=str_vec.size();
			ret.len_op=operators.size();
			return ret;
		}

		State_length init_state;
		//It removes the "tails" of each table so that the new lengths much the input.
		void reduce_state(State_length st){
			int_vec.pop(int_vec.size()-st.len_int);
			//int_vec.erase(int_vec.begin()+st.len_int, int_vec.end());
			float_vec.pop(float_vec.size()-st.len_fl);
			//float_vec.erase(float_vec.begin()+st.len_fl, float_vec.end());	
			//char_vec.erase(char_vec.begin()+st.len_char, char_vec.end());
			char_vec.pop(char_vec.size()-st.len_char);
			//str_vec.erase(str_vec.begin()+st.len_str, str_vec.end());
			str_vec.pop(str_vec.size()-st.len_str);
			//operators.erase(operators.begin()+st.len_op, operators.end());
			operators.pop(operators.size()-st.len_op);
		}

	public:
		Command(){
			//cmd.clear(); float_vec.clear(); char_vec.clear(); str_vec.clear();
			//string types[]={"int", "float", "char", "string"};
			//type_vec.insert(types.begin(), types.begin(), types.end());
			Ops ops[]={ASSIGN, EQ, PLUS, MINUS, MUL, DIV, MOD, ABS, NEQ, GT, GEQ, LEQ, LT, UNR, AND, OR, NOT};
			//string ops[]={"=", "==", "+", "-", "*", "/", "%", "abs", "!=", ">", ">=", "<=", "<", "-unr", "&&", "||", "!"};
			//operators.assign(ops, ops+17);
			operators.assign(ops, 17);
		}
		//the command is in a string format and the constructor breaks
		//the string into a series of set operations. Each symbol of the command
		//is represented by a pair of strings on the command: the type and
		//the value. The possible values of type are: int, float, char, string,
		//operator, intvar, floatvar, charvar, stringvar
		Command(string str_cmd);
		/*It returns all unevaluated variables of cmd. When function returns, 
		types[i] contains the type of the i-th return variable and symbol_id[i]
		contains the position of the variable on the cmd symbols*/
		vector<string> get_variables(vector<string>& types, vector<int>& symbol_id);

		/*it returns the value of cmd. It must be an integer. If there is at
		least an unknow variable on cmd, the method throws an exception*/
		int evaluate();
		
		/*It clears any elements that have been added to any lists after
		the constructor call*/
		void clear() {reduce_state(init_state);}
	
		/*Public set_symbol family which updates cmd. */
		inline void set_operator(string str, int i){set_operator(str, i, cmd);}
		inline void set_variable(string type, string name, int i){set_variable(type, name, i, cmd);}
		inline void set_const_str(string str, int i){set_const_str(str, i, cmd);}
		inline void set_int(int n, int i){set_int(n, i, cmd);}
		inline void set_float(float fl, int i){set_float(fl, i, cmd);}
		inline void set_char(char c, int i){set_char(c, i, cmd);}

		private:

		//get_symbol family: It returns the i-th symbol of lst
		inline int get_int(int i, Fvector<uint8_t>& lst){
			if(lst[2*i]==0)
				return int_vec[lst[2*i+1]];
			throw new cmd_bad_content();
		}
		inline float get_float(int i, Fvector<uint8_t>& lst){
			if(lst[2*i]==1)
				return float_vec[lst[2*i+1]];
			throw new cmd_bad_content();
		}
		inline char get_char(int i, Fvector<uint8_t>& lst){
			if(lst[2*i]==2)
				return char_vec[lst[2*i+1]];
			throw new cmd_bad_content();
		}
		inline Types get_type(int i, Fvector<uint8_t>& lst){
			switch(lst[2*i]){
				case 0:
					return INT;
				case 1:
					return FLOAT;
				case 2:
					return CHAR;
				default:
					return STRING;
			}
		}
		inline string get_str(int i, Fvector<uint8_t>& lst){
			switch(lst[2*i]){
				/*case 3: //operator
					return operators[lst[2*i+1]];*/
				case 4:
					return str_vec[lst[2*i+1]];
				case 5: 
					return str_vec[lst[2*i+1]];
				case 6:
					return str_vec[lst[2*i+1]];
				case 7:
					return str_vec[lst[2*i+1]];
				case 8:
					return str_vec[lst[2*i+1]];
				default:
					throw new cmd_bad_content();
			}
		}

		inline Ops get_op(int i, Fvector<uint8_t>& lst){
			if(lst[2*i]==3) return operators[lst[2*i+1]];
			else
				throw new cmd_bad_content();
		}




		/*inline string get_str(int i, vector<uint8_t>& lst){
			if(lst[2*i]!=3 && lst[2*i]!=4) throw Exception;
			return str_vec[lst[2*i+1]];
		}
		inline string get_type(int i, vector<uint8_t>& lst){
			if(lst[2*i]>=6) throw Exception;
			return type_vec[lst[2*i+1]];
		}
		inline string get_op(int i, vector<uint8_t>& lst){
			if(lst[2*i]!=5) throw Exception;
			return operators[lst[2*i+1]];
		}*/

		public:
		//get_symbol family: It returns the i-th symbol of cmd
		inline int get_int(int i){return get_int(i, cmd);}
		inline float get_float(int i){return get_float(i, cmd);}
		inline char get_char(int i){return get_char(i, cmd);}
		inline string get_str(int i){return get_str(i, cmd);}
		inline Types get_type(int i){return get_type(i, cmd);}

		inline void get_const(string& val, int i){val=get_str(i);}
		inline void get_const(int& val, int i){val=get_int(i);}
		inline void get_const(float& val, int i){val=get_float(i);}
		inline void get_const(char& val, int i){val=get_char(i);}
		inline Ops get_op(int i){ return get_op(i, cmd);}
		/*inline int get_symbol(int i){ return get_symbol(i, cmd);}
		inline float get_symbol(int i){ return get_symbol(i, cmd);}
		inline char get_symbol(int i){ return get_symbol(i, cmd);}
		inline string get_str(int i){ return get_str(i, cmd);}
		inline string get_type(int i){ return get_type(i, cmd);}*/


};
#endif
