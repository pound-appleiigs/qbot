# umm, it would help if I made a rule for .x and .l

all:
	cd .. && make filter

clean:
	$(RM) biff chef drawl fudd lisp rapper zima

filter: biff chef drawl fudd lisp rapper zima

biff: biff.x
	$(LEX) biff.x
	$(CC) $(CFLAGS) -o biff lex.yy.c $(LEXLIBS)
	$(RM) -f lex.yy.c

chef: chef.x
	$(LEX) chef.x
	$(CC) $(CFLAGS) -o chef lex.yy.c $(LEXLIBS)
	$(RM) -f lex.yy.c

drawl: drawl.x
	$(LEX) drawl.x
	$(CC) $(CFLAGS) -o drawl lex.yy.c $(LEXLIBS)
	$(RM) -f lex.yy.c

fudd: fudd.x
	$(LEX) fudd.x
	$(CC) $(CFLAGS) -o fudd lex.yy.c $(LEXLIBS)
	$(RM) -f lex.yy.c

lisp: lisp.c
	$(CC) $(CFLAGS) -o lisp lisp.c

rapper: rapper.l
	$(LEX) rapper.l
	$(CC) $(CFLAGS) -o rapper lex.yy.c $(LEXLIBS)
	$(RM) -f lex.yy.c

zima: zima.c
	$(CC) $(CFLAGS) -o zima zima.c

