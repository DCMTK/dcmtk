#
#	Makefile for doxygen
#


SHELL = /bin/sh
DOXYGEN = doxygen

all: html man

html:
	$(DOXYGEN) htmldocs.cfg

man:
	$(DOXYGEN) manpages.cfg
	rm -f manpages/man1/*.man.1

clean:
	rm -f *.log $(TRASH)

distclean: clean
	rm -rf htmldocs
	rm -rf manpages
