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
#   This perl script allows to delete a station entry from a storage area.
#
#

require 'prefs.ph';
require 'layout.ph';
require 'password.ph';
require 'lock.ph';
require 'urldecod.ph';
require 'write.ph';
require 'checkvr.ph';

$path_info=$ENV{'PATH_INFO'};
&get_request;

$aetitle = '';
$passwd = '';
$stationid = '';
$command = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $stationid, $command, $rest) = split(/\//, $path_info);
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
  $filename = "$prefs{'data_path'}/$aetitle/$prefs{'datafile'}";
  &read_environment($filename);
  if ($command eq '')
  {
    # display warning
    &page_title("Delete Station");
    $decodedid = pack("H*",$stationid);
    printf("You have requested the deletion of station AE title <b>'%s'</b>.<br>\n",$decodedid);
    printf("<b>Attention:</b> Worklist entries referring to this station will <b>not</b> be removed!<p>\n");
    printf("<A HREF=\"%s%s/DELETE\">Delete</A> ", $prefs{'statdel.pl'}, $path_info);
    printf("or <A HREF=\"%s%s\">Cancel</A> and return to patient overview.\n",
      $prefs{'station.pl'}, $path_info);
    &page_footer;
  } else {
    # erase and back to patient overview
    &set_writelock("$prefs{'data_path'}/$aetitle");
    $decodedid = pack("H*",$stationid);
    # remove station key
    undef @temp;
    foreach(@STATION_KEYS) { if ($decodedid ne $_) { push(@temp, $_); } }
    @STATION_KEYS = @temp;
    # remove values
    delete $STATION_VALUES{"$decodedid\\name"};
    delete $STATION_VALUES{"$decodedid\\modality"};
    &write_environment($filename);
    printf("Location: %s/%s/%s\n\n", $prefs{'station.pl'}, $aetitle, $passwd);
  }
  &release_lock;
}

