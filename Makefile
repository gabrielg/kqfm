CFLAGS  = -g -Wall -std=c99
INSTALL = /usr/bin/install
PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man

kqfm: kqfm.c

clean:
	rm -rf kqfm *.dSYM

install: kqfm
	$(INSTALL) kqfm $(BINDIR)/kqfm
	$(INSTALL) man/kqfm.1 $(MANDIR)/man1/kqfm.1

doc:
	ronn --roff man/*.md
