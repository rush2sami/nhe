FILES= README Makefile xinfo
DIRS= doc examples hamburgershop include man src tests lib

# Place comments dependent on whether you install locally

#include local.mk
include install.mk


MANDIR=../..

SOURCE = sim-2.1
PACK = hush-packages-sim-$(VERSION)

all:
	cd src; make install

config: touch examples/resource.mk examples/ascii.mk
touch:; touch examples/resource.in examples/ascii.in

install:
	cp -r include/sim $(HUSH)/include
	cd src; make install
	cd include; make install

build:
	cd src; make build

asim:
	cd src; make clean
	cd ascii; make install

doc:
	cd include; make doc;

clean:
	cd src; make clean
	cd ascii; make clean
	cd examples; make clean
	cd hamburgershop; make clean
	rm -f lib/libsim*

shar:
	shar $(FILES) $(DIRS) > sim.shar

test:

oldinstall:
	cp -r man/man1/*.1 $(HUSH)/man/man1
	cp -r man/man2/*.2 $(HUSH)/man/man5
	cp -r include/* $(HUSH)/include
	cp lib/libsim.a $(HUSH)/lib; 
	cd $(HUSH)/lib; ranlib libsim.a
	cp -r examples/* $(HUSH)/examples/sim

maninstall:
	cp -r man/man1/*.1 $(MANDIR)/man/man1
	cp -r man/man6/*.6 $(MANDIR)/man/man6


pack:
	@echo -------- Start distribution $(PACK) -- in $(DISTRIB)
	@if [ ! -d $(DISTRIB) ]; then mkdir $(DISTRIB); fi
	@make clean
	@rm -f $(DISTRIB)/$(PACK).tar.Z $(DISTRIB)/$(PACK).uu;
	@set -x; cd ..; tar cvf - $(SOURCE) > $(DISTRIB)/$(PACK).tar;
	@echo -------- Compressing $(PACK)
	@cd $(DISTRIB); compress $(PACK).tar; 
	@echo ======== Distribution $(PACK) completed == in $(DISTRIB)
