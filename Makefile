CC := gcc

# TODO MAKE PORTABLE
CFLAGS := -Wall -Wpedantic -g -I/opt/homebrew/include -fsanitize=address
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lraylib

all: game.out editortest.out gameproto

include proto.mk

game.out: main.o api.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

gameproto.out: $(PROTO_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

editortest.out: main.o game_api.dylib
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

game_api.dylib: editor_concept.o editor_concept_user_code.o
	$(CC) $(CFLAGS) -dynamiclib $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o *.out .game_api* *.dylib
