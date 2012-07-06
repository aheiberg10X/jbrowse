%{
#include<cstdlib>
//#include<stdio.h>
#include<vector>
#include<list>
#include<map>
#include<iostream>
#include"biosql_defs.h"
#include<string>

#define MAX_FROM_TABLES 2048

using namespace std;

/*extern "C"
{
	int yyparse(void);
	int yylex(void);
	int yywrap(){
		return 1;
	}
}*/

extern int lineNo;
extern int errorNo;
int nextstat=1; //keep count of the lines of 

string parsed_table_file;

map<string, list<st_node> > st; //the symbol table
map<string, list<st_node> > tmp_st; //the active symbol table from the from_statements
int arg_cnt=1;
vector<string> emit_lst;
vector<string> select_lst;
st_node *cur_owner=NULL;

%}


%union{
	int int_val;
	std::string* str;
	struct __st_node* entry;
}

%token  CONST_CHAR
%token  CONST_STRING
%token  NUMBER
%token  EQUAL
%token  ASSIGN
%token  STAR
%token  GREATER
%token  LESS
%token  NOT_EQUAL
%token  LESS_EQUAL
%token  GREATER_EQUAL
%token  MUL
%token  DIV
%token  MOD
%token  PLUS
%token  MINUS
%token  AND
%token  OR
%token  NOT
%token  SEMICOLON
%token  DOT
%token  LPAREN
%token  RPAREN
%token  COMMA
%token  INTEGER
%token  FLOAT
%token  CHAR
%token  STRING_TYPE
%token  ID
%token  SELECT
%token  FROM
%token  WHERE
%token  COUNT
%token  TABLE
%token  MAPJOIN
%token	IMPORT
%token	USE
%token  USING
%token	INTERVAL_CREATION
%token	CREATE_INTERVALS
%token	MERGE_INTERVALS
%token  INTERVALS
%token	BOTH_MATES
%token	GENOME
%token	PRINT

%type <str> names
%type <str> numbers
%type <entry> obj_names
%type <entry> table_keyword
%type <entry> table_arg
%type <str> from_arg
%type <entry> lvalue
%type <str> comparison_op
%type <entry> lowest_expr
%type <entry> where_args
%type <entry> arith_expr
%type <str> arith_op


%nonassoc	ID
%right	ASSIGN
%left	OR
%left	AND
%nonassoc	EQUAL GREATER LESS LESS_EQUAL GREATER_EQUAL NOT_EQUAL
%left	PLUS MINUS
%nonassoc	NOT
%left	TK_POSITIVE_SIGN TK_NEGATIVE_SIGN
%nonassoc	TK_LEFT_BRACKET TK_RIGHT_BRACKET TK_LEFT_SQUARE_BRACKET TK_RIGHT_SQUARE_BRACKET



%%
program: table_prototypes genome_names assigned_selects print_statements;

table_prototypes: table_prototype table_prototypes
|
;

table_prototype: table_keyword LPAREN table_args RPAREN SEMICOLON{
	cur_owner=NULL;
}
;


table_keyword: TABLE names{
	if(*$2=="READS") arg_cnt=-1;
	else arg_cnt=1;
	if(lookup(st, *$2, "table", NULL)!=NULL)
		symerror("Multiple definitions for table "+*$2);
	check_and_add(st, *$2, "table", "table", "table", NULL);
	cur_owner=lookup(st, *$2, "table", NULL);
}
	
;

names: ID{
	$$=new string(*(yylval.str));
}
;

numbers: NUMBER{
	$$=new string(*(yylval.str));
}


table_args: table_args COMMA table_arg {
//arguments need to appear in output in the order that the user enders them.

}

|table_arg{

}
;

table_arg: INTEGER names{
	string place=update_attribute_place(arg_cnt, cur_owner->name, *$2);
	check_and_add(st, *$2, "attribute", "int", place, cur_owner);
}
|FLOAT names{
	string place=update_attribute_place(arg_cnt, cur_owner->name, *$2);
	check_and_add(st, *$2, "attribute", "float", place, cur_owner);
}
|CHAR names{
	string place=update_attribute_place(arg_cnt, cur_owner->name, *$2);
	check_and_add(st, *$2, "attribute", "char", place, cur_owner);
}
|STRING_TYPE names{
	string place=update_attribute_place(arg_cnt, cur_owner->name, *$2);
	check_and_add(st, *$2, "attribute", "string", place, cur_owner);
	if(*$2=="chromosome" || *$2=="chr" || *$2=="chromo"){
		stringstream ss;
		ss << arg_cnt-1;
		emit(emit_lst, "table: "+cur_owner->name+" chr_loc: "+ss.str());
	}
}
		
;


