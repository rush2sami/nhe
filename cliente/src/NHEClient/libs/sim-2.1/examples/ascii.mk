HUSH = /home/eliens/dv/hush
CC = g++ -g -fPIC -Wall

HUSHLIBS = -lhush -lwhush -lwtk -lwidgets -ldvtcl -ldvtk -ldv
TKLIBS =  -ltk4.1 -ltcl7.5
SPLIBS = -lapp -lxentmgr -lparser -lentmgr -ljjc
CONTRIBLIBS = 

SYSTEMFLAGS = -I/usr/openwin/include
SYSTEMLIBS = -R/usr/openwin/lib -L/usr/openwin/lib -lX11 -lm  -lsocket -lnsl -ldl
DPFLAGS = -I/opt/gcc-2.7.2/lib/g++-include



SIM = $(HUSH)
LIBDIRS = -L$(SIM)/lib -R$(SIM)/lib 
 
CFLAGS= -g -I. -DASCII -I$(SIM)/include
 
LDFLAGS =  $(LIBDIRS) -lasim -lm

OBJECTS=

.SUFFIXES: .c .o .out
.c.o:; $(CC) $(CFLAGS) -c $*.c
.c.out:; $(CC) $(CFLAGS) $(OBJECTS) $*.c -o $*.out $(LDFLAGS)
