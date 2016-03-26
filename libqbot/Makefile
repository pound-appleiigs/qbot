# library

RANLIB=true

# don't mess with this makefile, edit ../Makefile

.SUFFIXES: .c
.c.o:
	$(CC) -c $(CFLAGS) $(PDEFS) $(ENABLEDMALLOC) $<

all:
	cd .. && make library

library: libqbot.a qbtty

SRCS = generic.c io.c init.c nick.c word.c rehash.c
OBJS = generic.o io.o init.o nick.o word.o rehash.o

$(OBJS): libq.h

libqbot.a: $(OBJS)
	$(AR) cr $@ $(OBJS) && $(RANLIB) $@

QB_SRCS = qbtty.c
QB_OBJS = qbtty.o

qbtty: $(QB_OBJS) libqbot.a
	$(CC) $(LFLAGS) -o qbtty $(QB_OBJS) libqbot.a $(DMALLOCLIBS)

clean:
	rm -f $(OBJS) $(QB_OBJS) libqbot.a qbtty

