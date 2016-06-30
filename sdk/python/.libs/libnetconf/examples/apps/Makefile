#
# Makefile for libnetconf example applications
#
# Author: Radek Krejci <rkrejci@cesnet.cz>
# Copyright (c) 2012-2014 CESNET, z.s.p.o.
#

CC      = gcc
CFLAGS  = -Wall -g
INCLUDE = -I../../src/ 
LIB     = -lnetconf
LIBPATH	= -L../../.libs/
TARGETS = get notif

all: $(TARGETS)

get: get.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBPATH) $(LIB)

notif: notif.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBPATH) $(LIB)

clean:
	rm -f *.o
	rm -f $(TARGETS)

