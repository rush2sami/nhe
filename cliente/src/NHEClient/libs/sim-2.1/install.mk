## <h1>The Hush Installation -- install.mk</h1>
## The hush package will be installed in the HUSH root directory:
## ( containing lib, include, bin ).
## The hush  root directory is defined by the HUSH variable.
## Search for #### to check for items that may
## need to be modified.

ROOTDIRS = bin lib include man doc examples

#### Select name and version number: 

NAME=hush
VERSION=3.0

#### Set date and version

DATE : sh = date "+(%b-%e-%Y)"
HUSHVERSION = hush-$(VERSION)--$(DATE)

#### Select the root directory (which will have ROOTDIRS):

HUSH = /home/eliens/dv/hush
DEST = $(HUSH)
LOCAL = ..

#### Config options

CONFIGOPTIONS = --verbose --prefix=$(HUSH)  --exec-prefix=$(HUSH)

## Select a directory to make a distribution:
## It is not very likely that you will need it!

DISTRIB = /home/eliens/dv/hush/src/distribution
SELECT  = $(LEVELS)

## The hush distribution is built in levels.
## You will definitely need the levels API INCLUDE and LIBRARY
## but even there you have some choice, although very limited.

#### Select the appropriate levels: 

LEVELS = api-$(VERSION) contrib examples packages media tools shells man 
 
## For each level the contents must be indicated.
## If a level is not included in LEVELS its
## content definition will have no effect.
## For convenience, select a
## suitable cfg file, such as this one or api.cfg
## which is a simplified version.

#### Select level contents: 
## API contains the basic hush classes, the'abstract' widget 
## Ahierarchy and the default runtime Tcl/Tk support for hush
## The DV variable is used to indicate
## alternative runtime support for hush.

API = hush widgets dv
DV  =

## The order for man must be mann manl (for making doc)

MAN = man1 man2 man3 man4 man5 man6 man7 man8 man9 mann manl 
SHELLS = hush
PACKAGES = adt-1.3 sim-2.1 xanim-1.1 hymne-1.2 client-1.0.1 web-2.0
#TCLTK=  tcl7.4 tk4.0 tclX7.4a-b5 stcl0.2 
TCLTK = tcl7.5 tk4.1
CONTRIB = TkSteal-3.3 blt-1.8 
MEDIA = mesa-1.2.2 qvlib-1.0
LANGUAGES = 
SUPPORT = sp-1.0.1
TOOLS = grammar-1.0 html-1.0 filters-1.0
EXAMPLES = hush widgets graphics coffee drawtool
GALLERY = randomrunner amuse
EXTRA =

## The hush package needs the tcl/tk stuff,
## which may be found in a number of places.
## For convenience it is included in the distribution.
## Also, you may want extensions to Tcl/Tk. The are
## standardly located in the contrib directory
## 
## You must decide what Tcl/Tk and contrib libraries
## will be needed.
## Remove the libraries you do not have or want to be included
## You can find all libraries in tcl or  contrib, except for
## hush and the libraries that come from the packages, the languages,
## and whatever.
## Unfortunately, the order of declaration sometimes matters.

#### Set the flags
##The flags indicated below will result in
##defining the flag in <dv/flags.h>.
##The meaning of the known flags is given below:
##<ul>
##<li> MULTINTERP -- requires Tk4.1 or tcltk/stcl1.0.
##<li> REGEX -- taken from splash
##<li> THREADS -- Solaris only
##</ul>


FLAGS = MULTINTERP REGEX 
 
#### Select the libraries:

#TKLIBS = $(TCLTKLIBDIRS) -lsafe -ltk -ltcl 
TKLIBS = $(TCLTKLIBDIRS) -ltk4.1 -ltcl7.5 
HUSHLIBS =  -lhush -lwhush -lwtk -lwidgets -ldvtcl -ldvtk -ldv
OTHERLIBS = 
CONTRIBLIBS = 

## Compiling is easy compared to linking and finding
## the appropriate libraries.
## Standardly the Tcl/Tk and contrib libraries
## are assumed to be in the $(HUSH)/lib
## directory. However, at the expense of explicitly
## indicating where the libraries can be found you
## may change this.

#### Select the place where libraries are searched for:

TCLTKDIR = /home/eliens/dv/hush/src/tcltk

LOCALLIBDIRS = -L$(LOCAL)/lib -R$(LOCAL)/lib
TCLTKLIBDIRS = # -L$(TCLTKDIR)/lib -R$(TCLTKDIR)/lib
HUSHLIBDIRS = -L$(HUSH)/lib -R$(HUSH)/lib

