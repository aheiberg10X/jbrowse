%{
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"biosql_defs.h"
#define MAX_FROM_TABLES 2048

extern int lineNo;
extern int errorNo;
st_node *cur_owner=NULL;
st_node *lst=NULL; //the symbol table
st_node **from_lst=NULL;//(st_node**)malloc(sizeof(st_node*)*(MAX_FROM_TABLES+1));
int len_from_lst=0;
st_node *select_lst=NULL;
st_node *where_lst=NULL;

st_node *emit_lst=NULL;
int nextstat=1; //keep count of the lines of 


%}

%union{
	int int_val;
	char *string;
	struct st_node *entry;
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
%token  INTERSECT

%type <string> names
%type <entry> table_keyword
%type <entry> table_arg
%type <entry> from_arg
%type <entry> lvalue
%type <entry> rvalue
%type <string> comparison_op
%type <string> lowest_expr
%type <string> where_args
%type <entry> arith_expr
%type <string> arith_op

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
program: table_prototypes assigned_selects select_statement
;

table_prototypes: table_prototype table_prototypes
|
;

table_prototype: table_keyword LPAREN table_args RPAREN SEMICOLON{cur_owner=NULL;
	if(strcmp($1->name, "READS")!=0){
		emit(&emit_lst, "end_load_table", NULL, NULL, NULL,NULL);
	}
}
;

table_keyword: TABLE names{
	if(lookup(lst, $2, NULL, NULL, NULL)==NULL){
		$$=create_node($2, "table", "table", NULL);
		cur_owner=$$;
		add_node(&lst, $$);
		if(strcmp($2, "READS")!=0){
			emit(&emit_lst, "load_table", $$->name, NULL, NULL,NULL);
		}
	}
	else symerror("Conflict with name tables");
}
	
;

names: ID{
	$$=(char*)malloc(strlen(yylval.string)+3);
	strcpy($$, yylval.string);
}
;

table_args: table_args COMMA table_arg {
//arguments need to appear in output in the order that the user enders them.

	//char *tmp_str=malloc(1024);
	if(strcmp(cur_owner->name, "READS")!=0){
		if(strcmp($3->type,"integer")==0)
			//sprintf(tmp_str, "loaded_param int %s",$3->name);
			emit(&emit_lst, "loaded_param", "int", $3->name, NULL,NULL);
		else if(strcmp($3->type,"string")==0)
			//sprintf(tmp_str, "loaded_param char* %s",$3->name);
			emit(&emit_lst, "loaded_param", "char*", $3->name, NULL,NULL);
		else
			//sprintf(tmp_str, "loaded_param %s %s",$3->type, $3->name);
			emit(&emit_lst, "loaded_param", $3->type, $3->name, NULL,NULL);
		//squeeze_node(emit_lst, "loaded_param", tmp_str);
	}
	//free(tmp_str);
}

|table_arg{
	if(strcmp(cur_owner->name, "READS")!=0){
		if(strcmp($1->type,"integer")==0)
			emit(&emit_lst, "loaded_param", "int", $1->name, NULL,NULL);
		else if(strcmp($1->type,"string")==0)
			emit(&emit_lst, "loaded_param", "char*", $1->name, NULL,NULL);
		else
			emit(&emit_lst, "loaded_param", $1->type, $1->name, NULL,NULL);
	}
}
;

table_arg: INTEGER names{
	$$=check_and_create(lst, $2, "integer", "attribute", cur_owner);
	if($$!=NULL) add_node(&lst, $$);
	else symerror("Conflicting attribute names");
	}
|FLOAT names{
	$$=check_and_create(lst, $2, "float", "attribute", cur_owner);
	if($$!=NULL) add_node(&lst, $$);
	else symerror("Conflicting attribute names");
	}
|CHAR names{
	$$=check_and_create(lst, $2, "char", "attribute", cur_owner);
	if($$!=NULL) add_node(&lst, $$);
	else symerror("Conflicting attribute names");
	}
|STRING_TYPE names{
	$$=check_and_create(lst, $2, "string", "attribute", cur_owner);
	if($$!=NULL) add_node(&lst, $$);
	else symerror("Conflicting attribute names");
	}
		
;

assigned_selects: assigned_select assigned_selects
|
;

assigned_select: lvalue ASSIGN select_statement{emit(&emit_lst, "result_table", ":", $1->name, NULL,NULL); cur_owner=NULL;}
;

lvalue: names{
	$$=check_and_create(lst, $1, "table", "table", NULL);
	if($$==NULL) symerror("assigned table already exists");
	add_node(&lst, $$);
	cur_owner=$$;
}
;

select_statement: SELECT select_args FROM from_arg WHERE where_args{
	if(1/*cur_owner != NULL*/){ //indication of assigned select...need to update the symbol table.
		if(select_lst->name[0]=='*'){
			replicate_args(from_lst, len_from_lst, lst, cur_owner); //All properties of the tables in from_args are replicated under a new owner.
			emit(&emit_lst, "return_arg",":", "*",NULL, NULL);
		}
		else if(strcmp(select_lst->name, "countvec")==0){
			add_node(&lst, create_node("countvec", "integer", "attribute", cur_owner));
			emit(&emit_lst, "return_arg",":","strength_vector",NULL, NULL);
		}
		else{
			check_and_copy_from_mult(&lst, select_lst, from_lst, len_from_lst, cur_owner);
			st_node *tmp;
			for(tmp=select_lst;tmp!=NULL;tmp=tmp->next) emit(&emit_lst, "return_arg",":",tmp->name,NULL, NULL);
		}
		
	}
	len_from_lst=0;
	where_lst=NULL;
	destroy_list(select_lst);
	select_lst=NULL;

}
| SELECT select_args FROM INTERSECT from_args{
	
	squeeze_node(emit_lst, "Input", "begin_intersect"); //add a flag to help with code generation

	emit(&emit_lst, "end_intersect",NULL, NULL,NULL, NULL);
	if(select_lst->name[0]=='*'){
		replicate_args(from_lst, len_from_lst, lst, cur_owner); //All properties of the tables in from_args are replicated under a new owner.
		///////////////emit(&emit_lst, "return_arg",":", "*",NULL, NULL);
	}
	else if(strcmp(select_lst->name, "countvec")==0){
		add_node(&lst, create_node("countvec", "integer", "attribute", cur_owner));
		emit(&emit_lst, "return_arg",":","strength_vector",NULL, NULL);
	}
	else{
		check_and_copy_from_mult(&lst, select_lst, from_lst, len_from_lst, cur_owner);
		st_node *tmp;
		for(tmp=select_lst;tmp!=NULL;tmp=tmp->next) emit(&emit_lst, "return_arg",":",tmp->name,NULL, NULL);
	}

	len_from_lst=0;
	where_lst=NULL;
	destroy_list(select_lst);
	select_lst=NULL;

}
;

select_args: STAR{
	add_node(&select_lst, create_node("*",NULL, NULL, NULL));

}
| COUNT{
	add_node(&select_lst, create_node("countvec", NULL, NULL, NULL));
}
| select_arg_series

select_arg_series: names COMMA select_arg_series{
	//printf("@@adding %s\n",$1);
	add_node(&select_lst, create_node($1, NULL, NULL, NULL)); //remembers which names have been encountered by select
}
| names{
	//printf("@@adding %s\n",$1);
	add_node(&select_lst, create_node($1, NULL, NULL, NULL)); //remembers which names have been encountered by select
}

;

from_args: from_arg COMMA from_args
| from_arg
;

from_arg: names{
	$$=lookup(lst, $1, "table", "table", NULL);
	//st_node *tmp;
	//for (tmp=lst;tmp!=NULL;tmp=tmp->next) printf("name: %s type: %s kind: %s owner: %x\n",tmp->name, tmp->type, tmp->kind, tmp->owner);
	if($$==NULL) symerror("Unknown table in the FROM statement");
	add_node_to_array(from_lst, &len_from_lst, $$);
	emit(&emit_lst, "Input","=",$$->name,NULL, NULL);
}
;

where_args: where_args COMMA where_args { //the type of where_args is str and denotes the respective temporary variable
	$$=get_newtemp();
	emit(&emit_lst, $$,"=",$1, "&&", $3);
}
|where_args AND where_args{
$$=get_newtemp();
	emit(&emit_lst, $$,"=",$1, "&&", $3);
}
|where_args OR where_args{
	$$=get_newtemp();
	emit(&emit_lst, $$,"=",$1, "||", $3);
}
|NOT where_args{
	$$=get_newtemp();
	emit(&emit_lst, $$,"=","!",$2, NULL);
}
|LPAREN where_args RPAREN{
	$$=strdup($2);
	//emit(&emit_lst, $$,"=",$1.place, NULL, NULL);
}
|lowest_expr{
	$$=strdup($1);
}
;


lowest_expr: arith_expr comparison_op rvalue{

	if (strcmp($1->type, $3->type)!=0) symerror("Incompatible types in a where expression");

	char *tmp_str1=(char*)malloc(1024);
	char *tmp_str2=(char*)malloc(1024);
	
	if (strcmp($1->type, "string")!=0){
		sprintf(tmp_str1,"%s %s %s",$1->place,$2,$3->name);
		sprintf(tmp_str2,"%d",nextstat+3);
	}
	else{
		if(strcmp($2,"==")==0)
			sprintf(tmp_str1, "(strcmp( %s , %s )==0)", $1->place, $3->name);
		else if(strcmp($2,"!=")==0)
			sprintf(tmp_str1, "(strcmp( %s , %s )!=0)", $1->place, $3->name);
		else if($2[0]=='>')
			sprintf(tmp_str1, "(strcmp( %s , %s )>0)", $1->place, $3->name);
		else 
			sprintf(tmp_str1, "(strcmp( %s , %s )<0)", $1->place, $3->name);
	}


	

	$$=get_newtemp();
	/*emit(&emit_lst, "if", tmp_str1, "goto",tmp_str2,NULL);//original impl
	emit(&emit_lst, $$,"=","0",NULL, NULL);*/
	emit(&emit_lst, $$, "=", tmp_str1,NULL, NULL);
	/*sprintf(tmp_str2,"%d",nextstat+2);
	emit(&emit_lst, "goto", tmp_str2,NULL, NULL, NULL);
	emit(&emit_lst, $$,"=","1",NULL,NULL);*/

	free(tmp_str1);
	free(tmp_str2);

}
;

arith_expr: arith_expr arith_op arith_expr{
	if(strcmp($1->type,"integer")!=0 || strcmp($3->type,"integer")!=0) symerror("Only integers are allowed in arithmetic operations");
	$$=create_node($1->name, $1->type, $1->kind, $1->owner);
	$$->place=get_newtemp();
	emit(&emit_lst, $$->place, "=", $1->place, $2, $3->place);
}	

| arith_op arith_expr{
	if(strcmp($2->type,"integer")!=0) symerror("Only integers are allowed in arithmetic operations");
	$$=create_node($2->name, $2->type, $2->kind, $2->owner);
	$$->place=get_newtemp();
	emit(&emit_lst, $$->place, "=", $1, $2->place, NULL);
}
| names	{
	st_node *tmp=lookup_mult_tables(from_lst, len_from_lst, lst, $1);
	if (tmp==NULL){
		printf("name: %s\n",$1);
		symerror("Unknown attribute in where expressions\n");
	}
	if(tmp->kind==NULL || strcmp(tmp->kind, "attribute")!=0) symerror("Illegal non attribute in an arithmetic op");
	$$=create_node(tmp->name, tmp->type, tmp->kind, tmp->owner);
	$$->place=strdup(tmp->name);
	//printf("tmp: %s type %s $3 %s type %s\n",tmp->name, tmp->type, $3->name, $3->type);

}

| NUMBER {
	$$=create_node(yylval.string, "integer", NULL, NULL);
	$$->place=strdup($$->name);

}
;

comparison_op: EQUAL{$$=strdup("==");}
			|GREATER_EQUAL{$$=strdup(">=");}
			|LESS_EQUAL {$$=strdup("<=");}
			|GREATER {$$=strdup(">");}
			|LESS {$$=strdup("<");}
;

arith_op: PLUS {$$=strdup("+");}
		|MINUS{$$=strdup("-");}
		|MUL{$$=strdup("x");}
		|DIV{$$=strdup("/");}
		|MOD{$$=strdup("%");}

rvalue: CONST_CHAR{
	$$=create_node(yylval.string, "char", NULL, NULL);
}
| CONST_STRING {
	$$=create_node(yylval.string, "string", NULL, NULL);
}
| NUMBER{
	$$=create_node(yylval.string, "integer", NULL, NULL);
}
;

%%
extern FILE *yyin;

int main(){
	//yydebug=1;
	yylval.entry = (st_node*) malloc(sizeof(st_node));
	yylval.entry->next=NULL;

	from_lst=(st_node**)malloc(sizeof(st_node*)*(MAX_FROM_TABLES+1));

	yyparse();
	if(errorNo==0){
		printf("Compilation completed without errors\n");
		fprintf(stderr,"Compilation completed without errors\n");
		output_emit_list(emit_lst);
		//writeToFile(target_file);//the intermediate code will be printed at argv[1].interm
	}
	else{
		printf("There were %d errors\n",errorNo);
		fprintf(stderr,"There were %d errors\n",errorNo);
	}

	/*st_node *tmp=lst;
	for(tmp=lst;tmp!=NULL;tmp=tmp->next){
		printf("%s %s %s",tmp->name, tmp->type, tmp->kind);
		if (tmp->owner!=NULL) printf(" %s\n",tmp->owner->name);
		else printf("\n");
	}*/
	

	return 0;
}

int yyerror()
{ 

	printf("\nline %d: syntax Error",lineNo);
	///exit(2);
	errorNo++;
	if(errorNo>5){
		printf("\nToo many errors to continue");
		exit(1);
	}
	
	//return 1;
	
}

int symerror(char *msg){
	printf("\nline %d: Symbol Error: %s",lineNo, msg);
	exit(2);
	errorNo++;
	if(errorNo>5){
		printf("\nToo many errors to continue");
		exit(1);
	}
	
	//return 1;
	
}

void ioerror(char *msg){
	fprintf(stderr, "FATAL ERROR!!! %s",msg);
	exit(2);
}

/*lst is the head of a list of st_node. The function
returns the node whose name, type, kind and owner match the input. If any of
type, kind, owner is NULL it is considered as wildcard.
The function returns NULL on failure*/
st_node *lookup(st_node *lst, char *name, char *type, char *kind, st_node *owner){
	st_node *ret;
	//printf("name: %s, lst: %x\n",name, lst);
	if(lst==NULL) return NULL;

	for (ret=lst;ret!=NULL;ret=ret->next){
		if(strcmp(name, ret->name)==0){
			if(type==NULL || strcmp(type, ret->type)==0){
				if(kind==NULL || strcmp(kind, ret->kind)==0){
					if(owner==NULL || owner==ret->owner){
						return ret;
					}
				}
			}
		}
	}
	return NULL;
}

//it creates a node whose properties are initialized from the input args
st_node *create_node(char *name, char *type, char *kind, st_node *owner){
	st_node *tmp=(st_node*)malloc(sizeof(st_node));
	if(name!=NULL) tmp->name=strdup(name); else tmp->name=NULL;
	if(type!=NULL) tmp->type=strdup(type); else tmp->type=NULL;
	if(kind!=NULL) tmp->kind=strdup(kind); else tmp->kind=NULL;
	tmp->owner=owner;
	tmp->next=NULL;
	tmp->place=NULL;
	return tmp;
}



/*lst is the head of a list of st_node. The function creates a node
whose data are taken from the input and adds it at the beginning of the list*/
//void add_node(st_node *lst, char *name, char *type, char *kind, st_node *owner){
/*lst is the head of a list of st_node in which newnode is going to be added
at the beginning of the list*/
void add_node(st_node **lst, st_node *newnode){
	if ((*lst)==NULL) *lst=newnode;
	else{
		newnode->next=*lst;
		*lst=newnode;
	}
}

//It creates a node with name node_info and adds it to the list at the place that precedes
//where_str. For example if the list consists of A->B->C and where_str=B, the new node
//is going to be placed between B and C. Now if there are multiple B's such as A->B->B-C
//the new node is going to be inserted between B and C
void squeeze_node(st_node *lst, char *where_str, char *node_info){
	st_node *tmp=lst;
	st_node *new_tmp_nxt;
	for(tmp=lst;tmp!=NULL;tmp=tmp->next){
		if(tmp->next==NULL) break;
		if(strstr(tmp->name, where_str)!=NULL && strstr(tmp->next->name, where_str)==NULL)
			break;
	}
	new_tmp_nxt=tmp->next;
	add_node(&new_tmp_nxt, create_node(node_info, NULL, NULL, NULL));
	tmp->next=new_tmp_nxt;
}

//It returns that node of lst that has been entered prior to list_node. In this implementation
//the answer is lst_node->next, but it can change if the list implementation changes.
st_node *get_previous_node(st_node *lst_node){
	if(lst_node==NULL) return NULL;
	else return lst_node->next;
}

//It adds newnode to the array list lst of length len_lst. The basic operation is
//lst[n++]=newnode
void add_node_to_array(st_node **lst, int *len_lst, st_node *newnode){
	int n=*len_lst;
	if(n>=MAX_FROM_TABLES) symerror("Cannot handle that many tables in the from statement\n");
	lst[n++]=newnode;
	*len_lst=n;
}


void destroy_list(st_node *lst){
	st_node *tmp, *vic;
	tmp=lst;
	while(tmp!=NULL){
		vic=tmp;
		tmp=tmp->next;
		free(vic->name);
		vic->name=NULL;
		free(vic->type);
		vic->type=NULL;
		free(vic->kind);
		vic->kind=NULL;
		free(vic);
		vic=NULL;
	}
	return;
}

//If lst does not contain already an attribute with the same name and the same
//owner, it creates one. Otherwise it returns NULL
st_node *check_and_create(st_node *lst, char *name, char *type, char *kind, st_node *cur_owner){
	st_node *ret=NULL;
	if (lookup(lst, name, type, "attribute", cur_owner)==NULL)
		ret=create_node(name, type, kind, cur_owner);
	return ret;
}

//It returns that node of lst where the name matches and the owner is one
//of the tables of from_lst. Both lists contain pointers to the same 
//objects
st_node *lookup_mult_tables(st_node **from_lst, int len_from_lst, st_node *lst, char *name){
	st_node *tmp;
	st_node *hit=NULL;
	int i=0;
	//printf("!!!!!!!looking up name: %s tblist: %x\n",name, tbl_lst);
	for(i=0;i<MAX_FROM_TABLES;i++){
		tmp=from_lst[i];
		//printf("tbl name: %s tmp: %x\n",tmp->name,tmp);
		//for(hit=lst;hit!=NULL;hit=hit->next) if(strcmp("arg21", name)==0)printf(">>>name: %s kind: %s owner: %x\n",hit->name, hit->kind, hit->owner);
		hit=lookup(lst, name, NULL, "attribute", tmp);
		if(hit!=NULL){
			//printf("actual hit: %x\n",hit);
			return hit;
		}
	}
	//printf("returning NULL\n");
	return NULL;
}

//it checks whether each of the nodes of arg_lst is a property of any of the 
//tables in the "from_args" and adds them in lst with owner as specified
//by the input
void check_and_copy_from_mult(st_node **lst, st_node *arg_lst, st_node **from_lst, int len_from_lst, st_node *owner){
	st_node *tmp;
	st_node *lkup;
	//printf("tmp: %s\n",arg_lst->name);
	for (tmp=arg_lst;tmp!=NULL;tmp=tmp->next){
		lkup=lookup_mult_tables(from_lst, len_from_lst, *lst, tmp->name);
		if(lkup==NULL) symerror("An attribute in select is not contained in the from tables\n");
		//printf(">>>adding %s %s\n",lkup->name, lkup->type);
		add_node(lst, create_node(lkup->name, lkup->type, lkup->kind, owner));
	}
}



//It replicates all properties of lst whose owner is a table in the from_args
//and cur_owner is the owner of the new attributes.
void replicate_args(st_node **from_lst, int len_from_lst, st_node *lst, st_node *cur_owner){
	st_node *tmp;
	int i=0;
	for (tmp=lst;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->kind, "attribute")==0 && (tmp->owner!=NULL)){
			for(i=0;i<len_from_lst;i++){
				if(strcmp(from_lst[i]->name, tmp->owner->name)==0 && strcmp(from_lst[i]->kind, "table")==0){
					/////if(lookup(from_lst, tmp->owner->name, "table", "table", NULL)!=NULL){ //check if the owner table of tmp is in from_lst
					add_node(&lst, create_node(tmp->name, tmp->type, tmp->kind, cur_owner));
					break;
				}
			}
		}
	}
	return;
}

