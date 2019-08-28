CPP		= g++
DEBUG	= -g
CFLAG	= -c -I include

OBJECTS = init/main.o \
	src/errno.o \
	src/file.o \
	src/flags.o \
	src/lex.o \
	src/preprosess.o \
	src/value.o

%.o: %.cpp
	$(CPP) $(DEBUG) $(CFLAG) -o $@ $<

all: rasm/rasm

rasm/rasm: $(OBJECTS)
	$(CPP) $(DEBUG) -o $@ $^

clean:
	(cd init; rm -f *.o)
	(cd src; rm -f *.o)

do_test:
	make
	make clean
	rasm/rasm test/test.s
	rm rasm/rasm

init/main.o: init/main.cpp

src/errno.o: src/errno.cpp

src/file.o: src/file.cpp

src/flags.o: src/flags.cpp

src/lex.o: src/lex.cpp

src/preprosess.o: src/preprosess.cpp

src/value.o: src/value.cpp