LIBDIRS = $(LOCALLIBDIRS) $(HUSHLIBDIRS) $(TCLTKLIBDIRS)

#### Set HUSH_LIBRARY TK_LIBRARY and TCL_LIBRARY

HUSH_LIBRARY = $(HUSH)/lib/hush
TCL_LIBRARY = $(HUSH)/lib/tcl7.5
TK_LIBRARY = $(HUSH)/lib/tk4.1

#### Set default flags for compilation -- check your Tcl/Tk installation  

TCLSRC = $(TCLTKDIR)/tcl7.5/generic
TKSRC = $(TCLTKDIR)/tk4.1/generic

## and don't forget to check the settings below.

TCLFLAGS = -I$(TCLSRC)
TKFLAGS = -I$(TKSRC)
TCLTKFLAGS = $(TCLFLAGS) $(TKFLAGS)

LOCALINCLUDE = -I$(LOCAL) -I$(LOCAL)/include
HUSHINCLUDE = -I$(HUSH)/include

INCLUDEFLAGS = $(LOCALINCLUDE) $(HUSHINCLUDE)

CFLAGS = $(COMPILERFLAGS) $(INCLUDEFLAGS)  $(TCLTKFLAGS) $(SYSTEMFLAGS)
LDFLAGS = $(LIBDIRS) $(OTHERLIBS) $(HUSHLIBS) $(TKLIBS) $(SYSTEMLIBS)

CCFLAGS = $(CFLAGS)

#### Decide whether he libraries will be  shared or nonshared:

SHARE = shared   

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

#### Select your favorite (available) C++ compiler:
## Eliminate -g when your confident

 CCC = g++
 COMPILERFLAGS = -g -fPIC -Wall 
 DPFLAGS = -I/opt/gcc-2.7.2/lib/g++-include

### CCC = CC 
### COMPILERFLAGS = -g -PIC +w # -D_REENTRANT
### DPFLAGS = -I/opt/SUNWspro/SC3.0.1/include/CC

##CCC = /net/SunSoft-beta/SUNWspro/bin/CC
##COMPILERFLAGS = -g -PIC +w

CC = $(CCC) $(COMPILERFLAGS)

#### And select your (ordinary) C compiler:

cc = gcc -fPIC 

#### If you work with an ORB set the appropriate ORB FLAGS

ORBIX = /opt/Orbix-2.0.1/corba2
IDL   = $(ORBIX)/bin/idl
IDLFLAGS = -B -h .idl.h -c .clt.c -s .srv.c -N -I$(HUSH)/idl

## Additionally, for sound and X windows you may need
## flags and libraries that are not set by default.
## Typically these settings change per system in
## an albeit unpredictable way. Sorry for that.

##### Select your system flags and libs:

XDIR = /usr/openwin
XINCLUDEDIR = -I$(XDIR)/include
XLIBDIR = -R$(XDIR)/lib -L$(XDIR)/lib 
XTLIBS = -lXt -lXext -lXaw -lXmu
SPLIBS = -lapp -lxentmgr -lparser -lentmgr -ljjc
TCLTKXLIBS = -ltclx -ltcltk -lintl

THREADLIB = # -lthread
 
SYSTEMFLAGS = $(XINCLUDEDIR) 
SYSTEMLIBS = $(XLIBDIR) -lX11 -lm $(THREADLIB) -lsocket -lnsl -ldl

## To compile the examples and test programs, we define:

.SUFFIXES: .out 
.c.out:; $(CC) $(CFLAGS) $*.c -o $*.out $(LDFLAGS)
.cc.out:; $(CCC) $(CCFLAGS) $*.cc -o $*.out $(LDFLAGS)

## Remember, ignorance is bliss.
## That seems to exlude Solaris as a source of happiness.

##<h4>Miscellaneous</h4>
## In striving for portability some system commands
## are addressed by macros. This allows you to change
## these commands if needed.
## For example, Solaris does not need nor support
## ranlib. The solution is to define RANLIB as : or echo.
## See ../boot/advice.mk to check whether your definitions
## are right. Make the changes MANUALLY!

#### Check if the system commands are correct:

LD = $(CC) -G 
CP = cp -r
MV = mv
AR = ar
CAT = cat
ECHO = echo
RANLIB = echo

## For the various packages you may need to set some flags
## these flags are highly dependent on the system you are 
## working on. (You're on your own here.)

#### For WWW -- packages/web-1.0

MACHINE=sun4-sol2

WEBFLAGS = -DUSE_DIRENT -D__svr4__ \
     -DNGROUPS=NGROUPS_MAX -Dd_namlen=d_reclen \
	  -DNO_GETWD -DHAS_GETCWD -DPOSIXWAIT 

