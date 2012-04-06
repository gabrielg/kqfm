CFLAGS  = -g -Wall
INSTALL = /usr/bin/install
PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin

kqfm: kqfm.c

clean:
	rm -rf kqfm *.dSYM

install: kqfm
	$(INSTALL) kqfm $(BINDIR)/kqfm