//it adds to em_lst an st_node whose name is the concatenation of str1...4 
//and also increases nextstat.
void emit(st_node **em_lst, char *str1, char *str2, char *str3, char *str4, char *str5){
	char *str=(char*)malloc(2048);
	int cnt=0;
	*str='\0';
	if(str1!=NULL){
		strcat(str, str1);
		strcat(str," ");
		cnt+=strlen(str1);
	}
	if(str2!=NULL){
		strcat(str, str2);
		strcat(str," ");
		cnt+=strlen(str2);
	}
	if(str3!=NULL){
		strcat(str, str3);
		strcat(str," ");
		cnt+=strlen(str3);
	}
	if(str4!=NULL){
		strcat(str, str4);
		strcat(str," ");
		cnt+=strlen(str4);
	}
	if(str5!=NULL){
		strcat(str, str5);
		strcat(str," ");
		cnt+=strlen(str5);
	}
	if(cnt>=2048){ioerror("Too many characters appended to emit");}
	add_node(em_lst, create_node(str, NULL, NULL, NULL));
	nextstat++;
}

//it traverses lst backwards and outputs the name of each node.
//Remember: nodes have been added to the top of the list each time.
//So the list needs to be tranversed in a reverse direction.
void output_emit_list(st_node *lst){
	static int i=1;
	if (lst==NULL) return;
	output_emit_list(lst->next);
	printf("%d. %s\n", i++, lst->name);
}


//it generates a new temporary variable
char *get_newtemp(){
	static int cnt=0;
	char *ret=(char *)malloc(sizeof(char)*(50));
	sprintf(ret, "t%d",cnt++);
	return ret;
}
