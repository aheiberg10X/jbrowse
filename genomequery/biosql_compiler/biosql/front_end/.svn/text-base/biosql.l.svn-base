%{
	#include<math.h>
	#include<malloc.h>
	#include<string>
	#include "biosql.h"
	#include <iostream>
	
	using namespace std;
	int lineNo;

	int errorNo;
	int errors=0;
	int line=1;
	int old_line;

	extern "C"{
		int yylex(void);
	}
%}

dot		\.
digit	[0-9]
number	{digit}+
constChar	"'"([^"\"""\'""\\"])"'"
constString	"\""([^"\"""\'""\\"\n])*"\""
both_mates	both_mates|BOTH_MATES
include	include<
print_cmd	print|PRINT
genome	genome|GENOME
select	select|SELECT
from	from|FROM
mapjoin mapjoin|MAPJOIN
where	where|WHERE
table	table|TABLE
count	count|COUNT
and		and|AND
or		or|OR
not		not|NOT
integer integer|INTEGER
float	float|FLOAT
char	char|CHAR
string_type string|STRING
import	import|IMPORT
use		use|USE
using	using|USING
interval_creation interval_creation|INTERVAL_CREATION
create_intervals	create_intervals|CREATE_INTERVALS
merge_intervals	merge_intervals|MERGE_INTERVALS
intervals intervals|INTERVALS
name		([a-z]|[A-Z])+([0-9]|_|[a-z]|[A-Z])*
comment #.*\n

%x incl

%%
{include}	{BEGIN(incl); old_line=line;}
<incl>[^\0>\n]+	{
	//cout << yytext << endl;
	//cout << YY_BUF_SIZE << endl;
	
	yyin = fopen( yytext, "r" );
	if(yyin==NULL){
		cout << "FILE NOT FOUND " << yytext <<endl;
		exit(2);
	}
	yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));	
	BEGIN(INITIAL);
	

}
<incl>>\n	{
	//cout << "ending include" << endl;
	line=old_line+1;
	BEGIN(INITIAL);
}
<<EOF>>	{
	yypop_buffer_state();
	if ( !YY_CURRENT_BUFFER )
	{
		//cout << "yyterminating" << endl;
		yyterminate();


	}
	BEGIN(incl);
	

}

{comment} {lineNo=line++; /*printf("comment line: %s\n",yytext);*/}

{dot}	{
	lineNo=line;
	
	yylval.str=new string(yytext);
	return DOT;
}
{both_mates}	{
	lineNo=line;
	
	yylval.str=new string(yytext);
	return BOTH_MATES;

}
{constChar}	{
	lineNo=line;
	
	yylval.str=new string(yytext);
	return CONST_CHAR;
	//printf("constChar: %s\n",yytext);
}

{constString}	{
	lineNo=line;
	
	yylval.str=new string(yytext);
	return CONST_STRING;

	//printf("constString: %s\n",yytext);
}

{number}	{
	lineNo=line;
	
	yylval.str=new string(yytext);
	return NUMBER;

	//printf("number: %s\n",yytext);
}
{print_cmd}	{
	lineNo=line;
	yylval.str=new string(yytext);
	return PRINT;
}

{genome} {
	lineNo=line;//printf("where\n");
	
	yylval.str=new string(yytext);
	return GENOME;
}

{select}	{
	lineNo=line;//printf("select\n");
	
	yylval.str=new string(yytext);
	return SELECT;

}

{from}	{
	lineNo=line;//printf("where\n");
	
	yylval.str=new string(yytext);
	return FROM;

}
{mapjoin}	{
	lineNo=line;//printf("where\n");
	
	yylval.str=new string(yytext);
	return MAPJOIN;
}

{import} {
	lineNo=line;//printf("where\n");
	
	yylval.str=new string(yytext);
	return IMPORT;
}



{where}	{
	lineNo=line;//printf("where\n");
	
	yylval.str=new string(yytext);
	return WHERE;

}

