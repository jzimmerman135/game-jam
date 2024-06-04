PROTO_OBJ=proto.o proto/lib/cJSON/cJSON.o map.o
CFLAGS+=-g

OBJS+=palette.o

proto_clean:
	$(RM) gameproto $(PROTO_OBJ)
