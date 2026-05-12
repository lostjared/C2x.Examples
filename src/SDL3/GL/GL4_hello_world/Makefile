

CFLAGS=-std=c23 -O2 -Wall
LIBS=`pkg-config --libs sdl3` -ldl
INCLUDES=`pkg-config --cflags sdl3` -I../include
OUTPUT=gl_init
all:
	$(CC) $(CFLAGS) $(INCLUDES) gl_init.c ../src/glad.c -o $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)

