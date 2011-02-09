include ../resource.mk

NAME=asim
CFLAGS += -DASCII

SRC : sh = ls ../src/*.c

VPATH = ../src
VOBJS = ${SRC:.c=.o}

include ../src/sim.mk

include ../lib.mk
