CC = gcc
CFLAGS = -g
OBJS = sysutil.o session.o ftpproto.o privparent.o bitminiftp.o
LIBS = 
BIN  = bitminiftp

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -fr *.o $(BIN)