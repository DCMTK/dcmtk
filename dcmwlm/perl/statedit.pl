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
#   This perl script allows to create and update station data
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:02 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: statedit.pl,v $
#   Revision 1.2  2010-10-14 13:02:02  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:16  wilkens
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
$stationid = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $stationid, $rest) = split(/\//, $path_info);
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
      printf("Location: %s/%s/%s\n\n", $prefs{'station.pl'}, $aetitle, $passwd);
    } else {
      # We have received a filled-in form.
      # We save/update it and return a URL to the updated form.

      $name = &makeSH($rqpairs{'name'});
      $nameOK = &checkSH($name);
      if ($stationid eq '')
      {
        $id = &makeAE($rqpairs{'stationid'});
        $idOK = &checkAE($id);
        $idProtected = 0;
        $idUnique = 1;
        foreach(@STATION_KEYS) { if ($id eq $_) { $idUnique = 0; } }
      } else {
        $id = pack("H*",$stationid);
        $idOK = 1;
        $idProtected = 1;
        $idUnique = 1;
      }
      if (($idOK)&&($idUnique)&&($nameOK))
      {
        &set_writelock("$prefs{'data_path'}/$aetitle");
        $newentry = 1;
        foreach(@STATION_KEYS) { if ($id eq $_) { $newentry = 0; } }
        if ($newentry) { push(@STATION_KEYS, $id); }
        $STATION_VALUES{"$id\\name"} = $name;
        $STATION_VALUES{"$id\\modality"} = $rqpairs{'modality'};
        &write_environment($filename);
        printf("Location: %s/%s/%s\n\n", $prefs{'station.pl'}, $aetitle, $passwd);
      } else {
        &create_station_form("$prefs{'statedit.pl'}$path_info",
          $id, $name, $rqpairs{'modality'},
          $idOK, $idUnique, $idProtected, $nameOK, 0);
      }
    }
  } else {
    # We have received a request for a form.
    if ($stationid eq '')
    {
      &create_station_form("$prefs{'statedit.pl'}/$aetitle/$passwd",
        '', '', '', 1, 1, 0, 1, 1);
    } else {
      $key = pack("H*",$stationid);
      &create_station_form("$prefs{'statedit.pl'}/$aetitle/$passwd/$stationid",
        $key,
        $STATION_VALUES{"$key\\name"},
        $STATION_VALUES{"$key\\modality"},
        1, 1, 1, 1, 0);
    }
  }
  &release_lock;
}

