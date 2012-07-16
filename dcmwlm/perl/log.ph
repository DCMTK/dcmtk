#!/usr/local/bin/perl
#
#  Copyright (C) 1996-2010, OFFIS e.V.
#  All rights reserved.  See COPYRIGHT file for details.
#
#  This software and supporting documentation were developed by
#
#    OFFIS e.V.
#    R&D Division Health
#    Escherweg 2
#    D-26121 Oldenburg, Germany
#
#  for CEN/TC251/WG4 as a contribution to the Computer Assisted Radiology
#  (CAR) 1996 DICOM Demonstration.
#
#
# Module: dcmwlm (WWW Component)
#
# Author: Marco Eichelberg
#
# Purpose:
#   This module contains perl code implementing a simple log file facility.
#
#

require 'prefs.ph';

#
#  void printlog(string text)
#    appends a line with date/time and the given text
#    to the log file (the path to which is defined in prefs.pl).
#
sub printlog # (string)
{
  local($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);
  ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;

  local($filename) = join('/',$prefs{'data_path'},$prefs{'logfile'});
  if (open(outfile, ">>$filename"))
  {
    printf(outfile "%04d-%02d-%02d %02d:%02d:%02d %s\n",
      $year+1900, $mon+1, $mday, $hour, $min, $sec, @_[0]);
    close(outfile);
  }
}

1;
