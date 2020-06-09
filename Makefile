CFLAGS = -O2 -DNDEBUG
#CFLAGS = -pg -O5
#CFLAGS = -g -Wall -W
CPP = g++
LOBJS = learn.cpp crf.o crfpos.o lookahead.o
MOBJS = main.cpp lookahead.o tokenize.o crf.o crfpos.o dyad.a

all: lapos-learn lapos lapos-eval

lapos-learn: $(LOBJS)
	$(CPP) -o lapos-learn $(CFLAGS) $(LOBJS)
lapos: $(MOBJS)
	$(CPP) -o lapos_server $(CFLAGS) $(MOBJS)
lapos-eval: eval.cpp
	$(CPP) -o lapos-eval $(CFLAGS) eval.cpp
clean:
	/bin/rm -r -f $(OBJS) a.out tmp lapos_server lapos-learn lapos-eval *.o *~ *.a
.cpp.o:
	$(CPP) -c $(CFLAGS) $<

dyad.a: httpserv.c dyad.c httpserv.h
	gcc -c httpserv.c dyad.c
	rm -f dyad.a
	ar qv dyad.a httpserv.o dyad.o

