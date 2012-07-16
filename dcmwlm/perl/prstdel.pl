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
$stepid = '';
$command = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $stepid, $command, $rest) = split(/\//, $path_info);
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
    &page_title("Delete Procedure Step");
    $decodedid = pack("H*",$stepid);
    printf("You have requested the deletion of procedure step ID <b>'%s'</b>.<br>\n",$decodedid);
    printf("<b>Attention:</b> Worklist entries referring to this procedure will <b>not</b> be removed!<p>\n");
    printf("<A HREF=\"%s%s/DELETE\">Delete</A> ", $prefs{'prstdel.pl'}, $path_info);
    printf("or <A HREF=\"%s%s\">Cancel</A> and return to procedure overview.\n",
      $prefs{'procstep.pl'}, $path_info);
    &page_footer;
  } else {
    # erase and back to procedure overview
    &set_writelock("$prefs{'data_path'}/$aetitle");
    $decodedid = pack("H*",$stepid);
    # remove procedure key
    undef @temp;
    foreach(@PROCEDURESTEP_KEYS) { if ($decodedid ne $_) { push(@temp, $_); } }
    @PROCEDURESTEP_KEYS = @temp;
    # remove values
    delete $PROCEDURESTEP_VALUES{"$decodedid\\procedure"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\title"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\modality"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\medication"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\contrast"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\location"};
    delete $PROCEDURESTEP_VALUES{"$decodedid\\comments"};

    &write_environment($filename);
    printf("Location: %s/%s/%s\n\n", $prefs{'procstep.pl'}, $aetitle, $passwd);
  }
  &release_lock;
}

