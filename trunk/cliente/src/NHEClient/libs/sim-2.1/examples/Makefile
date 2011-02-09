include ../../../install.mk

NAME = sim

FILES = browse Makefile README *.mk *.txt *.c



SRC : sh = ls *.c

HTML = ${SRC:.c=.c.html}
DOC = index.html $(HTML)

OUT= ${SRC:.c=.out}

include examples.mk
