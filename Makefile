CC := gcc

# TODO MAKE PORTABLE
CFLAGS := -Wall -Wpedantic -I/opt/homebrew/include
LDFLAGS := -L/opt/homebrew/lib
RAYLIB := -lraylib

all: game.out

game.out: main.o 
	$(CC) $(CFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(RAYLIB)

clean:
	rm -rf *.o *.out
