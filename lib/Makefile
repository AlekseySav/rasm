include ../stddefs.mk

C_INC 	= ../include

OBJECTS = buffer.o \
	error.o \
	flags.o \
	vector.o \

all: lib.a

lib.a: $(OBJECTS)
	$(AR) rsc $@ $^

clean:
	rm -f *.o
	rm -f *.a

buffer.o: buffer.c

error.o: error.c

flags.o: error.c

vector.o: vector.c