#
#  void create_station_form(
#   0 string action,
#   1 string stationid (ae title),
#   2 string name,
#   3 string modality,
#   4 bool stationidOK,
#   5 bool stationidUnique,
#   6 bool stationidProtected,
#   7 bool nameOK,
#   8 bool isNewStation)
#
sub create_station_form
{
  &page_title("Station Form");

  if ((! @_[4])||(! @_[5])||(! @_[7]))
  {
    printf("<b>Errors</b> have been found in the data you typed in.\n");
    printf("The labels of the offending fields have been marked bold/italic.\n");
    printf("Please correct the data and press the update button.<p>\n");
  }
  printf("<FORM METHOD=get ACTION=\"%s\">\n",@_[0]);
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n<TR><TD nowrap>");
  if ((! @_[4])||(! @_[5])) { printf("<b><i>"); }
  printf("Station AE Title:\n");
  if ((! @_[4])||(! @_[5])) { printf("</i></b>"); }
  if (! @_[6])
  {
    printf("</TD><TD nowrap><input name=\"stationid\" maxlength=16 size=16 value=\"%s\">\n",@_[1]);
  } else {
    printf("</TD><TD nowrap>%s\n",@_[1]);
  }
  if (! @_[5]) { printf(" (AE Title already in use!)\n"); }
  printf("</TD></TR><TR><TD nowrap>");
  if (! @_[7]) { printf("<b><i>"); }
  printf("Station Name:");
  if (! @_[7]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"name\" maxlength=16 size=16 value=\"%s\"></TD></TR>\n",@_[2]);

  printf("</TR><TR><TD nowrap>Modality:</TD>\n<TD nowrap>");
  printf("<select name=\"modality\">\n");
  printf("<option value=\"OT\" %s>Other\n",(@_[3] eq 'OT' ? 'selected' : ''));
  printf("<option value=\"AS\" %s>Angioscopy\n",(@_[3] eq 'AS' ? 'selected' : ''));
  printf("<option value=\"BI\" %s>Biomagnetic Imaging\n",(@_[3] eq 'BI' ? 'selected' : ''));
  printf("<option value=\"CF\" %s>Cinefluorography\n",(@_[3] eq 'CF' ? 'selected' : ''));
  printf("<option value=\"CD\" %s>Color flow Doppler\n",(@_[3] eq 'CD' ? 'selected' : ''));
  printf("<option value=\"CR\" %s>Computed Radiography\n",(@_[3] eq 'CR' ? 'selected' : ''));
  printf("<option value=\"CT\" %s>Computed Tomography\n",(@_[3] eq 'CT' ? 'selected' : ''));
  printf("<option value=\"CP\" %s>Culposcopy\n",(@_[3] eq 'CP' ? 'selected' : ''));
  printf("<option value=\"CS\" %s>Cystoscopy\n",(@_[3] eq 'CS' ? 'selected' : ''));
  printf("<option value=\"DG\" %s>Diaphanography\n",(@_[3] eq 'DG' ? 'selected' : ''));
  printf("<option value=\"DF\" %s>Digital Fluoroscopy\n",(@_[3] eq 'DF' ? 'selected' : ''));
  printf("<option value=\"DM\" %s>Digital Microscopy\n",(@_[3] eq 'DM' ? 'selected' : ''));
  printf("<option value=\"DS\" %s>Digital Subtraction Angiography\n",(@_[3] eq 'DS' ? 'selected' : ''));
  printf("<option value=\"DD\" %s>Duplex Doppler\n",(@_[3] eq 'DD' ? 'selected' : ''));
  printf("<option value=\"EC\" %s>Echocardiography\n",(@_[3] eq 'EC' ? 'selected' : ''));
  printf("<option value=\"EM\" %s>Electron Microscopy\n",(@_[3] eq 'EM' ? 'selected' : ''));
  printf("<option value=\"ES\" %s>Endoscopy\n",(@_[3] eq 'ES' ? 'selected' : ''));
  printf("<option value=\"XC\" %s>External Camera\n",(@_[3] eq 'XC' ? 'selected' : ''));
  printf("<option value=\"FA\" %s>Fluorescein Angiography\n",(@_[3] eq 'FA' ? 'selected' : ''));
  printf("<option value=\"FS\" %s>Fundoscopy\n",(@_[3] eq 'FS' ? 'selected' : ''));
  printf("<option value=\"GM\" %s>Gross Microscopy\n",(@_[3] eq 'GM' ? 'selected' : ''));
  printf("<option value=\"LP\" %s>Laparoscopy\n",(@_[3] eq 'LP' ? 'selected' : ''));
  printf("<option value=\"LS\" %s>Laser Surface Scan\n",(@_[3] eq 'LS' ? 'selected' : ''));
  printf("<option value=\"MR\" %s>Magnetic Resonance\n",(@_[3] eq 'MR' ? 'selected' : ''));
  printf("<option value=\"MA\" %s>Magnetic Resonance Angiography\n",(@_[3] eq 'MA' ? 'selected' : ''));
  printf("<option value=\"MS\" %s>Magnetic Resonance Spectroscopy\n",(@_[3] eq 'MS' ? 'selected' : ''));
  printf("<option value=\"NM\" %s>Nuclear Medicine\n",(@_[3] eq 'NM' ? 'selected' : ''));
  printf("<option value=\"PT\" %s>PET\n",(@_[3] eq 'PT' ? 'selected' : ''));
  printf("<option value=\"RF\" %s>Radiofluoroscopy\n",(@_[3] eq 'RF' ? 'selected' : ''));
  printf("<option value=\"RG\" %s>Radiography\n",(@_[3] eq 'RG' ? 'selected' : ''));
  printf("<option value=\"SM\" %s>Slide Microscopy\n",(@_[3] eq 'SM' ? 'selected' : ''));
  printf("<option value=\"ST\" %s>SPECT\n",(@_[3] eq 'ST' ? 'selected' : ''));
  printf("<option value=\"TG\" %s>Thermography\n",(@_[3] eq 'TG' ? 'selected' : ''));
  printf("<option value=\"US\" %s>Ultrasound\n",(@_[3] eq 'US' ? 'selected' : ''));
  printf("<option value=\"VF\" %s>Videofluorography\n",(@_[3] eq 'VF' ? 'selected' : ''));
  printf("<option value=\"XA\" %s>X-Ray Angiography\n",(@_[3] eq 'XA' ? 'selected' : ''));
  printf("</select></TD></TR></TABLE><P>\n");
  if (! @_[8])
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