#### For Hymne -- packages/hymne-0.95

HYMNEFLAGS = -I/usr/demo/SOUND/include

## You need not change the following definition 
## It is used to generate resource.mk files     
## for the examples in examples and the gallery.

.SUFFIXES: .in .mk .html .cfg .tcl .ps .t .h .hh .c .cc \
	   .idl .srv.c .clt.c .idl.h \
	   .l .y .l.html .y.html .java .class .java.html \
	   .idl.html .c.html .h.html .cc.html .hh.html \
	   .mk.html .cfg.html .tcl.html \
	   .1 .2 .3 .4 .5 .6 .7 .8 .9 .n .l \
	   .dir .tar .tar.Z

default: all

redo: clean config build 

.idl.idl.h: $*.idl
	$(IDL) $(IDLFLAGS) $*

LATEX = latex
DVIPS = dvips
TOHTML  = doc-html

index.html: index	
	$(TOHTML) < index > index.html

index.ps: index.html
	#$(TOHTML) < index > index.html
	html-latex.flt < index.html > index.tex
	$(LATEX) index.tex
	dvips < index.dvi > index.ps
	

Makefile.html:
	cc-html Makefile > Makefile.html

.1.html:; man-html $*.1 > $*.html
.2.html:; man-html $*.2 > $*.html
.3.html:; man-html $*.3 > $*.html
.4.html:; man-html $*.4 > $*.html
.5.html:; man-html $*.5 > $*.html
.6.html:; man-html $*.6 > $*.html
.7.html:; man-html $*.7 > $*.html
.8.html:; man-html $*.8 > $*.html
.9.html:; man-html $*.9 > $*.html
.l.html:; man-html $*.l > $*.html
.n.html:; man-html $*.n > $*.html

.t.html:
	$(TOHTML)  $*.t > $*.html

.h.h.html:
	cc-html $*.h > $*.h.html
 
.c.c.html:
	cc-html $*.c > $*.c.html

.hh.hh.html:
	cc-html $*.hh > $*.hh.html
 
.cc.cc.html:
	cc-html $*.cc > $*.cc.html

.idl.idl.html:
	cc-html $*.idl > $*.idl.html

.java.java.html:
	cc-html $*.java > $*.java.html

.l.l.html:
	cc-html $*.l > $*.l.html

.y.y.html:
	cc-html $*.y > $*.y.html
 
.mk.mk.html:
	cc-html $*.mk > $*.mk.html
 
.cfg.cfg.html:
	cc-html $*.cfg > $*.cfg.html

.tcl.tcl.html:
	cc-html $*.tcl > $*.tcl.html
 
.html.ps:
	html-latex $*.html > $*.tex
	$(LATEX) $*.tex
	dvips < $*.dvi > $*.ps

## To create resource.mk in examples

.in.mk:
	@echo ---- Configuring $*.mk -------- for $(HUSH)
	  @echo --- Configuring $$i ---- for $(HUSH); \
	  echo "HUSH = $(HUSH)" > $*.mk; \
	  echo "CC = $(CC)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  echo "HUSHLIBS = $(HUSHLIBS)" >> $*.mk; \
	  echo "TKLIBS = $(TKLIBS)" >> $*.mk; \
	  echo "SPLIBS = $(SPLIBS)" >> $*.mk; \
	  echo "CONTRIBLIBS = $(CONTRIBLIBS)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  echo "SYSTEMFLAGS = $(SYSTEMFLAGS)" >> $*.mk; \
	  echo "SYSTEMLIBS = $(SYSTEMLIBS)" >> $*.mk; \
	  echo "DPFLAGS = $(DPFLAGS)" >> $*.mk; \
	  echo "" >> $*.mk; \
	  cat $*.in >> $*.mk; 
	@echo ==== $*.mk  created ========= for $(HUSH)

## To allow for packing and compression

.dir.tar.Z:
	@echo ---- Tarring $*.dir -------- for $(HUSH)
	@set -x; if [ -d $*.dir ]; then \
	    tar cvFFf - $*.dir > $*.tar; compress $*.tar; \
	    fi;
	@echo ---- Compressed $* -------- for $(HUSH)

tar.Z.dir:
	@echo ---- Uncompressing $*.tar.Z -------- for $(HUSH)
	@set -x; if [ -f $*.tar.Z ]; then \
	    uncompress $*.tar.Z; tar xvf $*.tar; \
	    fi;
	@echo ---- Untarred $* -------- for $(HUSH)

## The End 
