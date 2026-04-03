CC=emcc
CFLAGS = -std=c23 -O2 -s USE_SDL=2 -s USE_SDL_TTF=2
PRELOAD = --preload-file data/ 
SOURCES = visual-graph2.c
OBJECTS = $(SOURCES:.c=.o)
OUTPUT = Graph.html

.PHONY: all clean install

all: $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTPUT) $(PRELOAD)  -s USE_SDL=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH -s ASSERTIONS -s ENVIRONMENT=web 
clean:
	rm -f *.o $(OUTPUT) *.wasm *.js *.data
