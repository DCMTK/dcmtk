This repo contains the latest snapshot of dcmtk (10/2015).

dcmj2pnm has memory problems if the size of the decompressed pixel data exeeds the sizeof(size_t) (ie., 4 bytes). 
In this case the unsigned long overflows and the size to be allocated is incorrect.

This will either lead to a crash or corrupt output files.

dcm2pnm.cc is included in several utilities.  To minimize risk, a copy of this file is made so that only dcmj2pnm is built with these changes.
The changes include looping through the frames and writing out each outputfile (instead of swallowing the whole decompressed file into memory).
This approach uses very little memory.

original dcm2pnm: dcmimage/apps/dcm2pnm
modified dcm2pnm: dcmjpeg/apps/dcm2pnm
