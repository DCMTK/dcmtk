#!/bin/sh

# remove ".ad l" statement at the beginning of
# each manpage file to enable justified text.

for file in manpages/man1/*.1 ; do
  echo Patching file $file...
  sed "/^.ad l/d" $file > manpage.tmp
  mv manpage.tmp $file
done
