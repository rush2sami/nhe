# The root hush directory is known by including install.mk

#include ../local.mk
include ../install.mk

LIBDIR = $(DEST)/lib

SHARE = shared

NAME=sim
LIB=lib$(NAME)
VERSION=2.0