genome_names: genome_names genome_names
|GENOME names SEMICOLON{
	//emit(&emit_lst, "genome", $2, NULL, NULL, NULL);
	emit(emit_lst, "genome "+(*$2));
}


assigned_selects: assigned_select assigned_selects
|
;

assigned_select: lvalue ASSIGN select_statement{//emit(&emit_lst, "result_table", ":", $1->name, NULL,NULL); cur_owner=NULL;

	cur_owner=NULL;
	emit(emit_lst, "End_of_sentence");
}
;

lvalue: names{ //$1 should be a new table name which is added to the symbol table
	check_and_add(st, *$1, "table", "table", "table", NULL);
	cur_owner=lookup(st, *$1, "table", NULL);
	emit(emit_lst, "result_table: "+*$1);
	select_lst.clear();
	tmp_st.clear();
}
;

select_statement: SELECT select_args FROM compound_from_arg WHERE where_args{
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "WHERE");
	emit(emit_lst, $6->place);
}
| SELECT select_intrvl_args FROM compound_from_arg{
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "INTERVAL_HANDLING");
}
| SELECT select_args FROM MAPJOIN from_args{
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "MAPJOIN");
}
;

select_intrvl_args: CREATE_INTERVALS LPAREN RPAREN{
	emit(emit_lst, "CREATE_INTERVALS");
	select_lst.push_back("st");
	select_lst.push_back("nd");
}
| MERGE_INTERVALS LPAREN names GREATER numbers RPAREN{
	emit(emit_lst, "MERGE_INTERVALS");
	select_lst.push_back("*");
	//select_lst.push_back("nd");
	emit(emit_lst, "merge > "+*$5);
}
| MERGE_INTERVALS LPAREN names LESS numbers RPAREN{
	emit(emit_lst, "MERGE_INTERVALS");
	select_lst.push_back("*");
	//select_lst.push_back("nd");
	emit(emit_lst, " < "+*$5);
}
;

select_args: STAR{
	select_lst.push_back("*");
}
|select_arg_series
;

select_arg_series: names COMMA select_arg_series{
	select_lst.push_back(*$1);
}
| names{
	select_lst.push_back(*$1);
}
| obj_names{
	//???????? don't do anything for now... return later.
	//add_node(&select_lst, create_node($1->name, NULL, NULL, $1->owner));
}
;

from_args: compound_from_arg COMMA from_args
| compound_from_arg
;

compound_from_arg: from_arg{
	emit(emit_lst, "INPUT: "+*$1);
	
}
| from_arg USING INTERVALS LPAREN arith_expr COMMA arith_expr RPAREN{
	emit(emit_lst, "INPUT: "+*$1);
	emit(emit_lst, "Interval calling on "+*$1);
	emit(emit_lst, "st: "+$5->place);
	emit(emit_lst, "nd: "+$7->place);

}
| from_arg USING INTERVALS LPAREN arith_expr COMMA arith_expr COMMA BOTH_MATES RPAREN{
	emit(emit_lst, "INPUT: "+*$1);
	emit(emit_lst, "Interval calling on "+*$1+ " both_mates");
	//emit(emit_lst, "both_mates");
	emit(emit_lst, "st: "+$5->place);
	emit(emit_lst, "nd: "+$7->place);

}

;

from_arg: names{
	update_active_scope(st, tmp_st, *$1);
	$$=new string(*$1);
	disambiguate(tmp_st);
}
;

where_args: where_args COMMA where_args { //the type of where_args is str and denotes the respective temporary variable
	$$=new st_node();
	*$$=*$1;
	$$->place="( "+$1->place+" ) && ( "+$3->place+" )";
}
|where_args AND where_args{
	$$=new st_node();
	*$$=*$1;
	$$->place="( "+$1->place+" ) && ( "+$3->place+" )";
}
|where_args OR where_args{
	$$=new st_node();
	*$$=*$1;
	$$->place="( "+$1->place+" ) || ( "+$3->place+" )";
}
|NOT where_args{
	$$=new st_node();
	*$$=*$2;
	$$->place="!"+$2->place;
}
|LPAREN where_args RPAREN{
	$$=new st_node();
	*$$=*$2;
	$$->place="( "+$2->place+" )";
}
|lowest_expr{
	$$=new st_node();
	*$$=*$1;
}
;


lowest_expr: arith_expr comparison_op arith_expr{
	$$=new st_node();
	$$->place=$1->place+" "+*$2+" "+$3->place;
	$$->type=cmp_type_compatibility($1->type, $3->type);

}
;

arith_expr: arith_expr arith_op arith_expr{
	$$=new st_node();
	$$->place=$1->place+" "+*$2+" "+$3->place;
	$$->type=arith_type_compatibility($1->type, $3->type);
}	