{count}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return COUNT;

}

{table}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return TABLE;

}

{integer}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return INTEGER;

}

{float}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return FLOAT;

}

{char}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return CHAR;

}

{string_type}	{
	lineNo=line;//printf("count\n");
	
	yylval.str=new string(yytext);
	return STRING_TYPE;

}
{and}	{
	lineNo=line;//printf("and\n");
	
	yylval.str=new string(yytext);
	return AND;

}

{or}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return OR;

}

{not}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return NOT;

}

{use}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return USE;
}

{using}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return USING;

}
{interval_creation}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return INTERVAL_CREATION;

}

{create_intervals}	{
	lineNo=line;//printf("or\n");
	yylval.str=new string(yytext);
	return CREATE_INTERVALS;
}

{merge_intervals}	{
	lineNo=line;//printf("or\n");
	yylval.str=new string(yytext);
	return MERGE_INTERVALS;
}


{intervals}	{
	lineNo=line;//printf("or\n");
	
	yylval.str=new string(yytext);
	return INTERVALS;

}

"=="	{
	lineNo=line;//printf("equality\n");/*return TK_EQUAL*/
	
	yylval.str=new string(yytext);
	return EQUAL;
}

"="		{lineNo=line;//printf("assignment\n");/*return TK_ASSIGN*/
	
	yylval.str=new string(yytext);
	return ASSIGN;
	
}

">"		{
	lineNo=line;//printf("greater\n");/*return TK_GREATER*/
	
	yylval.str=new string(yytext);
	return GREATER;

}

"<"		{
	lineNo=line;//printf("less\n");/*return TK_LESS*/
	
	yylval.str=new string(yytext);
	return LESS;
}

"!="		{
	lineNo=line;//printf("neq\n");/*return TK_NOT_EQUAL*/
	
	yylval.str=new string(yytext);
	return NOT_EQUAL;
	
}

"<="		{
	lineNo=line;//printf("leq\n");/*return TK_LESS_EQUAL*/
	
	yylval.str=new string(yytext);
	return LESS_EQUAL;
}

">="		{
	lineNo=line;//printf("geq\n");/*return TK_GREATER_EQUAL*/
	
	yylval.str=new string(yytext);
	return GREATER_EQUAL;
}

"*"		{
	lineNo=line;//printf("star\n");/*return TK_MUL*/
	
	yylval.str=new string(yytext);
	return STAR;
}

"/"		{
	lineNo=line;//printf("div\n");/*return TK_DIV*/
	
	yylval.str=new string(yytext);
	return DIV;
}

"%"		{
	lineNo=line;//printf("mod\n");/*return TK_MOD*/
	
	yylval.str=new string(yytext);
	return MOD;
}

"+"		{
	lineNo=line;//printf("plus\n");/*return TK_PLUS*/
	
	yylval.str=new string(yytext);
	return PLUS;
}

"-"		{
	lineNo=line;//printf("minus\n");/*return TK_MINUS*/
	
	yylval.str=new string(yytext);
	return MINUS;
}

"("		{
	lineNo=line;//printf("lparen\n");/*return TK_LEFT_BRACKET*/
	
	yylval.str=new string(yytext);
	return LPAREN;
}

")"		{
	lineNo=line;//printf("rparen\n");/*return TK_RIGHT_BRACKET*/
	
	yylval.str=new string(yytext);
	return RPAREN;

}

","		{
	lineNo=line;//printf("comma\n");/*return TK_COMMA*/
	
	yylval.str=new string(yytext);
	return COMMA;
	
}

";"		{
	lineNo=line;//printf("comma\n");/*return TK_COMMA*/
	
	yylval.str=new string(yytext);
	return SEMICOLON;
	
}

{name}	{
	lineNo=line;//printf("name: %s\n",yytext);
	
	yylval.str=new string(yytext);
	return ID;

}
[ \t\r]+	/*eat whitespace*/
[\n]		{line++;}

%%

//main(){yylex();}
