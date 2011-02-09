include ../examples/resource.mk

OUT= testqueues.out testgenerator.out testdistributions.out

OBJECTS=

all: test

test: $(OBJECTS) $(OUT)

demo: testgenerator.out
	testgenerator.out 1000 10000 1

install:
	@echo nothing to install

clean:
	rm -f *.o *.out
	rm -f -r ptrepository
