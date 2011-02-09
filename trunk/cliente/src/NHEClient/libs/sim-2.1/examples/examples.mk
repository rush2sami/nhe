all: $(OUT) 

config: touch resource.mk
touch:; touch resource.in

test:; make -f mkfile test 

demo:; make -f mkfile demo 

install: 
	if [ ! -d $(DEST)/examples ]; then mkdir $(DEST)/examples; fi
	if [ ! -d $(DEST)/examples/$(NAME) ]; \
				 then mkdir $(DEST)/examples/$(NAME); fi
	cp $(FILES) $(DEST)/examples/$(NAME)
	cp mkfile $(DEST)/examples/$(NAME)/Makefile

bininstall:
	cp *.out $(DEST)/examples/$(NAME)

doc: $(DOC)
	@echo -------- Documenting $(NAME)  -- in $(DEST)
	@if [ ! -d $(DEST)/doc ]; then mkdir $(DEST)/doc; fi
	@if [ ! -d $(DEST)/doc/examples ]; then mkdir $(DEST)/doc/examples; fi
	@if [ ! -d $(DEST)/doc/examples/$(NAME) ]; \
	    then mkdir $(DEST)/doc/examples/$(NAME); fi
	@cp index.html $(NAME).html
	@cp *.html *.txt $(DEST)/doc/examples/$(NAME);
	@echo ======== Installing $(NAME) completed == in $(DEST)

cleandoc:
	rm -f *.html

clean:
	rm -f core *.a *.o out *.out lib*.so.*  *.html
	rm -f -r Templates.DB
