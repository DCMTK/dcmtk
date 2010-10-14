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
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: log.ph,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:06  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
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
