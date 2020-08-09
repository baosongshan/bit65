CC = gcc
CFLAGS = -g
OBJS = sysutil.o session.o ftpproto.o privparent.o bitminiftp.o str.o privsock.o
LIBS = -lcrypt
BIN  = bitminiftp

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -fr *.o $(BIN)