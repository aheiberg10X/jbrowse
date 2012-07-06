LEX=flex
LDFLAGS=-lfl -Wfatal-errors
CPP=g++

biosql: biosql.c lex.yy.c
	$(CPP) -g $^ $(LDFLAGS) -lm -o $@

biosql.c: biosql.y
	bison -t -v -o $@ -d $^

lex.yy.c: biosql.l
	$(LEX) -o $@ $^

clean:
	rm biosql biosql.output biosql.c biosql.h
