#!/bin/bash

oldFail=0
newFail=1
mkdir -p FILE

#  bring down the large file from S3
aws s3 cp s3://trice-test-data/dcmtk/dcmjpeg/apps/FILE/file FILE/file

# try it using the old version
/usr/local/bin/dcmj2pnm --conv-guess-lossy --use-frame-number --all-frames FILE/file FILE/OLD
if [ $? -ne 0 ]
then
   echo "old version failed"
   oldFail=1
fi

# try with the new version
./dcmj2pnm --conv-guess-lossy --use-frame-number --all-frames FILE/file FILE/NEW
if [ $? -eq 0 ]
then
   echo "new version succeeded"
   newFail=0
      # Make the movie - commented out for now
   # ffmpeg -r 24 -b 5000k -i FILE/NEW.f%d.ppm -vcodec libx264 FILE/movie.mp4
fi

if [ $oldFail -eq 0 -o $newFail -eq 1 ]
then
   echo "Large File Test failure"
   exit 1
fi

#verify static images are good
# get a static image
aws s3 cp s3://trice-test-data/dcmtk/dcmjpeg/apps/FILE/aud.dcm FILE/aud.dcm

# convert using old version
dcmj2pnm +oj  FILE/aud.dcm FILE/aud.old.jpg

# convert using new version
./dcmj2pnm +oj  FILE/aud.dcm FILE/aud.new.jpg

# ensure the results are the same between the new version
cmp FILE/aud.old.jpg FILE/aud.new.jpg
if [ $? -ne 0 ]
then
   echo "Static Image Test failed"
   exit 1
fi

exit 0  # perfect

