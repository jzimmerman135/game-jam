CC := gcc

# TODO MAKE PORTABLE
CFLAGS := -Wall -Wpedantic -I/opt/homebrew/include
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lraylib

include proto.mk

all: game.out

game.out: main.o api.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

proto.out: $(PROTO_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.out
