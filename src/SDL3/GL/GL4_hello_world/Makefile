

CFLAGS=-std=c23 -O2
LIBS=`pkg-config --libs sdl3` -lGL -ldl
INCLUDES=`pkg-config --cflags sdl3` -I../include
OUTPUT=gl_init
all:
	$(CC) gl_init.c ../src/glad.c -o $(OUTPUT) $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f $(OUTPUT)

