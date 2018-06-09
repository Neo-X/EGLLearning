CFLAGS=-O3

PLATFORM := $(shell uname -a | cut -d ' ' -f 1)
ifeq ($(PLATFORM),Darwin)
LIBS=-framework OpenGL -framework GLUT -lm
else
CFLAGS+= -DHAVE_BUILTIN_SINCOS 
LIBS=$(shell pkg-config --cflags --libs gl glesv2) -lm -lglut 
# TODO: Handle Windows as well
endif

all: native

native: es2gears.c
	gcc -o demo es2gears.c $(CFLAGS) $(LIBS) 

clean: demo*
	rm demo demo.html 2>/dev/null
