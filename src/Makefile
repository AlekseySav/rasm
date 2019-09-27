include ../stddefs.mk

C_INC 	= ../include
LD		+= -r

OBJECTS = file.o \
	parse.o \
	preprocess.o \
	token.o

all: src.o

src.o: $(OBJECTS)
	$(LD) $^ -o $@

clean:
	rm -f *.o

file.o: file.c

parse.o: parse.c

preprocess.o: preprocess.c

token.o: token.c
