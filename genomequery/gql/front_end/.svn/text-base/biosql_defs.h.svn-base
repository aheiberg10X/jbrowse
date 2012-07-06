#include<iostream>
#include<string>
#include<vector>
#include<string>
#include<sstream>

using namespace std;

void ioerror(string msg);

int symerror(string msg);

struct __st_node{//an entry of the symbol table
	string name;
	string type;//integer-string-table-char etc
	string kind;//attribute-table etc
	string place;//used for the intermediate code
	struct __st_node *owner;//a pointer to the owner entity. In case of an attribute, 
	//the owner is the table that contains it. NULL if no owner exists.
};
typedef struct __st_node st_node;


void emit(vector<string>& emit_lst, string code){
	emit_lst.push_back(code);
	//cout << code << endl;
}

void output_emit_list(vector<string>& emit_lst){
	for(vector<string>::iterator it=emit_lst.begin();it!=emit_lst.end();it++)
		cout << *it << endl;
}

/*If arg_cnt<0 place=attr_name. Otherwise place=arg_arg_cnt*/
string update_attribute_place(int &arg_cnt, string prefx, string atr_name){
	if(arg_cnt<0) return atr_name;
	stringstream ss;
	ss << prefx << ":arg" << arg_cnt++;
	return ss.str();
}

/*if there is no conflict with the contents of st, it adds to it a node with
fields the input arguments name, kind etc*/
void check_and_add(map<string, list<st_node> >& st, string name, string kind, string type, string place, st_node* owner);

/*in case there are no coflicts between the given table_name and st, it copies
all entries whose owner is the table with the given table_name to tmp_st. */

void update_active_scope(map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, string& table_name);

/*It disambiguates conflicting attributes of the same owner by appending
subscripts 1 and 2 (e.g. arg1 and arg2 for arg)*/
void disambiguate(map<string, list<st_node> >&st);

/*It checks whether the contents of select_lst appear in tmp_st and copies them
to st. If select_lst contains * it copies everything from tmp_st*/
void copy_to_symbol_table(vector<string>& select_lst, map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, st_node *owner);

st_node* lookup(map<string, list<st_node> >& st, string name, string kind, st_node* owner);

//It copies all fields of src to dst. 
///void copy_nodes(st_node* dst, st_node* src);

/*it creates a node with the given fields*/
st_node* create_node(string name, string type, string kind, st_node* owner);

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int + float gives float*/
string arith_type_compatibility(string type1, string type2);

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int < float gives float*/
string cmp_type_compatibility(string type1, string type2);

/*It returns the position of the element, -1 if failure*/
template<class D>
int search_vector(vector<D>& lst, D element);

//if nd==NULL it calls symerror with msg and name as parameters
//void check_for_symerror(st_node *nd, char *msg, char *name);
int symerror(string);
extern "C"
{
	int yylex(void);
	int yyparse(void);
	int yywrap(){
		return 1;
	}
	int yyerror(char *);
}

