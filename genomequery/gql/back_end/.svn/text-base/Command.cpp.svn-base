#include "Command.hpp"
#include <sstream>

using namespace std;
/*If element is already in the vector, it returns
its position. Otherwise it appends it to the end
of the vector and returns its position*/
template <class T>
int find_or_add(Fvector<T> &vec, T element){
	int i;
	for(i=0;i<vec.size();i++)
		if(vec.vec[i]==element) return i;
	vec.push_back(element);
	return i;
}

template <class T>
T unary_operation(T arg, Ops op){
	T result;
	switch(op){
		case ABS:
			return arg>=0?arg:-arg;
			break;
		case  UNR:
			return -arg;
			break;
		case NOT:
			return !arg;
			break;
		default:
			;
	}
}

template <class T>
T arith_operation(T arg1, T arg2, Ops op){
	switch(op){
		case PLUS:
			return arg1+arg2;
		case MINUS:	
			return arg1-arg2;
		case MUL:
			return arg1*arg2;
		case DIV:
			return arg1/arg2;
		case MOD:
			return int(arg1)%int(arg2);
		default:
			;
	}
}

template <class T>
int cmp_operation(T arg1, T arg2, Ops op){
	switch(op){
		case EQ:	
			return arg1==arg2?1:0;
		case NEQ:
			return arg1!=arg2?1:0;
		case GT:
			return arg1>arg2?1:0;
		case GEQ:
			return arg1>=arg2?1:0;
		case LT:
			return arg1<arg2?1:0;
		case LEQ:
			return arg1<=arg2?1:0;
		default:
			;
	}

}

int logic_operation(int arg1, int arg2, Ops op){
	switch(op){
		case AND:
			return arg1&&arg2;
		case OR:
			return arg1||arg2;
		default:
			;
	}
}

//the command is in a string format and the constructor breaks
//the string into a series of set operations. Each symbol of the command
//is represented by a pair of strings on the command: the type and
//the value. The possible values of type are: int, float, char, string,
//operator, intvar, floatvar, charvar, stringvar
Command::Command(string str_cmd){
	cout << str_cmd << endl;
	//cmd.clear(); float_vec.clear(); char_vec.clear(); str_vec.clear(); operators.clear();
	stringstream ss(str_cmd, stringstream::in | stringstream::out);
	string type;
	char buf[256];
	int ival;
	float fval;
	char cval;
	string sval;
	while(ss.get(buf, 255, ' ')){
		type=buf;
		ss.ignore(1);
		if(type=="int"){
			ss >> ival;
			set_int(ival, -1);
		}
		else if(type=="float"){
			ss >> fval;
			set_float(fval, -1);
		}
		else if(type=="char"){
			ss >> cval;
			set_char(cval, -1);
		}
		else if(type=="string"){
			ss.get(buf, 255, ' ');
			//ss.ignore(1);
			sval=buf;
			set_const_str(sval, -1);
		}
		else if(type=="operator"){
			ss.get(buf, 256, ' ');
			//ss.ignore(1);
			sval=buf;
			set_operator(sval, -1);
		}
		else{
			ss.get(buf, 256, ' ');
			//ss.ignore(1);
			sval=buf;
			set_variable(type, sval, -1);
		}
		ss.ignore(1);
	}
	init_state=get_state(); //store the current state. 

}


