# makefile for qbot
 
CC = gcc -ansi -pedantic -Wall -Wno-implicit
 
CFLAGS=-g
#CFLAGS=-O3 -fomit-frame-pointer -funroll-loops
#CFLAGS=-O3 -funroll-loops -pg
 
LFLAGS=-g
#LFLAGS=
#LFLAGS=-pg
 
PDEFS= #-DDEBUG
 
LEX=flex
LEXLIBS=-lfl
#LEX=lex
#LEXLIBS-ll
 
MAKE = make
RM = rm

RANLIB = ranlib
#RANLIB = /bin/true
 
DMALLOC=
#DMALLOC= ENABLEDMALLOC=-DENABLE_DMALLOC DMALLOCLIBS=/usr3/jsanford/software/t/malloc/dmalloc-3.1.3/libdmalloc.a"
 
# -
 
SYSDEFS=CC="${CC}" CFLAGS="${CFLAGS}" LFLAGS="${LFLAGS}" PDEFS="${PDFES}" LEX="${LEX}" LEXLIBS="${LEXLIBS}" RANLIB="${RANLIB}" MAKE="${MAKE}" RM="${RM}" ${DMALLOC}
 
all: library filter qbot

OBJS=qbot.o

LIBS=libqbot/libqbot.a -lnsl -lsocket

FINAL=qbot

.c.o:
	$(CC) -c $(CFLAGS) $(PDEFS) $(ENABLEDMALLOC) $<

$(FINAL): $(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) -o $(FINAL) $(OBJS) $(LIBS) $(DMALLOCLIBS)

library:
	cd libqbot && ${MAKE} library ${SYSDEFS}

filter:
	cd filters && $(MAKE) filter ${SYSDEFS}

clean:
	rm -f $(FINAL) $(OBJS)
	cd filters && ${MAKE} clean
	cd libqbot && ${MAKE} clean


