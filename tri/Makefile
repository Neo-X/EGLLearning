CFLAGS=-O3

PLATFORM := $(shell uname -a | cut -d ' ' -f 1)
ifeq ($(PLATFORM),Darwin)
LIBS=-framework OpenGL -framework GLUT -lm
else
CFLAGS+= -DHAVE_BUILTIN_SINCOS -ggdb
### Using nvidia OpenGL lib is extreamly important!
LIBS= -lm -lglut -lEGL -lX11 -L/usr/lib/nvidia-396 -L/usr/lib/nvidia-390 -lOpenGL
# TODO: Handle Windows as well
endif

all: native

native: es2tri.c
	g++ -o headless_demo headless_es2tri.c $(CFLAGS) $(LIBS) 
	g++ -o headless_demo_particles headless_es2Particles.c $(CFLAGS) $(LIBS) 

clean: demo*
	rm demo demo.html 2>/dev/null
