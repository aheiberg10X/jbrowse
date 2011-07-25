LEX=flex
LDFLAGS=-lfl

biosql: biosql.c lex.yy.c
	$(CC) $^ $(LDFLAGS) -lm -o $@

biosql.c: biosql.y
	bison -t -v -o $@ -d $^

lex.yy.c: biosql.l
	$(LEX) -o $@ $^

clean:
	rm biosql biosql.output biosql.c biosql.h