| arith_op arith_expr{
	$$=new st_node();
	$$->type=$2->type;
	$$->place=*$1+" "+$2->place;
}

| names LPAREN arith_expr RPAREN { //abs(expr)
	if(*$1 != "abs" && *$1 != "ABS")
		symerror("Undefined function call: "+*$1);
	$$=new st_node();
	$$->type=$3->type;
	$$->place=*$1+$3->place;
}

| names	{
	st_node *tmp=lookup(tmp_st, *$1, "attribute", NULL);
	if(tmp==NULL) symerror("Unknown table attribute: "+*$1);
	$$=new st_node();
	*$$=*tmp;
	$$->place=$$->type+"var"+" "+$$->place;
}

| obj_names{
	$$=new st_node();
	*$$=*$1;
	$$->place=$$->type+"var"+" "+$$->place;

}

| NUMBER {
	$$=create_node(*(yylval.str), "int", "", NULL);
	$$->place=$$->name;
}
| CONST_STRING{
	$$=create_node(*(yylval.str), "string", "", NULL);
	$$->place=$$->name;
}

| CONST_CHAR{
	$$=create_node(*(yylval.str), "char", "", NULL);
	$$->place=$$->name;
}
;

obj_names: names DOT names{
	if (lookup(tmp_st, *$1, "table", NULL)==NULL)
		symerror("Parent table "+*$1+" does not appear in FROM table series");
	st_node *tmp_parent=lookup(st, *$1, "table", NULL);
	st_node *tmp=lookup(st, *$3, "attribute", tmp_parent);
	if(tmp==NULL) symerror("Unknown table attribute: "+*$3);
	$$=new st_node();
	//copy_nodes($$, tmp);
	*$$=*tmp;
	$$->place=*$1+"."+tmp->name;
}
;

comparison_op: EQUAL{$$=new string("==");}
			|GREATER_EQUAL{$$=new string(">=");}
			|LESS_EQUAL {$$=new string("<=");}
			|GREATER {$$=new string(">");}
			|LESS {$$=new string("<");}
;

arith_op: PLUS {$$=new string("+");}
		|MINUS{$$=new string("-");}
		|MUL{$$=new string("x");}
		|DIV{$$=new string("/");}
		|MOD{$$=new string("%");}

;

print_statements: print_statement print_statements
| 
;

print_statement: PRINT names{
	if(lookup(st, *$2, "table", NULL)==NULL)
		symerror("Unknown table "+*$2);
	if(*$2=="READS" || *$2=="Reads")
		symerror("Cannot print the entire reads table. Choose a subset of it");
	else emit(emit_lst, "print: "+*$2);
}
|PRINT names BOTH_MATES{
	st_node* nd=lookup(st, *$2, "table", NULL);
	if(nd==NULL)
		symerror("Unknown table "+*$2);
	if(*$2=="READS" || *$2=="Reads")
		symerror("Cannot print the entire reads table. Choose a subset of it");
	else emit(emit_lst, "print: "+*$2+" both_mates");
}
;

%%
extern FILE *yyin;

int main(){
	//yydebug=1;

	yyparse();
	if(errorNo==0){
		cout << "Compilation completed without errors" << endl;
		cerr << "Compilation completed without errors" << endl;
		output_emit_list(emit_lst);
		//writeToFile(target_file);//the intermediate code will be printed at argv[1].interm
	}
	else{
		cout << "There were " << errorNo << "errors" << endl;
		cerr << "There were " << errorNo << "errors" << endl;
	}



	return 0;
}

int yyerror(char* msg)
{ 

	cout << "line "<< lineNo << "syntax Error... contents: " << *yylval.str << endl;

	///exit(2);
	errorNo++;
	if(errorNo>5){
		printf("Too many errors to continue\n");
		exit(1);
	}
	
	//return 1;
	
}


int symerror(string msg){
	cout << "line " << lineNo <<": "<< msg << endl;
	exit(2);
	errorNo++;
	if(errorNo>5){
		cout << "Too many errors to continue" << endl;
		exit(1);
	}
	
	//return 1;
	
}

void ioerror(string msg){
	cout << "FATAL ERROR!!! " << msg << endl;
	exit(2);
}


/*if there is no conflict with the contents of st, it adds to it a node with
fields the input arguments name, kind etc*/
void check_and_add(map<string, list<st_node> >& st, string name, string kind, string type, string place, st_node* owner){
	map<string, list<st_node> >::iterator it=st.find(name);
	if (it!=st.end()){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++)
			if(vit->name==name){
				if(vit->kind=="attribute" && vit->owner==owner){
					symerror("Attribute "+name+" already exists for table "+vit->owner->name);
					return;
				}
				else if(vit->name==name && vit->kind=="table" && vit->owner==owner){
					symerror("Table "+name+" already exists. ");
					return;
				}
			}
	}

	st_node nnode;
	nnode.name=name;
	nnode.type=type;
	nnode.kind=kind;
	nnode.place=place;
	nnode.owner=owner;
	st[name].push_back(nnode);
}

