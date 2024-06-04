CC := gcc

# TODO MAKE PORTABLE
CFLAGS := -Wall -Wpedantic -g -I/opt/homebrew/include -fsanitize=address
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lraylib

OBJS = api.o map.o lib/cJSON/cJSON.o

all: game.out editortest.out game_api.dylib

include proto.mk

game.out: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

gameproto.out: $(PROTO_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

editortest.out: main.o game_api.dylib
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

game_api.dylib: $(OBJS)
	$(CC) $(CFLAGS) -dynamiclib $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o *.out .game_api* *.dylib
