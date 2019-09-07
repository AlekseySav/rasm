include ../stddefs.mk

C_INC 	= ../include
LD		+= -r

OBJECTS = file.o

all: src.o

src.o: $(OBJECTS)
	$(LD) $^ -o $@

clean:
	rm -f *.o

file.o: file.c
