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
#   This perl script displays the worklist data menu for a storage area.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:02 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: worklist.pl,v $
#   Revision 1.2  2010-10-14 13:02:02  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:20  wilkens
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
require 'isocode.ph';

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
  &page_title("Worklist Entries - '$aetitle'");
  printf("<TABLE BORDER CELLSPACING=1 WIDTH=100%>\n");
  &set_readlock("$prefs{'data_path'}/$aetitle");
  $filename = "$prefs{'data_path'}/$aetitle/$prefs{'datafile'}";
  &read_environment($filename);
  printf("<TR><TD nowrap><B>Accession No.</B></TD>\n");
  printf("<TD nowrap><B>Patient's Name</B></TD>\n");
  printf("<TD nowrap><B>Procedure Description</B></TD><TD></TD><TD></TD></TR>\n");
  @dicomfiles = ();
  if (opendir(DIRHANDLE, "$prefs{'data_path'}/$aetitle"))
  {
    $dirent = readdir(DIRHANDLE);
    while ($dirent ne '')
    {
      if ($dirent =~ /.*\.$prefs{'dicom_ext'}$/) { push(@dicomfiles, $dirent); }
      $dirent = readdir(DIRHANDLE);
    }
    closedir(DIRHANDLE);
  }
  &release_lock;

  $commandline = "$prefs{'readoviw'}";
  for (@dicomfiles) { $commandline = "$commandline $prefs{'data_path'}/$aetitle/$_"; }
  if ($#dicomfiles >= 0)
  {
    ($dummy, $overview) = split(/\\/, `$commandline`, 2);
    for (@dicomfiles)
    {
       ($accession, $name, $description, $overview) = split(/\\/,$overview, 4);
       $description=pack("H*",$description);
       if (length($description)>40) { $description = substr($description,0,40); }
       printf("<TR><TD nowrap>%s</TD>\n",&iso_to_html(pack("H*",$accession)));
       printf("<TD nowrap>%s</TD>\n",&iso_to_html(pack("H*",$name)));
       printf("<TD nowrap>%s</TD>\n",&iso_to_html($description));
       printf("<TD nowrap><A HREF=\"%s/%s/%s/%s\">update</A></TD>",
         $prefs{'workedit.pl'}, $aetitle, $passwd, unpack("H*",$_));
       printf("<TD nowrap><A HREF=\"%s/%s/%s/%s\">delete</A></TD></TR>\n",
         $prefs{'workdel.pl'}, $aetitle, $passwd, unpack("H*",$_));
       printf("</TR>\n");
    }
  }
  printf("</TABLE><p>\n");

  # Check if it is possible to create a new entry
  if (($#PATIENT_KEYS >= 0)&&($#PROCEDURESTEP_KEYS >= 0))
  {
    printf("<A HREF=\"%s/%s/%s\">Create a new worklist entry</A><p>\n",
      $prefs{'workedit.pl'}, $aetitle, $passwd);
  } else {
    printf("You cannot create a new worklist entry unless there is at least");
    printf(" one patient and one procedure step.<p>\n");
  }
  printf("<A HREF=\"%s/%s/%s\">Back to main Menu</A>\n",
      $prefs{'main.pl'}, $aetitle, $passwd);
  &page_footer;
}
