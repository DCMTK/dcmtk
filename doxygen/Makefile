#
#	Makefile for doxygen
#


SHELL = /bin/sh
DOXYGEN = doxygen

html:
	$(DOXYGEN) htmldocs.cfg

man:
	$(DOXYGEN) manpages.cfg

all: html man


clean:
	rm -f *.log *.bak

distclean: clean
	rm -rf htmldocs
	rm -rf manpages
