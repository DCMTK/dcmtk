#!/usr/local/bin/perl
#

$program='../apps/dcmpsprt';
# set '[Database] Directory = /data/dicom-images/rsna99/results' in test.cfg for better performance
$options='-c test.cfg';
$source='/data/dicom-images/rsna99';
# this entry is only used for message output, see comment on test.cfg
$destination='/data/dicom-images/rsna99/results';
$img_ext='.img';
$pst_ext='.pres';

printf("Create hardcopy objects from images in $source\nand store results in $destination\n\n");

# first, collect a list of all files in the archive
printf("reading file list...\n");
@files = `find $source/*$img_ext -type f -print"`;

# then create a list of all files
$files_idx = 0;
for (@files)
{
  if ($_ =~ /.*\/files$/)
  {
    $files[$files_idx++] = $_; 
  }
}

printf("creating hardcopy grayscale images...\n");
for (@files)
{
  chop($_);
  if ($_ !~ /.*\/files$/)
  {
    ($basename) = ($_ =~ /$source(.*)$img_ext/);
    printf("processing: $_\n");
    `$program $options -p $source$basename$pst_ext $_`;
  }
}
