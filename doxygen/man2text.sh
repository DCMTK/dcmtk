#!/bin/sh

# Generate a text representation of the man pages
cd manpages/man1/
for file in *.1 ; do \
	MANWIDTH=80 man -E ascii -l $file | col -bxp > ../../man2text/`echo $file | sed 's/\(.*\.\)1/\1txt/'` ;\
done
