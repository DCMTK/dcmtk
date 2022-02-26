# read a text file, perform a regex search/replace and write it back
file(READ "${INFILE}" CONTENT)
string(REGEX REPLACE "${REGEX}" "${REPLACE}" CONTENT "${CONTENT}")
file(WRITE "${OUTFILE}" "${CONTENT}")
