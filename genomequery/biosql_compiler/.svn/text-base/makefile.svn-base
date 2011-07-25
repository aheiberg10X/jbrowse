LEX=flex
LDFLAGS=-lfl

INCLUDES =  -I. -I /usr/include/sys

biosql: biosql.c lex.yy.c
	$(CC) $^ $(INCLUDES) $(LDFLAGS) -lm -o $@

biosql.c: biosql.y
	bison -t -v -o $@ -d $^

lex.yy.c: biosql.l
	$(LEX) -o $@ $^

clean: 
	rm biosql.c lex.yy.c biosql.h biosql