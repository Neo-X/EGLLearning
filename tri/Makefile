CFLAGS=-O3

PLATFORM := $(shell uname -a | cut -d ' ' -f 1)
ifeq ($(PLATFORM),Darwin)
LIBS=-framework OpenGL -framework GLUT -lm
else
CFLAGS+= -DHAVE_BUILTIN_SINCOS 
### Using nvidia OpenGL lib is extreamly important!
LIBS= -lm -lglut -lEGL -lX11 -L/usr/lib/nvidia-390 -lOpenGL
# TODO: Handle Windows as well
endif

all: native

native: es2tri.c
	g++ -o headless_demo headless_es2tri.c $(CFLAGS) $(LIBS) 

clean: demo*
	rm demo demo.html 2>/dev/null