/*it returns the value of cmd. It must be an integer. If there is at
least an unknown variable on cmd, the method throws an exception*/
int Command::evaluate(){
	static Fvector<uint8_t> stck;
	stck.clear();
	int i(0);
	int int1, int2, int3;
	float flt1, flt2, flt3;
	string str1, str2, str3;
	Ops op;
	Types type1, type2, type3;
	char c1, c2, c3;
	State_length st=get_state();
	int top_stack=-1; //keep track of the top of the stack in terms of the number of symbols that it holds
	for(i=0;i<cmd.size();i+=2){
		if(cmd.vec[i]<=2 || cmd.vec[i]==4){
			//stck.insert(stck.begin(),cmd[i+1]); stck.insert(stck.begin(),cmd[i]);
			stck.push_back(cmd.vec[i]); stck.push_back(cmd.vec[i+1]);
			top_stack+=1;
		}
		else if(cmd.vec[i]!=3) throw new cmd_bad_symbol();
		else{
			//op=get_op(i/2);
			op=get_op(i/2);
			if(op==ABS || op==UNR || op==NOT){
				type1=get_type(top_stack, stck);//(0, stck);
				//type1=get_str(0, stck);
				if(type1==INT){
					int1=get_int(top_stack, stck);//(0, stck);
					int3=unary_operation<int>(int1, op);
					set_int(int3, top_stack, stck); //replace int1 with int3 on the stck
				}
				else if(type1==FLOAT){
					//flt1=get_symbol(i, stck);
					flt1=get_float(top_stack, stck);//(0, stck);
					flt3=unary_operation<float>(flt1, op);
					set_float(flt3, top_stack, stck);
				}
			}
			else{
				type1=get_type(top_stack-1, stck);//(1, stck);
				//type1=get_str(0, stck);
				type2=get_type(top_stack, stck);//(0, stck);
				//type2=get_str(1, stck);
				if(type1==INT && type2==INT){
					int1=get_int(top_stack-1, stck);//(1, stck);
					int2=get_int(top_stack, stck);//(0, stck);
					if(op==PLUS || op==MINUS || op==MUL || op==DIV || op==MOD)
						int3=arith_operation<int>(int1, int2, op);
					else if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ)
						int3=cmp_operation<int>(int1, int2, op);
					else
						int3=logic_operation(int1, int2, op);
					//stck.erase(stck.begin(), stck.begin()+2); 
					//set_int(int3, 0, stck);
					stck.pop(2); top_stack--; set_int(int3, top_stack, stck);
				}
				else if(type1==INT && type2==FLOAT){
					int1=get_int(top_stack-1, stck);//(1, stck);
					flt2=get_float(top_stack, stck);//(0, stck); 
					if(op==PLUS || op==MINUS || op==MUL || op==DIV || op==MOD)
						int3=arith_operation<int>(int1, flt2, op);
					else if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ)
						int3=cmp_operation<float>(int1, flt2, op);
					//stck.erase(stck.begin(), stck.begin()+2);
					//set_int(int3, 0, stck);
					stck.pop(2); top_stack--; set_int(int3, top_stack, stck);
				}
				else if(type1==FLOAT && type2==INT){
					flt1=get_float(top_stack-1, stck);//(1, stck);
					int2=get_int(top_stack, stck);
					if(op==PLUS || op==MINUS || op==MUL || op==DIV || op==MOD)
						int3=arith_operation<int>(flt1, int2, op);
					else if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ)
						int3=cmp_operation<float>(int1, flt2, op);
					//stck.erase(stck.begin(), stck.begin()+2);
					//set_int(int3, 0, stck);
					stck.pop(2); top_stack--; set_int(int3, top_stack, stck);
				}
				else if(type1==FLOAT && type2==FLOAT){
					flt1=get_float(top_stack-1, stck);//(1, stck);
					flt2=get_float(top_stack, stck);///(0, stck);
					//stck.erase(stck.begin(), stck.begin()+2);
					stck.pop(2); top_stack--;
					if(op==PLUS || op==MINUS || op==MUL || op==DIV || op==MOD){
						flt3=arith_operation<float>(flt1, flt2, op);
						//set_float(flt3,0, stck);
						set_float(flt3, top_stack, stck);
					}
					else if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ){
						int3=cmp_operation<float>(flt1, flt2, op);
						//set_int(int3,0, stck);
						set_int(int3, top_stack, stck);
					}
				}
				else if(type1==CHAR && type2==CHAR){
					c1=get_char(top_stack-1, stck);//(1, stck);
					c2=get_char(top_stack, stck);//(0, stck);
					//stck.erase(stck.begin(), stck.begin()+2);
					stck.pop(2); top_stack--;
					if(op==PLUS || op==MINUS || op==MUL || op==DIV || op==MOD){
						c3=arith_operation<char>(c1, c2, op);
						//set_char(c3, 0, stck);
						set_char(c3, top_stack, stck);
					}
					else if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ){
						int3=cmp_operation<char>(c1, c2, op);
						//set_int(int3, 0, stck);
						set_int(int3, top_stack, stck);
					}
				}
				else if(type1==STRING && type2==STRING){
					str1=get_str(top_stack-1, stck);//(1, stck);
					str2=get_str(top_stack, stck);//(0, stck);
					if(op==EQ || op==NEQ || op==LT || op==GT || op==LEQ || op==GEQ)
						int3=cmp_operation<string>(str1, str2, op);
					//stck.erase(stck.begin(),stck.begin()+2);
					//set_int(int3, 0, stck);
					stck.pop(2); top_stack--; set_int(int3, top_stack, stck);
				}

			}
		}
		
	}
	if(stck.size()!=2) throw new cmd_wrong_stack();
	int retval=get_int(0, stck);
	reduce_state(st);
	return retval;
}


