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
#   This perl script allows to create and update procedure step data
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:02 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: prstedit.pl,v $
#   Revision 1.2  2010-10-14 13:02:02  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:15  wilkens
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
$stepid = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $stepid, $rest) = split(/\//, $path_info);
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
      printf("Location: %s/%s/%s\n\n", $prefs{'procstep.pl'}, $aetitle, $passwd);
    } else {
      # We have received a filled-in form.
      # We save/update it and return a URL to the updated form.

      $title = &makeLO($rqpairs{'title'});
      $titleOK = (&checkLO($title))&&(length($title)>0);
      $medication = &makeLO($rqpairs{'medication'});
      $medicationOK = &checkLO($medication);
      $contrast = &makeLO($rqpairs{'contrast'});
      $contrastOK = &checkLO($contrast);
      $location = &makeSH($rqpairs{'location'});
      $locationOK = &checkSH($location);
      $comments = &makeLT($rqpairs{'comments'});
      $commentsOK = &checkLT($comments);
      if (($titleOK)&&($medicationOK)&&($contrastOK)&&($locationOK)&&($commentsOK))
      {
        &set_writelock("$prefs{'data_path'}/$aetitle");
        if ($stepid eq '')
        {
          $stepid = unpack("H*",&nextStepID($aetitle));
        }
        $decodedid = pack("H*",$stepid);
        $newentry = 1;
        foreach(@PROCEDURESTEP_KEYS) { if ($decodedid eq $_) { $newentry = 0; } }
        if ($newentry) { push(@PROCEDURESTEP_KEYS, $decodedid); }
        $PROCEDURESTEP_VALUES{"$decodedid\\procedure"} =$rqpairs{'procedure'};
        $PROCEDURESTEP_VALUES{"$decodedid\\title"} = $title;
        $PROCEDURESTEP_VALUES{"$decodedid\\modality"} = $rqpairs{'modality'};
        $PROCEDURESTEP_VALUES{"$decodedid\\medication"} = $medication;
        $PROCEDURESTEP_VALUES{"$decodedid\\contrast"} = $contrast;
        $PROCEDURESTEP_VALUES{"$decodedid\\location"} = $location;
        $PROCEDURESTEP_VALUES{"$decodedid\\comments"} = $comments;
        &write_environment($filename);
        printf("Location: %s/%s/%s\n\n", $prefs{'procstep.pl'}, $aetitle, $passwd);
      } else {
        &create_procstep_form("$prefs{'prstedit.pl'}$path_info",
          pack("H*",$stepid), $rqpairs{'procedure'},
          $title, $rqpairs{'modality'}, $medication, $contrast,
          $location, $comments, $titleOK, $medicationOK, $contrastOK,
          $locationOK, $commentsOK, 0);
      }
    }
  } else {
    # We have received a request for a form.
    if ($stepid eq '')
    {
      &create_procstep_form("$prefs{'prstedit.pl'}/$aetitle/$passwd",
        '', '', '', '', '', '', '', '', 1, 1, 1, 1, 1, 1);
    } else {
      $key = pack("H*",$stepid);
      &create_procstep_form("$prefs{'prstedit.pl'}/$aetitle/$passwd/$stepid",
        $key,
        $PROCEDURESTEP_VALUES{"$key\\procedure"},
        $PROCEDURESTEP_VALUES{"$key\\title"},
        $PROCEDURESTEP_VALUES{"$key\\modality"},
        $PROCEDURESTEP_VALUES{"$key\\medication"},
        $PROCEDURESTEP_VALUES{"$key\\contrast"},
        $PROCEDURESTEP_VALUES{"$key\\location"},
        $PROCEDURESTEP_VALUES{"$key\\comments"},
        1, 1, 1, 1, 1, 0);
    }
  }
  &release_lock;
}

