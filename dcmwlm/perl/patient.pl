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
#   This perl script displays the patient data menu for a storage area.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: patient.pl,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:10  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#

require 'prefs.ph';
require 'layout.ph';
require 'password.ph';
require 'lock.ph';
require 'write.ph';

$path_info=$ENV{'PATH_INFO'};

$aetitle = '';
$passwd = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $rest) = split(/\//, $path_info);
}
if (($passwd eq '') || (! &checkurlcode($passwd, $aetitle)))
{
  # Password is incorrect.
  &page_title("Password invalid");
  printf("<A HREF=\"%s\">Click here</A> to return to main menu.\n", $prefs{'main.pl'});
  &page_footer;
} else {
  # Password is correct.
  &page_title("Patient Data - '$aetitle'");
  printf("<TABLE BORDER CELLSPACING=1 WIDTH=100%>\n");
  printf("<TR><TD nowrap><B>Patient's Name</B></TD>\n");
  printf("<TD nowrap><B>Birth Date</B></TD>\n");
  printf("<TD nowrap><B>Patient ID</B></TD><TD></TD><TD></TD></TR>\n");

  &set_readlock("$prefs{'data_path'}/$aetitle");
  $filename = "$prefs{'data_path'}/$aetitle/$prefs{'datafile'}";
  &read_environment($filename);
  &release_lock;
  @keys = @PATIENT_KEYS;
  @values = ();
  for (@keys){ push(@values,join("\\",$PATIENT_VALUES{"$_\\name"},$_)); }
  for (sort(@values))
  {
    ($name, $key) = split(/\\/);
    printf("<TR><TD nowrap>%s</TD><TD nowrap>%s</TD><TD nowrap>%s</TD>",
      $name, $PATIENT_VALUES{"$key\\birthdate"}, $key);
    printf("<TD nowrap><A HREF=\"%s/%s/%s/%s\">update</A></TD>",
      $prefs{'patiedit.pl'}, $aetitle, $passwd, unpack("H*",$key));
    printf("<TD nowrap><A HREF=\"%s/%s/%s/%s\">delete</A></TD></TR>\n",
      $prefs{'patidel.pl'}, $aetitle, $passwd, unpack("H*",$key));
  }
  printf("</TABLE><p>\n");
  printf("<A HREF=\"%s/%s/%s\">Create a new patient</A><p>\n",
      $prefs{'patiedit.pl'}, $aetitle, $passwd);
  printf("<A HREF=\"%s/%s/%s\">Back to main Menu</A>\n",
      $prefs{'main.pl'}, $aetitle, $passwd);
  &page_footer;
}
