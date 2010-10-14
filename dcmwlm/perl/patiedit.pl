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
#   This perl script allows to create and update patient data
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: patiedit.pl,v $
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
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $patientid, $rest) = split(/\//, $path_info);
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
  if ($request ne '')
  {
    if ($rqpairs{'action'} eq 'Cancel')
    {
      printf("Location: %s/%s/%s\n\n", $prefs{'patient.pl'}, $aetitle, $passwd);
    } else {
      # We have received a filled-in form.
      # We save/update it and return a URL to the updated form.
      $birthdate = &makeDA($rqpairs{'birthdate'});
      $name = join('^',$rqpairs{'familyname'}, $rqpairs{'givenname'});
      $name = &makePN($name);
      $medicalalerts = &makeLON($rqpairs{'medicalalerts'});
      $contrastallergies = &makeLON($rqpairs{'contrastallergies'});
      if ((&checkPN($name))&&(&checkDA($birthdate))&&
          (&checkLON($medicalalerts))&&(&checkLON($contrastallergies)))
      {
        &set_writelock("$prefs{'data_path'}/$aetitle");
        if ($patientid eq '')
        {
          $patientid = unpack("H*",&nextPID($aetitle));
        }
        $decodedid = pack("H*",$patientid);
        $newentry = 1;
        foreach(@PATIENT_KEYS) { if ($decodedid eq $_) { $newentry = 0; } }
        if ($newentry) { push(@PATIENT_KEYS, $decodedid); }
        $PATIENT_VALUES{"$decodedid\\name"} = $name;
        $PATIENT_VALUES{"$decodedid\\birthdate"} = $birthdate;
        $PATIENT_VALUES{"$decodedid\\sex"} = $rqpairs{'sex'};
        $PATIENT_VALUES{"$decodedid\\alerts"} = $medicalalerts;
        $PATIENT_VALUES{"$decodedid\\allergies"} = $contrastallergies;
        &write_environment($filename);
        printf("Location: %s/%s/%s\n\n", $prefs{'patient.pl'}, $aetitle, $passwd);
      } else {
        &create_patient_form("$prefs{'patiedit.pl'}$path_info",
          pack("H*",$patientid), $rqpairs{'familyname'}, $rqpairs{'givenname'},
          $birthdate, $rqpairs{'sex'},
          $medicalalerts, $contrastallergies,
          &checkPN($name), &checkPN($name),
          &checkDA($birthdate), &checkLON($medicalalerts),
          &checkLON($contrastallergies), 0);
      }
    }
  } else {
    # We have received a request for a form.
    if ($patientid eq '')
    {
      &create_patient_form("$prefs{'patiedit.pl'}/$aetitle/$passwd",
        '', '', '', '', '', '', '', 1, 1, 1, 1, 1, 1);
    } else {
      $key = pack("H*",$patientid);
      ($familyname, $givenname) = split(/\^/,$PATIENT_VALUES{"$key\\name"},2);
      &create_patient_form("$prefs{'patiedit.pl'}/$aetitle/$passwd/$patientid",
        $key, $familyname, $givenname,
        $PATIENT_VALUES{"$key\\birthdate"},
        $PATIENT_VALUES{"$key\\sex"},
        $PATIENT_VALUES{"$key\\alerts"},
        $PATIENT_VALUES{"$key\\allergies"},
        1, 1, 1, 1, 1, 0);
    }
  }
  &release_lock;
}

#
#  void create_patient_form(
#   0 string action,
#   1 string patientid,
#   2 string familyname,
#   3 string givenname,
#   4 string birthdate,
#   5 string sex,
#   6 string medicalalerts,
#   7 string contrastallergies
#   8 bool familynameOK,
#   9 bool givennameOK,
#  10 bool birthdateOK,
#  11 bool medicalalertsOK,
#  12 bool contrastallergiesOK
#  13 bool isNewPatient)
#
sub create_patient_form
{
  &page_title("Patient Form");

  if ((! @_[8])||(! @_[9])||(! @_[10])||(! @_[11])||(! @_[12]))
  {
    printf("<b>Errors</b> have been found in the data you typed in.\n");
    printf("The labels of the offending fields have been marked bold/italic.\n");
    printf("Please correct the data and press the update button.<p>\n");
  }
  printf("<FORM METHOD=get ACTION=\"%s\">\n",@_[0]);
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");
  printf("<TR><TD nowrap>Patient ID:</TD>\n");
  printf("<TD nowrap>%s<TR><TD nowrap>", ((@_[1] eq '') ? 'will be assigned automatically.' : @_[1]));
  if (! @_[8]) { printf("<b><i>"); }
  printf("Family Name:");
  if (! @_[8]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"familyname\" maxlength=48 size=48 value=\"%s\"></TD></TR>\n",@_[2]);

  printf("<TR><TD nowrap>");
  if (! @_[9]) { printf("<b><i>"); }
  printf("Given Name(s):");
  if (! @_[9]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"givenname\" maxlength=48 size=48 value=\"%s\"></TD></TR>\n",@_[3]);

  printf("<TR><TD nowrap>");
  if (! @_[10]) { printf("<b><i>"); }
  printf("Birth date:");
  if (! @_[10]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"birthdate\" maxlength=10 size=8 value=\"%s\"></TD></TR>\n",@_[4]);

  printf("<TR><TD nowrap>Sex:</TD><TD nowrap>");
  printf("<select name=\"sex\">\n");
  printf("<option value=\"\" %s>(none)\n", (@_[5] eq ''  ? 'selected' : ''));
  printf("<option value=\"F\" %s>female\n",(@_[5] eq 'F' ? 'selected' : ''));
  printf("<option value=\"M\" %s>male\n",  (@_[5] eq 'M' ? 'selected' : ''));
  printf("<option value=\"O\" %s>other\n", (@_[5] eq 'O' ? 'selected' : ''));
  printf("</select></TD></TR></TABLE><P>\n");

  if (! @_[11]) { printf("<b><i>"); }
  printf("Medical Alerts:<br>\n");
  if (! @_[11]) { printf("</i></b>"); }
  printf("<textarea name=\"medicalalerts\" rows=6 cols=64>%s</textarea><p>\n",@_[6]);
  if (! @_[12]) { printf("<b><i>"); }
  printf("Contrast Allergies:<br>\n");
  if (! @_[12]) { printf("</i></b>"); }
  printf("<textarea name=\"contrastallergies\" rows=6 cols=64>%s</textarea><p>\n",@_[7]);
  if (! @_[13])
  {
    printf("<P><INPUT TYPE=SUBMIT name=\"action\" value=\"Update\">\n");
    printf("<INPUT TYPE=RESET name=\"action\" value=\"Reset Form\">\n");
    printf("<INPUT TYPE=SUBMIT name=\"action\" value=\"Cancel\">\n");
  } else {
    printf("<P><INPUT TYPE=SUBMIT name=\"action\" value=\"Save\">\n");
    printf("<INPUT TYPE=RESET name=\"action\" value=\"Reset Form\">\n");
    printf("<INPUT TYPE=SUBMIT name=\"action\" value=\"Cancel\">\n");
  }
  printf("</FORM>\n");
  &page_footer;
}

