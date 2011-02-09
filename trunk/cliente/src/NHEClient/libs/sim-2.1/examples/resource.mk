HUSH = /home/eliens/dv/hush
CC = g++ -g -fPIC -Wall

HUSHLIBS = -lhush -lwhush -lwtk -lwidgets -ldvtcl -ldvtk -ldv
TKLIBS =  -ltk4.1 -ltcl7.5
SPLIBS = -lapp -lxentmgr -lparser -lentmgr -ljjc
CONTRIBLIBS = 

SYSTEMFLAGS = -I/usr/openwin/include
SYSTEMLIBS = -R/usr/openwin/lib -L/usr/openwin/lib -lX11 -lm  -lsocket -lnsl -ldl
DPFLAGS = -I/opt/gcc-2.7.2/lib/g++-include

LIBDIRS = -L$(HUSH)/lib -R$(HUSH)/lib

OTHERS = -lsim 
CFLAGS = -g  -I$(HUSH)/include $(SYSTEMFLAGS)
LDFLAGS = $(LIBDIRS) $(HUSHLIBS) $(OTHERS) $(TKLIBS) \
	  $(SYSTEMLIBS)


.SUFFIXES: .out
.c.out:
	$(CC) -o $*.out $*.c $(CFLAGS) $(LDFLAGS) 
