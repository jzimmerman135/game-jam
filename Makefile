CC := gcc

# TODO MAKE PORTABLE
CFLAGS := -Wall -Wpedantic -g3 -I/opt/homebrew/include -fsanitize=address
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lraylib

OBJS = api.o map.o lib/cJSON/cJSON.o powerups.o intro.o
OBJS+=palette.o
ASSETS=intro.txt.h prophecy.txt.h flapflap.txt.h

all: $(ASSETS) game.out game_api.dylib

include proto.mk

game.out: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

gameproto.out: $(PROTO_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

%.txt.h: %.txt
	xxd -i $< > $@

game_api.dylib: $(OBJS) $(ASSETS)
	$(CC) $(CFLAGS) -dynamiclib $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o *.out .game_api* *.dylib
	$(RM) $(ASSETS)
