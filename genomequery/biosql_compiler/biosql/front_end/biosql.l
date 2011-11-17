%{
	#include<math.h>
	#include<malloc.h>
	#include<string.h>
	#include "biosql.h"
	
	int lineNo;
	int errorNo;
	int errors=0;
	int line=1;
%}

dot		\.
digit	[0-9]
number	{digit}+
constChar	"'"([^"\"""\'""\\"])"'"
constString	"\""([^"\"""\'""\\"\n])*"\""
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
use		use
mates	mates|MATES
name		([a-z]|[A-Z])+([0-9]|_|[a-z]|[A-Z])*
comment #.*\n

%%
{comment} {lineNo=line++; /*printf("comment line: %s\n",yytext);*/}

{dot}	{
	lineNo=line;
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return DOT;
}
{mates}	{
	lineNo=line;
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return MATES;

}
{constChar}	{
	lineNo=line;
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return CONST_CHAR;
	//printf("constChar: %s\n",yytext);
}

{constString}	{
	lineNo=line;
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return CONST_STRING;

	//printf("constString: %s\n",yytext);
}

{number}	{
	lineNo=line;
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return NUMBER;

	//printf("number: %s\n",yytext);
}

{select}	{
	lineNo=line;//printf("select\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return SELECT;

}

{from}	{
	lineNo=line;//printf("where\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return FROM;

}
{mapjoin}	{
	lineNo=line;//printf("where\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return MAPJOIN;
}

{import} {
	lineNo=line;//printf("where\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return IMPORT;
}

{where}	{
	lineNo=line;//printf("where\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return WHERE;

}

{count}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return COUNT;

}

{table}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return TABLE;

}

{integer}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return INTEGER;

}

{float}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return FLOAT;

}

{char}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return CHAR;

}

{string_type}	{
	lineNo=line;//printf("count\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return STRING_TYPE;

}
{and}	{
	lineNo=line;//printf("and\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return AND;

}

{or}	{
	lineNo=line;//printf("or\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return OR;

}

{not}	{
	lineNo=line;//printf("or\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return NOT;

}

{use}	{
	lineNo=line;//printf("or\n");
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return USE;
}


"=="	{
	lineNo=line;//printf("equality\n");/*return TK_EQUAL*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return EQUAL;
}

"="		{lineNo=line;//printf("assignment\n");/*return TK_ASSIGN*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return ASSIGN;
	
}

">"		{
	lineNo=line;//printf("greater\n");/*return TK_GREATER*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return GREATER;

}

"<"		{
	lineNo=line;//printf("less\n");/*return TK_LESS*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return LESS;
}

"!="		{
	lineNo=line;//printf("neq\n");/*return TK_NOT_EQUAL*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return NOT_EQUAL;
	
}

"<="		{
	lineNo=line;//printf("leq\n");/*return TK_LESS_EQUAL*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return LESS_EQUAL;
}

">="		{
	lineNo=line;//printf("geq\n");/*return TK_GREATER_EQUAL*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return GREATER_EQUAL;
}

"*"		{
	lineNo=line;//printf("star\n");/*return TK_MUL*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return STAR;
}

"/"		{
	lineNo=line;//printf("div\n");/*return TK_DIV*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return DIV;
}

"%"		{
	lineNo=line;//printf("mod\n");/*return TK_MOD*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return MOD;
}

"+"		{
	lineNo=line;//printf("plus\n");/*return TK_PLUS*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return PLUS;
}

"-"		{
	lineNo=line;//printf("minus\n");/*return TK_MINUS*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return MINUS;
}

"("		{
	lineNo=line;//printf("lparen\n");/*return TK_LEFT_BRACKET*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return LPAREN;
}

")"		{
	lineNo=line;//printf("rparen\n");/*return TK_RIGHT_BRACKET*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return RPAREN;

}

","		{
	lineNo=line;//printf("comma\n");/*return TK_COMMA*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return COMMA;
	
}

";"		{
	lineNo=line;//printf("comma\n");/*return TK_COMMA*/
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return SEMICOLON;
	
}

{name}	{
	lineNo=line;//printf("name: %s\n",yytext);
	yylval.string=(char*)malloc(sizeof(yytext));
	strcpy(yylval.string,yytext);
	return ID;

}

[ \t\r]+	/*eat whitespace*/
[\n]		{line++;}

%%

//main(){yylex();}
