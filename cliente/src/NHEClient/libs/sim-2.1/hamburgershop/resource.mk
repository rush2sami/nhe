HUSH = /home/hush
CC = g++ -fPIC -Wall

HUSHLIBS = -lhush -lwidgets -ldv
TKLIBS =  -lsafe -ltk -ltcl
XTLIBS = -lXt -lXext -lXaw -lXmu
CONTRIBLIBS = 

SYSTEMFLAGS = -I/usr/openwin/include
SYSTEMLIBS = -lm -R/usr/openwin/lib -L/usr/openwin/lib -lX11

LIBDIRS = -L$(HUSH)/lib -R$(HUSH)/lib

OTHERS = -lsim -lsocket -lnsl
CFLAGS = -g  -I$(HUSH)/include $(SYSTEMFLAGS)
LDFLAGS = $(LIBDIRS) $(HUSHLIBS) $(OTHERS) $(TKLIBS) \
	  $(SYSTEMLIBS)


.SUFFIXES: .out
.c.out:
	$(CC) -o $*.out $*.c $(CFLAGS) $(LDFLAGS) 