/*It returns the entry that matches with all non empty name/kind/owner fields,
NULL otherwise*/
st_node* lookup(map<string, list<st_node> >& st, string name, string kind, st_node* owner){
	map<string, list<st_node> >::iterator it=st.find(name);
	if (it!=st.end()){
		list<st_node>::iterator vit = it->second.begin();
		int i=0;
		for(;vit!=it->second.end();vit++){
			if(name.empty() || vit->name==name)
				if(kind.empty() || vit->kind==kind)
					if(owner==NULL || vit->owner==owner){
						return &(*vit);
					}
			i++;
		}
			
	}
	return NULL;
}


/*in case there are no coflicts between the given table_name and st, it copies
all entries whose owner is the table with the given table_name to tmp_st. */
void update_active_scope(map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, string& table_name){
	st_node *parent=lookup(st, table_name, "table", NULL);
	check_and_add(tmp_st, table_name, "table", "table", "table", NULL);
	if(parent==NULL) symerror("Unknown table: "+table_name);
	map<string, list<st_node> >::iterator it=st.begin();
	for(;it!=st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++)
			if(vit->owner==parent){
				tmp_st[vit->name].push_back(*vit);
			}
	}
}

/*It disambiguates conflicting attributes of the same owner by appending
subscripts 1 and 2 (e.g. arg1 and arg2 for arg). The assumption is that all 
lists of st with more than one elements contain ambiguous elements*/
void disambiguate(map<string, list<st_node> >&st/*, st_node* owner*/){
	map<string, list<st_node> >::iterator it=tmp_st.begin();
	for(;it!=tmp_st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		if(it->second.size()==1) 
			continue;
		int cnt=1;
		for(;vit!=it->second.end();vit++){
			//if(vit->owner!=owner) symerror("Bad formated st map");
			st_node nnode=*vit;
			stringstream ss;
			ss << vit->name << cnt++;
			ss >> nnode.name;
			st[nnode.name].push_back(nnode);
		}
	}
}



/*It checks whether the contents of select_lst appear in tmp_st and copies them
to st. If select_lst contains * it copies everything from tmp_st*/
void copy_to_symbol_table(vector<string>& select_lst, map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, st_node *cur_owner){
	if(select_lst.size()==2 && select_lst[0]=="st" && select_lst[1]=="nd"){
		st_node *nnode=create_node("st", "int", "attribute", cur_owner);
		nnode->place="st";
		if(cur_owner != NULL ) nnode->place=cur_owner->name+":"+nnode->place;
		st["st"].push_back(*nnode);
		nnode=create_node("nd", "int", "attribute", cur_owner);
		nnode->place="nd";
		if(cur_owner != NULL ) nnode->place=cur_owner->name+":"+nnode->place;
		st["nd"].push_back(*nnode);
	}
	map<string, list<st_node> >::iterator it=tmp_st.begin();
	for(;it!=tmp_st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++){
			if(select_lst[0]=="*" || search_vector<string>(select_lst, vit->name)>0){
				st_node nnode=*vit;
				nnode.owner=cur_owner;
				if(cur_owner != NULL ) nnode.place=cur_owner->name+":"+nnode.place;
				st[vit->name].push_back(nnode);
			}
		}
	}
}

/*it creates a node with the given fields*/
st_node* create_node(string name, string type, string kind, st_node* owner){
	st_node *nnode=new st_node();
	nnode->name=name;
	nnode->type=type;
	nnode->kind=kind;
	nnode->owner=owner;
	return nnode;
}

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int + float gives float*/
string arith_type_compatibility(string type1, string type2){
	if(type1=="int" && type2=="int") return "int";
	if(type1=="float" || type2=="float") return "float";
	if(type1=="string" && type2=="string") return "string";
	if(type1=="char" && type2=="char") return "char";
	
	symerror("Illegal arithmetic operation between "+type1+" and "+type2);
}

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int < float gives float*/
string cmp_type_compatibility(string type1, string type2){
	if(type1=="int" && type2=="int") return "int";
	if(type1=="float" || type2=="float") return "int";
	if(type1=="string" && type2=="string") return "int";
	if(type1=="char" && type2=="char") return "int";
	
	symerror("Illegal arithmetic operation between "+type1+" and "+type2);
}

/*It returns the position of the element, -1 if failure*/
template<class D>
int search_vector(vector<D>& lst, D element){
	int i=0;
	for(i=0;i<lst.size();i++)
		if(lst[i]==element) return i;
	return -1;
}