#
#  void create_procstep_form(
#   0 string action,
#   1 string stepid,
#   2 string procedure,
#   3 string title,
#   4 string modality,
#   5 string medication,
#   6 string contrast,
#   7 string location,
#   8 string comments,
#   9 bool titleOK,
#  10 bool medicationOK,
#  11 bool contrastOK,
#  12 bool locationOK,
#  13 bool commentsOK,
#  14 bool isNewStep)

sub create_procstep_form
{
  &page_title("Procedure Step Form");

  if ((! @_[9])||(! @_[10])||(! @_[11])||(! @_[12])||(! @_[13]))
  {
    printf("<b>Errors</b> have been found in the data you typed in.\n");
    printf("The labels of the offending fields have been marked bold/italic.\n");
    printf("Please correct the data and press the update button.<p>\n");
  }
  printf("<FORM METHOD=get ACTION=\"%s\">\n",@_[0]);
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");
  # Procedure Step ID
  printf("<TR><TD nowrap>Procedure Step ID:</TD>\n");
  printf("<TD nowrap>%s</TD></TR>", ((@_[1] eq '') ? 'will be assigned automatically.' : @_[1]));

  # Procedure
  printf("<TR><TD nowrap>Procedure:</TD><TD nowrap>");
  printf("<select name=\"procedure\">\n");

  @keys = @PROCEDURE_KEYS;
  @values = ();
  for (@keys){ push(@values,join("\000",$PROCEDURE_VALUES{"$_\\title"},$_)); }
  for (sort(@values))
  {
    ($name, $key) = split(/\000/);
    printf("<option value=\"%s\" %s>%s\n", $key,
      (@_[2] eq $key ? 'selected' : ''), $name);
  }
  printf("</select></TD></TR>\n");

  # Modality
  printf("<TR><TD nowrap>Modality:</TD>\n<TD nowrap>");
  printf("<select name=\"modality\">\n");
  printf("<option value=\"OT\" %s>Other\n",(@_[4] eq 'OT' ? 'selected' : ''));
  printf("<option value=\"AS\" %s>Angioscopy\n",(@_[4] eq 'AS' ? 'selected' : ''));
  printf("<option value=\"BI\" %s>Biomagnetic Imaging\n",(@_[4] eq 'BI' ? 'selected' : ''));
  printf("<option value=\"CF\" %s>Cinefluorography\n",(@_[4] eq 'CF' ? 'selected' : ''));
  printf("<option value=\"CD\" %s>Color flow Doppler\n",(@_[4] eq 'CD' ? 'selected' : ''));
  printf("<option value=\"CR\" %s>Computed Radiography\n",(@_[4] eq 'CR' ? 'selected' : ''));
  printf("<option value=\"CT\" %s>Computed Tomography\n",(@_[4] eq 'CT' ? 'selected' : ''));
  printf("<option value=\"CP\" %s>Culposcopy\n",(@_[4] eq 'CP' ? 'selected' : ''));
  printf("<option value=\"CS\" %s>Cystoscopy\n",(@_[4] eq 'CS' ? 'selected' : ''));
  printf("<option value=\"DG\" %s>Diaphanography\n",(@_[4] eq 'DG' ? 'selected' : ''));
  printf("<option value=\"DF\" %s>Digital Fluoroscopy\n",(@_[4] eq 'DF' ? 'selected' : ''));
  printf("<option value=\"DM\" %s>Digital Microscopy\n",(@_[4] eq 'DM' ? 'selected' : ''));
  printf("<option value=\"DS\" %s>Digital Subtraction Angiography\n",(@_[4] eq 'DS' ? 'selected' : ''));
  printf("<option value=\"DD\" %s>Duplex Doppler\n",(@_[4] eq 'DD' ? 'selected' : ''));
  printf("<option value=\"EC\" %s>Echocardiography\n",(@_[4] eq 'EC' ? 'selected' : ''));
  printf("<option value=\"EM\" %s>Electron Microscopy\n",(@_[4] eq 'EM' ? 'selected' : ''));
  printf("<option value=\"ES\" %s>Endoscopy\n",(@_[4] eq 'ES' ? 'selected' : ''));
  printf("<option value=\"XC\" %s>External Camera\n",(@_[4] eq 'XC' ? 'selected' : ''));
  printf("<option value=\"FA\" %s>Fluorescein Angiography\n",(@_[4] eq 'FA' ? 'selected' : ''));
  printf("<option value=\"FS\" %s>Fundoscopy\n",(@_[4] eq 'FS' ? 'selected' : ''));
  printf("<option value=\"GM\" %s>Gross Microscopy\n",(@_[4] eq 'GM' ? 'selected' : ''));
  printf("<option value=\"LP\" %s>Laparoscopy\n",(@_[4] eq 'LP' ? 'selected' : ''));
  printf("<option value=\"LS\" %s>Laser Surface Scan\n",(@_[4] eq 'LS' ? 'selected' : ''));
  printf("<option value=\"MR\" %s>Magnetic Resonance\n",(@_[4] eq 'MR' ? 'selected' : ''));
  printf("<option value=\"MA\" %s>Magnetic Resonance Angiography\n",(@_[4] eq 'MA' ? 'selected' : ''));
  printf("<option value=\"MS\" %s>Magnetic Resonance Spectroscopy\n",(@_[4] eq 'MS' ? 'selected' : ''));
  printf("<option value=\"NM\" %s>Nuclear Medicine\n",(@_[4] eq 'NM' ? 'selected' : ''));
  printf("<option value=\"PT\" %s>PET\n",(@_[4] eq 'PT' ? 'selected' : ''));
  printf("<option value=\"RF\" %s>Radiofluoroscopy\n",(@_[4] eq 'RF' ? 'selected' : ''));
  printf("<option value=\"RG\" %s>Radiography\n",(@_[4] eq 'RG' ? 'selected' : ''));
  printf("<option value=\"SM\" %s>Slide Microscopy\n",(@_[4] eq 'SM' ? 'selected' : ''));
  printf("<option value=\"ST\" %s>SPECT\n",(@_[4] eq 'ST' ? 'selected' : ''));
  printf("<option value=\"TG\" %s>Thermography\n",(@_[4] eq 'TG' ? 'selected' : ''));
  printf("<option value=\"US\" %s>Ultrasound\n",(@_[4] eq 'US' ? 'selected' : ''));
  printf("<option value=\"VF\" %s>Videofluorography\n",(@_[4] eq 'VF' ? 'selected' : ''));
  printf("<option value=\"XA\" %s>X-Ray Angiography\n",(@_[4] eq 'XA' ? 'selected' : ''));
  printf("</select></TD></TR>\n");

  # Description
  printf("<TR><TD nowrap>\n");
  if (! @_[9]) { printf("<b><i>"); }
  printf("Description:");
  if (! @_[9]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"title\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n",@_[3]);

  # Medication
  printf("<TR><TD nowrap>\n");
  if (! @_[10]) { printf("<b><i>"); }
  printf("Pre-Medication:");
  if (! @_[10]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"medication\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n",@_[5]);

  # Contrast
  printf("<TR><TD nowrap>\n");
  if (! @_[11]) { printf("<b><i>"); }
  printf("Contrast Agent:");
  if (! @_[11]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"contrast\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n",@_[6]);

  # Location
  printf("<TR><TD nowrap>\n");
  if (! @_[12]) { printf("<b><i>"); }
  printf("Location:");
  if (! @_[12]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"location\" maxlength=16 size=16 value=\"%s\"></TD></TR>\n",@_[7]);
  printf("</TABLE><P>\n");

  # Comments
  if (! @_[13]) { printf("<b><i>"); }
  printf("Comments:<br>");
  if (! @_[13]) { printf("</i></b>"); }
  printf("<textarea align=bottom name=\"comments\" rows=6 cols=64>%s</textarea><p>\n",@_[8]);

  if (! @_[14])
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
