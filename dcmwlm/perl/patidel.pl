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
#   This perl script allows to delete a patient entry from a storage area.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: patidel.pl,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:09  wilkens
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
require 'urldecod.ph';
require 'write.ph';
require 'checkvr.ph';

$path_info=$ENV{'PATH_INFO'};
&get_request;

$aetitle = '';
$passwd = '';
$patientid = '';
$command = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $patientid, $command, $rest) = split(/\//, $path_info);
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
    &page_title("Delete Patient");
    $decodedid = pack("H*",$patientid);
    printf("You have requested the deletion of patient ID <b>'%s'</b>.<br>\n",$decodedid);
    printf("<b>Attention:</b> Worklist entries referring to this patient will <b>not</b> be removed!<p>\n");
    printf("<A HREF=\"%s%s/DELETE\">Delete</A> ", $prefs{'patidel.pl'}, $path_info);
    printf("or <A HREF=\"%s%s\">Cancel</A> and return to patient overview.\n",
      $prefs{'patient.pl'}, $path_info);
    &page_footer;
  } else {
    # erase and back to patient overview
    &set_writelock("$prefs{'data_path'}/$aetitle");
    $decodedid = pack("H*",$patientid);
    # remove patientID key
    undef @temp;
    foreach(@PATIENT_KEYS) { if ($decodedid ne $_) { push(@temp, $_); } }
    @PATIENT_KEYS = @temp;
    # remove values
    delete $PATIENT_VALUES{"$decodedid\\name"};
    delete $PATIENT_VALUES{"$decodedid\\birthdate"};
    delete $PATIENT_VALUES{"$decodedid\\sex"};
    delete $PATIENT_VALUES{"$decodedid\\alerts"};
    delete $PATIENT_VALUES{"$decodedid\\allergies"};
    &write_environment($filename);
    printf("Location: %s/%s/%s\n\n", $prefs{'patient.pl'}, $aetitle, $passwd);
  }
  &release_lock;
}

