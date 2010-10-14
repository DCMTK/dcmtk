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
#   This perl script allows to delete a worklist entry from a storage area.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:02 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: workdel.pl,v $
#   Revision 1.2  2010-10-14 13:02:02  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:18  wilkens
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
require 'isocode.ph';

$path_info=$ENV{'PATH_INFO'};

$aetitle = '';
$passwd = '';
$dicomfile = '';
$command = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $dicomfile, $command, $rest) = split(/\//, $path_info);
}

if (($passwd eq '') || (! &checkurlcode($passwd, $aetitle)))
{
  # Password is incorrect.
  &page_title("Password invalid");
  printf("<A HREF=\"%s\">Click here</A> to return to main menu.\n", $prefs{'main.pl'});
  &page_footer;
} else {
  # Password is correct.
  &set_readlock("$prefs{'data_path'}/$aetitle");
  $diofile = pack("H*",$dicomfile);
  if ($command eq '')
  {
    # display warning
    &page_title("Delete Worklist Entry");

    $overview=`$prefs{'readoviw'} $prefs{'data_path'}/$aetitle/$diofile`;
    ($dummy, $accession, $rest) = split(/\\/,$overview);
    $accession = &iso_to_html(pack("H*",$accession));

    printf("You have requested the deletion of the worklist entry ");
    printf("with accession no. <b>'%s'</b>.<p>\n",$accession);
    printf("<A HREF=\"%s%s/DELETE\">Delete</A> ", $prefs{'workdel.pl'}, $path_info);
    printf("or <A HREF=\"%s%s\">Cancel</A> and return to worklist overview.\n",
      $prefs{'worklist.pl'}, $path_info);
    &page_footer;
  } else {
    # erase and back to worklist overview
    &set_writelock("$prefs{'data_path'}/$aetitle");
    unlink("$prefs{'data_path'}/$aetitle/$diofile");
    printf("Location: %s/%s/%s\n\n", $prefs{'worklist.pl'}, $aetitle, $passwd);
  }
  &release_lock;
}

