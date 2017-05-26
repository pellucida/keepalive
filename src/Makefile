
CC=gcc
CFLAGS=-fPIC -fvisibility=hidden -DLIBKEEPALIVE_CFG=\"/proxy/keepalive.cfg\"

LDFLAGS=-shared -Wl,-soname,libkeepalive.so
LDLIBS=-ldl
OBJ	= libkeepalive.o keepalivecfg.o cfile.o portlist.o

default: libkeepalive.so

libkeepalive.so:	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)  $(LDLIBS)


libkeepalive.o: libkeepalive.c constants.h keepalivecfg.h
cfile.o: cfile.c constants.h new.h cfile.h
keepalivecfg.o: keepalivecfg.c constants.h new.h cfile.h portlist.h keepalivecfg.h
portlist.o: portlist.c constants.h new.h portlist.h

clean:
	rm -f *.so *.o
