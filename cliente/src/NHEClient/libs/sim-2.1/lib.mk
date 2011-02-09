all: lib

config:

lib: $(SHARE)

touch:
	if [ -n "$(VPATH)" ]; then (cd $(VPATH); make clean) fi

objects: $(VOBJS) $(OBJECTS)

shared: objects
	rm -f $(LIB).so.* 
	$(LD) -o $(LIB).so.$(VERSION) $(OBJECTS) 

nonshared: objects
	rm -f $(LIB).a
	ar cr $(LIB).a $(OBJECTS)
	$(RANLIB) $(LIB).a

build: lib

install: lib 
	@echo -------- Installing $(LIB)  -- $(SHARE) --- in $(DEST)
	@if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi
	@if [ $(SHARE) = "nonshared" ]; then \
		mv $(LIB).a $(LIBDIR); \
		cd $(LIBDIR); $(RANLIB) $(LIB).a; \
	else \
	mv $(LIB).so.$(VERSION) $(DEST)/lib; \
	set -x; cd $(DEST)/lib; \
	   rm -f $(LIB).so; ln -s $(LIB).so.$(VERSION) $(LIB).so; \
	fi;
	@echo ======== Installing $(LIB) completed ====== in $(DEST)

clean:
	rm -f core *.a *.o *.out lib*.so.* 
	rm -f -r ptrepository