/*the main command queue. Each symbol of the 
command consists of 2 8-bit long integers. The first integer shows 
which vector actually contains the symbol and the second integer is
the address of the symbol in that vector. Correspondence between the
first integer and the "physical" vectors: 0 int_vec, 1 float_vec, 
2 char_vec, 3 operators, 4 str_vec - const string,  
5 variable of type int, 6 variable of type float, 7
variable of type char, 8 variable of type string- */
vector<string> Command::get_variables(vector<string> &types, vector<int>& symbol){
	vector<string> ret;
	int i(0);
	for(i=0;i < cmd.size();i+=2){
		if(cmd.vec[i]==5){
			ret.push_back(str_vec[cmd.vec[i+1]]);
			types.push_back("int");
		}
		else if(cmd[i]==6){
			ret.push_back(str_vec[cmd.vec[i+1]]);
			types.push_back("float");
		}
		else if(cmd[i]==7){
			ret.push_back(str_vec[cmd.vec[i+1]]);
			types.push_back("char");
		}
		else if(cmd.vec[i]==8){
			ret.push_back(str_vec[cmd.vec[i+1]]);
			types.push_back("string");
		}
		else continue;
		
		symbol.push_back(i/2); //remember there are 2 entries per symbol
	}
	return ret;
}

//Updates the i'th command of vector with str.
void Command::set_int(int num, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1=0;
	int op_i=find_or_add<int>(int_vec, num);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}

void Command::set_float(float num, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1=1;
	int op_i=find_or_add<float>(float_vec, num);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}

void Command::set_char(char num, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1=2;
	int op_i=find_or_add<char>(char_vec, num);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}


void Command::set_operator(string str, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1=3;
	Ops op;
	if (str=="=") op=ASSIGN;
	else if(str=="==") op=EQ;
	else if(str=="+") op=PLUS;
	else if(str=="-") op=MINUS;
	else if(str=="*") op=MUL;
	else if(str=="/") op=DIV;
	else if(str=="%") op=MOD;
	else if(str=="abs") op=ABS;
	else if(str=="!=") op=NEQ;
	else if(str==">") op=GT;
	else if(str==">=") op=GEQ;
	else if(str=="<=") op=LEQ;
	else if(str=="<") op=LT;
	else if(str=="-unr") op=UNR;
	else if(str=="&&") op=AND;
	else if(str=="||") op=OR;
	else if(str=="!") op=NOT;

	int op_i=find_or_add<Ops>(operators, op);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}

void Command::set_const_str(string str, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1=4;
	int op_i=find_or_add<string>(str_vec, str);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}


void Command::set_variable(string type, string name, int i, Fvector<uint8_t>& lst){
	uint8_t cmd1;
	int op_i;
	if(type=="intvar") cmd1=5;	
	else if(type=="floatvar") cmd1=6;
	else if(type=="charvar") cmd1=7;
	else if(type=="stringvar") cmd1=8;
	else throw new cmd_bad_type();
	op_i=find_or_add<string>(str_vec, name);
	if (i<0){
		lst.push_back(cmd1); lst.push_back(op_i);
	}
	else{
		lst.vec[2*i]=cmd1;
		lst.vec[2*i+1]=op_i;
	}
}



