#   _ _ _     _                         _ _
#  | (_) |__ | | _____  ___ _ __   __ _| (_)_   _____
#  | | | '_ \| |/ / _ \/ _ \ '_ \ / _` | | \ \ / / _ \
#  | | | |_) |   <  __/  __/ |_) | (_| | | |\ V /  __/
#  |_|_|_.__/|_|\_\___|\___| .__/ \__,_|_|_| \_/ \___|
#                          |_|
#
# Logo (C) Fabio Busatto <fabio.busatto@sikurezza.org>

CC=gcc
CFLAGS=-fPIC 
LDFLAGS=-shared -Wl,-soname,libkeepalive.so
LDLIBS=-ldl
OBJ	= libkeepalive.o keepalivecfg.o cfile.o portlist.o

default: libkeepalive.so

%.so: %.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)

libkeepalive.so:	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)  $(LDLIBS)
libkeepalive.o:		libkeepalive.c constants.h params.h

clean:
	rm -f *.so *.o
