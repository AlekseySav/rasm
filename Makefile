include stddefs.mk

C_INC 	= include

SOURCE	= init/main.o \
	src/src.o \
	lib/lib.a

all: rasm/rasm

rasm/rasm: $(SOURCE)
	$(CC) $^ -o $@

clean:
	(cd init; rm -f *.o)
	(cd lib; make clean)
	(cd src; make clean)

init/main.o: init/main.c

lib/lib.a:
	(cd lib; make)

src/src.o:
	(cd src; make)
