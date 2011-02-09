#### For local installations

HUSH = /tmp/sim-install
DEST = $(HUSH)
LOCAL = ..

####Select the libraries:

TKLIBS = $(TCLTKLIBDIRS) -ltk -ltcl 
HUSHLIBS = -lwidgets -lhush 
OTHERLIBS = 
CONTRIBLIBS = 


SHARE = shared   

CFLAGS = -I$(LOCAL)/include

##<html>
## <h4>Compiler</h4>
## The hush package is only tested on a Sparc,
## with AT&T 3.0.1 and g++ (?), under both Sunos 4.3.1 and 
## Solaris 2.
## Also, a port of an earlier version to linux exists.
## Information about porting hush to other environments
## will be appreciated.
## (tested for: AT&T C++ 3.0, Sun CC 4.0 , g++ 2.5.8/ 2.7.0)
##<p>
## If you make a shared version add the -PIC, -pic or -fpic option
## when calling the compiler.
##</html>

#### Select your favorate (available) C++ compiler:

CCC = g++ -fPIC
CC = $(CCC)

#### And select your (ordinary) C compiler:

cc = gcc -fPIC 

##<html>
## Additionally, for sound and X windows you may need
## flags and libraries that are not set by default.
## Typically these settings change per system in
## an albeit unpredictable way. Sorry for that.
##</html>

##### Select your system flags and libs:

XDIR = /usr/openwin
XINCLUDEDIR = -I$(XDIR)/include
XLIBDIR = -R$(XDIR)/lib -L$(XDIR)/lib 
XTLIBS = -lXt -lXext -lXaw -lXmu
 
SYSTEMFLAGS = $(XINCLUDEDIR) 
SYSTEMLIBS = $(XLIBDIR) -lX11 -lm -lsocket -lnsl -ldl


##<html>
## To compile the examples and test programs, we define:
##</html>

.SUFFIXES: .out 
.c.out:; $(CC) $(CFLAGS) $*.c -o $*.out $(LDFLAGS)

##<html>
## Remember, ignorance is bliss.
## That seems to exlude Solaris as a source of happiness.
##</html>


##<html>
##<h4>Miscellaneous</h4>
## In striving for portability some system commands
## are addressed by macros. This allows you to change
## these commands if needed.
## For example, Solaris does not need nor support
## ranlib. The solution is to define RANLIB as : or echo.
## See ../boot/advice.mk to check whether your definitions
## are right. Make the changes MANUALLY!
##</html>

LD = ld -G 
CP = cp -r
MV = mv
AR = ar
CAT = cat
ECHO = echo
RANLIB = echo

##<html>
## For the various packages you may need to set some flags
## these flags are highly dependent on the system you are 
## working on. (You're on your own here.)
##</html>

#### For WWW -- packages/web-1.0

MACHINE=sun4-sol2

WEBFLAGS = -DUSE_DIRENT -D__svr4__ \
     -DNGROUPS=NGROUPS_MAX -Dd_namlen=d_reclen \
	  -DNO_GETWD -DHAS_GETCWD -DPOSIXWAIT 

#### For Hymne -- packages/hymne-0.95

HYMNEFLAGS = -I/usr/demo/SOUND/include

##<html>
## You need not change the following definition 
## It is used to generate resource.mk files     
## for the examples in examples and the gallery.
##</html>

.SUFFIXES: .in .mk .html .cfg .tcl .ps .t .h .c  \
	   .c.html .h.html .mk.html .cfg.html .tcl.html \
	   .1 .2 .3 .4 .5 .6 .7 .8 .9 .n .l

default: all

index.html: index	
	ht-ml.flt < index > index.html

Makefile.html:
	cc-html.flt < Makefile > Makefile.html

.1.html:; man-html.flt < $*.1 > $*.html
.2.html:; man-html.flt < $*.2 > $*.html
.3.html:; man-html.flt < $*.3 > $*.html
.4.html:; man-html.flt < $*.4 > $*.html
.5.html:; man-html.flt < $*.5 > $*.html
.6.html:; man-html.flt < $*.6 > $*.html
.7.html:; man-html.flt < $*.7 > $*.html
.8.html:; man-html.flt < $*.8 > $*.html
.9.html:; man-html.flt < $*.9 > $*.html
.l.html:; man-html.flt < $*.l > $*.html
.n.html:; man-html.flt < $*.n > $*.html

.t.html:
	ht-ml.flt < $*.t > $*.html

.h.h.html:
	cc-html.flt < $*.h > $*.h.html
 
.c.c.html:
	cc-html.flt < $*.c > $*.c.html
 
.mk.mk.html:
	cc-html.flt < $*.mk > $*.mk.html
 
.cfg.cfg.html:
	cc-html.flt < $*.cfg > $*.cfg.html

.tcl.tcl.html:
	cc-html.flt < $*.tcl > $*.tcl.html
 
.html.ps:
	html-latex.flt < $*.html > $*.tex
	latex $*
	dvips < $*.dvi > $*.ps

##<html>
## To create resource.mk in examples
##</html>

.in.mk:
	@echo ---- Configuring $*.mk -------- for $(HUSH)
	  @echo --- Configuring $$i ---- for $(HUSH); \
	  echo "HUSH = $(HUSH)" > $*.mk; \
	  echo "CC = $(CC)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  echo "TKLIBS = $(TKLIBS)" >> $*.mk; \
	  echo "HUSHLIBS = $(HUSHLIBS)" >> $*.mk; \
	  echo "CONTRIBLIBS = $(CONTRIBLIBS)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  echo "SYSTEMFLAGS = $(SYSTEMFLAGS)" >> $*.mk; \
	  echo "SYSTEMLIBS = $(SYSTEMLIBS)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  cat $*.in >> $*.mk; 
	@echo ==== $*.mk  created ========= for $(HUSH)


##<html>
## The End 
##</html>
