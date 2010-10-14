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
#   This perl script allows to create and update worklist entries
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:02 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: workedit.pl,v $
#   Revision 1.2  2010-10-14 13:02:02  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:19  wilkens
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
require 'isocode.ph';

$path_info=$ENV{'PATH_INFO'};
&get_request;

$aetitle = '';
$passwd = '';
$dicomfile = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $dicomfile, $rest) = split(/\//, $path_info);
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
    # We have received a filled-in form.
    if ($rqpairs{'action'} eq 'Create')
    {
      # Create new form from known procedure and patient IDs.
      $patientID = pack("H*",$rqpairs{'patient'});
      $procedurestepID = pack("H*",$rqpairs{'procedurestep'});
      $procedureID = $PROCEDURESTEP_VALUES{"$procedurestepID\\procedure"};
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
      $datenow = sprintf("%04d%02d%02d", $year+1900, $mon+1, $mday);
      $timenow = sprintf("%02d%02d", $hour, $min);

      local($temp_modality) = $PROCEDURESTEP_VALUES{"$procedurestepID\\modality"};
      local($scheduled_station_AEtitle) = '';
      local($scheduled_station_name) = '';
      local($temp_name);
      for (@STATION_KEYS)
      {
        if ($STATION_VALUES{"$_\\modality"} eq $temp_modality)
        {
          if ($scheduled_station_AEtitle ne '') { $scheduled_station_AEtitle = "$scheduled_station_AEtitle\\"; }
          $scheduled_station_AEtitle = "$scheduled_station_AEtitle$_";
          $temp_name = $STATION_VALUES{"$_\\name"};
          if ($scheduled_station_name ne '') { $scheduled_station_name = "$scheduled_station_name\\"; }
          $scheduled_station_name = "$scheduled_station_name$temp_name";
        }
      }

      &create_worklist_form(
        "$prefs{'workedit.pl'}$path_info",
        $procedureID,
        $PROCEDURE_VALUES{"$procedureID\\description"},
        '', # Study UID
        $PROCEDURE_VALUES{"$procedureID\\priority"},
        '', # Accession No.
        '', # Req. Physician
        $PATIENT_VALUES{"$patientID\\name"},
        $patientID,
        $PATIENT_VALUES{"$patientID\\birthdate"},
        $PATIENT_VALUES{"$patientID\\sex"},
        $PATIENT_VALUES{"$patientID\\alerts"},
        $PATIENT_VALUES{"$patientID\\allergies"},
        1, 1, 1, 1, 1, 1,
        $scheduled_station_AEtitle,
        $datenow,
        $timenow,
        $PROCEDURESTEP_VALUES{"$procedurestepID\\modality"},
        '',
        $PROCEDURESTEP_VALUES{"$procedurestepID\\title"},
        $scheduled_station_name,
        $PROCEDURESTEP_VALUES{"$procedurestepID\\location"},
        $PROCEDURESTEP_VALUES{"$procedurestepID\\medication"},
        $procedurestepID,
        $PROCEDURESTEP_VALUES{"$procedurestepID\\contrast"},
        $PROCEDURESTEP_VALUES{"$procedurestepID\\comments"},
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    }
    elsif ($rqpairs{'action'} eq 'Cancel')
    {
      printf("Location: %s/%s/%s\n\n", $prefs{'worklist.pl'}, $aetitle, $passwd);
    } else {
      # Action = Save or Update.

      $scheduled_station_AEtitle = '';
      $scheduled_station_name = '';
      local($key);
      foreach (@STATION_KEYS)
      {
        $key = unpack("H*",$_);
        if ($rqpairs{"ae_$key"} eq "yes")
        {
          if (length($scheduled_station_AEtitle)==0)
          {
            $scheduled_station_AEtitle = $_;
            $scheduled_station_name = $STATION_VALUES{"$_\\name"};
          } else {
            $scheduled_station_AEtitle = join("\\", $scheduled_station_AEtitle, $_);
            $scheduled_station_name = join("\\", $scheduled_station_name, $STATION_VALUES{"$_\\name"});
          }
        }
      }

      $requested_procedure_ID = &makeSH($rqpairs{'requested_procedure_ID'});
      $requested_procedure_description = &makeLO($rqpairs{'requested_procedure_description'});
      $study_instance_UID = $rqpairs{'study_instance_UID'};
      $requested_procedure_Priority = &makeSH($rqpairs{'requested_procedure_priority'});
      $accession_no = &makeSH($rqpairs{'accession_no'});
      $requesting_physician = &makePN($rqpairs{'requesting_physician'});
      $patients_name = &makePN($rqpairs{'patients_name'});
      $patient_ID = &makeLO($rqpairs{'patient_ID'});
      $patients_birthdate = &makeDA($rqpairs{'patients_birthdate'});
      $patients_sex = $rqpairs{'patients_sex'};
      $medical_alerts = &makeLON($rqpairs{'medical_alerts'});
      $medical_alerts =~ s/\015\012/\\/g;
      $contrast_allergies = &makeLON($rqpairs{'contrast_allergies'});
      $contrast_allergies =~ s/\015\012/\\/g;

      $scheduled_procedure_step_start_date = &makeDA($rqpairs{'scheduled_procedure_step_start_date'});
      $scheduled_procedure_step_start_time = &makeTM($rqpairs{'scheduled_procedure_step_start_time'});
      $modality = $rqpairs{'modality'};
      $scheduled_performing_physicians_name = &makePN($rqpairs{'scheduled_performing_physicians_name'});
      $scheduled_procedure_step_description = &makeLO($rqpairs{'scheduled_procedure_step_description'});
      $scheduled_procedure_step_location = &makeSH($rqpairs{'scheduled_procedure_step_location'});
      $pre_medication = &makeLO($rqpairs{'pre_medication'});
      $scheduled_procedure_step_ID = &makeSH($rqpairs{'scheduled_procedure_step_ID'});
      $requested_contrast_agent = &makeLO($rqpairs{'requested_contrast_agent'});
      $scheduled_procedure_step_comments = &makeLT($rqpairs{'scheduled_procedure_step_comments'});

      $scheduled_station_AEtitle_OK = (length($scheduled_station_AEtitle) >0);
      $requested_procedure_description_OK = &checkLO($requested_procedure_description);
      $requesting_physician_OK = (length($requesting_physician)==0) ||
        &checkPN($requesting_physician);
      $patients_name_OK = &checkPN($patients_name);
      $patients_birthdate_OK = (length($patients_birthdate)==0) ||
        &checkDA($patients_birthdate);
      $medical_alerts_OK = &checkLON($medical_alerts);
      $contrast_allergies_OK = &checkLON($contrast_allergies);

      $scheduled_procedure_step_start_date_OK = &checkDA($scheduled_procedure_step_start_date);
      $scheduled_procedure_step_start_time_OK = &checkTM($scheduled_procedure_step_start_time);
      $scheduled_performing_physicians_name_OK = (length($scheduled_performing_physicians_name)==0) ||
        &checkPN($scheduled_performing_physicians_name);
      $scheduled_procedure_step_description_OK = &checkLO($scheduled_procedure_step_description);
      $scheduled_procedure_step_location_OK = (length($scheduled_procedure_step_location)==0) ||
        &checkSH($scheduled_procedure_step_location);
      $pre_medication_OK = &checkLO($pre_medication);
      $scheduled_procedure_step_ID_OK = &checkSH($scheduled_procedure_step_ID);
      $requested_contrast_agent_OK = &checkLO($requested_contrast_agent);
      $scheduled_procedure_step_comments_OK = &checkLT($scheduled_procedure_step_comments);

      if ($requested_procedure_description_OK
        && $requesting_physician_OK
        && $patients_name_OK
        && $patients_birthdate_OK
        && $medical_alerts_OK
        && $contrast_allergies_OK
        && $scheduled_station_AEtitle_OK
        && $scheduled_procedure_step_start_date_OK
        && $scheduled_procedure_step_start_time_OK
        && $scheduled_performing_physicians_name_OK
        && $scheduled_procedure_step_description_OK
        && $scheduled_procedure_step_location_OK
        && $pre_medication_OK
        && $scheduled_procedure_step_ID_OK
        && $requested_contrast_agent_OK
        && $scheduled_procedure_step_comments_OK)
      {
        # Everything is OK. Now (re)create DICOM file.
        if ($dicomfile eq '')
        {
          $dicomfilename = join('.',&nextFilenamePrefix($aetitle), $prefs{'dicom_ext'});
        } else {
          $dicomfilename = pack("H*",$dicomfile);
        }
        $temppath = "$prefs{'data_path'}/$aetitle/tempfile.tmp";

        if ($accession_no eq '') { $accession_no = &nextAccNr($aetitle); }
        if ($study_instance_UID eq '') { $study_instance_UID = &nextUID($aetitle); }
        &set_writelock("$prefs{'data_path'}/$aetitle");
        &write_environment($filename);

        # Write DICOM data to temporary file
        if (open(TMPFILE,">$temppath"))
        {
          printf(TMPFILE "\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s",
            unpack("H*", &html_to_iso($requested_procedure_ID)),
            unpack("H*", &html_to_iso($requested_procedure_description)),
            unpack("H*", &html_to_iso($study_instance_UID)),
            unpack("H*", &html_to_iso($requested_procedure_Priority)),
            unpack("H*", &html_to_iso($accession_no)),
            unpack("H*", &html_to_iso($requesting_physician)),
            unpack("H*", &html_to_iso($patients_name)),
            unpack("H*", &html_to_iso($patient_ID)),
            unpack("H*", &html_to_iso($patients_birthdate)),
            unpack("H*", &html_to_iso($patients_sex)),
            unpack("H*", &html_to_iso($medical_alerts)),
            unpack("H*", &html_to_iso($contrast_allergies)));
          printf(TMPFILE "\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\\n",
            unpack("H*", &html_to_iso($scheduled_station_AEtitle)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_start_date)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_start_time)),
            unpack("H*", &html_to_iso($modality)),
            unpack("H*", &html_to_iso($scheduled_performing_physicians_name)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_description)),
            unpack("H*", &html_to_iso($scheduled_station_name)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_location)),
            unpack("H*", &html_to_iso($pre_medication)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_ID)),
            unpack("H*", &html_to_iso($requested_contrast_agent)),
            unpack("H*", &html_to_iso($scheduled_procedure_step_comments)));
          close(TMPFILE);
          `$prefs{'writwlst'} $temppath $prefs{'data_path'}/$aetitle/$dicomfilename`;
          unlink($temppath);
        } else {
          &printlog("workedit.pl: cannot create temporary file for DICOM data");
        }
        printf("Location: %s/%s/%s\n\n", $prefs{'worklist.pl'}, $aetitle, $passwd);
      } else {
        # Found Errors.
        &create_worklist_form(
          "$prefs{'workedit.pl'}$path_info",
          $requested_procedure_ID,
          $requested_procedure_description,
          $study_instance_UID,
          $requested_procedure_Priority,
          $accession_no,
          $requesting_physician,
          $patients_name,
          $patient_ID,
          $patients_birthdate,
          $patients_sex,
          $medical_alerts,
          $contrast_allergies,
          $requested_procedure_description_OK,
          $requesting_physician_OK,
          $patients_name_OK,
          $patients_birthdate_OK,
          $medical_alerts_OK,
          $contrast_allergies_OK,
          $scheduled_station_AEtitle,
          $scheduled_procedure_step_start_date,
          $scheduled_procedure_step_start_time,
          $modality,
          $scheduled_performing_physicians_name,
          $scheduled_procedure_step_description,
          $scheduled_station_name,
          $scheduled_procedure_step_location,
          $pre_medication,
          $scheduled_procedure_step_ID,
          $requested_contrast_agent,
          $scheduled_procedure_step_comments,
          $scheduled_station_AEtitle_OK,
          $scheduled_procedure_step_start_date_OK,
          $scheduled_procedure_step_start_time_OK,
          $scheduled_performing_physicians_name_OK,
          $scheduled_procedure_step_description_OK,
          $scheduled_procedure_step_location_OK,
          $pre_medication_OK,
          $scheduled_procedure_step_ID_OK,
          $requested_contrast_agent_OK,
          $scheduled_procedure_step_comments_OK,
          0);
      }
    }
  } else {
    # We have received a request for a form.
    if ($dicomfile eq '')
    {
      # Create new worklist entry
      &create_new_form("$prefs{'workedit.pl'}/$aetitle/$passwd");
    } else {
      # Read DICOM file and display editor.
      $dicomfile = pack("H*",$dicomfile);
      $encodeddata=`$prefs{'readwlst'} $prefs{'data_path'}/$aetitle/$dicomfile`;
      ($dummy, $encodeddata) = split(/\\/,$encodeddata, 2);
      @arglist = ();
      for (1..10) # Argument 1-10 can be copied directly
      {
        ($value, $encodeddata) = split(/\\/,$encodeddata, 2);
        push(@arglist, &iso_to_html(pack("H*",$value)));
      }
      # medical_alerts is a multiline field
      ($value, $encodeddata) = split(/\\/,$encodeddata, 2);
      $dummy = pack("H*",$value);
      $dummy =~ s/\\/\015\012/g;
      push(@arglist, &iso_to_html($dummy));
      # contrast_allergies is a multiline field
      ($value, $encodeddata) = split(/\\/,$encodeddata, 2);
      $dummy = pack("H*",$value);
      $dummy =~ s/\\/\015\012/g;
      push(@arglist, &iso_to_html($dummy));

      for (1..6) { push(@arglist,1); }

      # Attention: AETitles must not be converted to HTML format!
      ($value, $encodeddata) = split(/\\/,$encodeddata, 2);
      push(@arglist, pack("H*",$value));

      for (1..11) # Argument 20-30 can also be copied directly
      {
        ($value, $encodeddata) = split(/\\/,$encodeddata, 2);
        push(@arglist, &iso_to_html(pack("H*",$value)));
      }
      for (1..10) { push(@arglist,1); }
      &create_worklist_form(
        "$prefs{'workedit.pl'}$path_info", @arglist, 0);
    }
  }
  &release_lock;
}


# void create_new_form(string action)
#
sub create_new_form
{
  &page_title("Create New Worklist Form");
  printf("<FORM METHOD=get ACTION=\"%s\">\n",@_[0]);
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");

  printf("<TR><TD nowrap>Patient:</TD><TD nowrap>");
  printf("<select name=\"patient\">\n");

  @keys = @PATIENT_KEYS;
  @values = ();
  for (@keys){ push(@values,join("\000",$PATIENT_VALUES{"$_\\name"},$_)); }
  for (sort(@values))
  {
    ($name, $key) = split(/\000/);
    printf("<option value=\"%s\">%s\n", unpack("H*",$key), $name);
  }
  printf("</select></TD></TR><P>\n");

  printf("<TR><TD nowrap>Procedure Step:</TD><TD nowrap>");
  printf("<select name=\"procedurestep\">\n");

  @keys = @PROCEDURESTEP_KEYS;
  @values = ();
  for (@keys){ push(@values,join("\000",$PROCEDURESTEP_VALUES{"$_\\title"},$_)); }
  for (sort(@values))
  {
    ($name, $key) = split(/\000/);
    printf("<option value=\"%s\">%s\n", unpack("H*",$key), $name);
  }
  printf("</select></TD></TR></TABLE><P>\n");
  printf("<P><INPUT TYPE=SUBMIT name=\"action\" value=\"Create\">\n");
  printf("<INPUT TYPE=RESET value=\"Reset Form\"></FORM>\n");
  &page_footer;
}


#
#  void create_worklist_form(
#
#   0 string action,
#   1 string requested_procedure_ID
#   2 string requested_procedure_description
#   3 string study_instance_UID
#   4 string requested_procedure_Priority
#   5 string accession_no
#   6 string requesting_physician
#   7 string patients_name
#   8 string patient_ID
#   9 string patients_birthdate
#  10 string patients_sex
#  11 string medical_alerts
#  12 string contrast_allergies
#  13 bool requested_procedure_description_OK
#  14 bool requesting_physician_OK
#  15 bool patients_name_OK
#  16 bool patients_birthdate_OK
#  17 bool medical_alerts_OK
#  18 bool contrast_allergies_OK
#  19 string scheduled_station_AEtitle
#  20 string scheduled_procedure_step_start_date
#  21 string scheduled_procedure_step_start_time
#  22 string modality
#  23 string scheduled_performing_physicians_name
#  24 string scheduled_procedure_step_description
#  25 string scheduled_station_name
#  26 string scheduled_procedure_step_location
#  27 string pre_medication
#  28 string scheduled_procedure_step_ID
#  29 string requested_contrast_agent
#  30 string scheduled_procedure_step_comments
#  31 bool scheduled_station_AEtitle_OK
#  32 bool scheduled_procedure_step_start_date_OK
#  33 bool scheduled_procedure_step_start_time_OK
#  34 bool scheduled_performing_physicians_name_OK
#  35 bool scheduled_procedure_step_description_OK
#  36 bool scheduled_procedure_step_location_OK
#  37 bool pre_medication_OK
#  38 bool scheduled_procedure_step_ID_OK
#  39 bool requested_contrast_agent_OK
#  40 bool scheduled_procedure_step_comments_OK
#  41 int isNewWorklist
#
sub create_worklist_form
{
  &page_title("Worklist Form");

  local($arg_action) = @_[0];
  local($arg_requested_procedure_ID) = @_[1];
  local($arg_requested_procedure_description) = @_[2];
  local($arg_study_instance_UID) = @_[3];
  local($arg_requested_procedure_priority) = @_[4];
  local($arg_accession_no) = @_[5];
  local($arg_requesting_physician) = @_[6];
  local($arg_patients_name) = @_[7];
  local($arg_patient_ID) = @_[8];
  local($arg_patients_birthdate) = @_[9];
  local($arg_patients_sex) = @_[10];
  local($arg_medical_alerts) = @_[11];
  local($arg_contrast_allergies) = @_[12];
  local($arg_requested_procedure_description_OK) = @_[13];
  local($arg_requesting_physician_OK) = @_[14];
  local($arg_patients_name_OK) = @_[15];
  local($arg_patients_birthdate_OK) = @_[16];
  local($arg_medical_alerts_OK) = @_[17];
  local($arg_contrast_allergies_OK) = @_[18];
  local($arg_scheduled_station_AEtitle) = @_[19];
  local($arg_scheduled_procedure_step_start_date) = @_[20];
  local($arg_scheduled_procedure_step_start_time) = @_[21];
  local($arg_modality) = @_[22];
  local($arg_scheduled_performing_physicians_name) = @_[23];
  local($arg_scheduled_procedure_step_description) = @_[24];
  local($arg_scheduled_station_name) = @_[25];
  local($arg_scheduled_procedure_step_location) = @_[26];
  local($arg_pre_medication) = @_[27];
  local($arg_scheduled_procedure_step_ID) = @_[28];
  local($arg_requested_contrast_agent) = @_[29];
  local($arg_scheduled_procedure_step_comments) = @_[30];
  local($arg_scheduled_station_AEtitle_OK) = @_[31];
  local($arg_scheduled_procedure_step_start_date_OK) = @_[32];
  local($arg_scheduled_procedure_step_start_time_OK) = @_[33];
  local($arg_scheduled_performing_physicians_name_OK) = @_[34];
  local($arg_scheduled_procedure_step_description_OK) = @_[35];
  local($arg_scheduled_procedure_step_location_OK) = @_[36];
  local($arg_pre_medication_OK) = @_[37];
  local($arg_scheduled_procedure_step_ID_OK) = @_[38];
  local($arg_requested_contrast_agent_OK) = @_[39];
  local($arg_scheduled_procedure_step_comments_OK) = @_[40];
  local($arg_isNewWorklist) = @_[41];

  $no_errors =
    $arg_requested_procedure_description_OK
    && $arg_requesting_physician_OK
    && $arg_patients_name_OK
    && $arg_patients_birthdate_OK
    && $arg_medical_alerts_OK
    && $arg_contrast_allergies_OK
    && $arg_scheduled_station_AEtitle_OK
    && $arg_requested_contrast_agent_OK
    && $arg_scheduled_procedure_step_start_date_OK
    && $arg_scheduled_procedure_step_start_time_OK
    && $arg_scheduled_performing_physicians_name_OK
    && $arg_scheduled_procedure_step_description_OK
    && $arg_scheduled_procedure_step_location_OK
    && $arg_pre_medication_OK
    && $arg_scheduled_procedure_step_ID_OK
    && $arg_scheduled_procedure_step_comments_OK;

  if (! $no_errors)
  {
    printf("<b>Errors</b> have been found in the data you typed in.\n");
    printf("The labels of the offending fields have been marked bold/italic.\n");
    printf("Please correct the data and press the update button.<p>\n");
  }

  printf("<FORM METHOD=get ACTION=\"%s\">\n",$arg_action);

  # ----- IMAGING SERVICE REQUEST SECTION -----
  printf("<H3>Imaging Service Request</H3>\n");
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");


  # Accession Number
  printf("<TR><TD nowrap>Accession Number:</TD>\n<TD nowrap>");
  printf("%s</TD></TR>", (($arg_accession_no eq '') ?
    'will be assigned automatically.' : $arg_accession_no));
  printf("<INPUT name=\"accession_no\" type=hidden value=\"%s\">\n", $arg_accession_no);
  # Requesting Physician
  printf("<TR><TD nowrap>");
  if (! $arg_requesting_physician_OK) { printf("<b><i>"); }
  printf("Requesting Physician:");
  if (! $arg_requesting_physician_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"requesting_physician\" maxlength=48 size=48 value=\"%s\"></TD></TR></TABLE><P>\n",
    $arg_requesting_physician);

  # ----- PATIENT SECTION -----
  printf("<HR><H3>Patient Data</H3>\n");
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");

  # Patient ID
  printf("<TR><TD nowrap>Patient ID:</TD>\n");
  printf("<TD nowrap>%s</TD></TR>\n", $arg_patient_ID);
  printf("<INPUT name=\"patient_ID\" type=hidden value=\"%s\">\n", $arg_patient_ID);

  # Patient's Name
  printf("<TR><TD nowrap>");
  if (! $arg_patients_name_OK) { printf("<b><i>"); }
  printf("Patient's Name:");
  if (! $arg_patients_name_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"patients_name\" maxlength=48 size=48 value=\"%s\"></TD></TR>\n",$arg_patients_name);

  # Patient's Birth Date
  printf("<TR><TD nowrap>");
  if (! $arg_patients_birthdate_OK) { printf("<b><i>"); }
  printf("Patient's Birth Date:");
  if (! $arg_patients_birthdate_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"patients_birthdate\" maxlength=10 size=8 value=\"%s\"></TD></TR>\n",$arg_patients_birthdate);

  # Patient's Sex
  printf("<TR><TD nowrap>Patient's Sex:</TD><TD nowrap>");
  printf("<select name=\"patients_sex\">\n");
  printf("<option value=\"\" %s>(none)\n", ($arg_patients_sex eq ''  ? 'selected' : ''));
  printf("<option value=\"F\" %s>female\n",($arg_patients_sex eq 'F' ? 'selected' : ''));
  printf("<option value=\"M\" %s>male\n",  ($arg_patients_sex eq 'M' ? 'selected' : ''));
  printf("<option value=\"O\" %s>other\n", ($arg_patients_sex eq 'O' ? 'selected' : ''));
  printf("</select></TD></TR></TABLE><P>\n");

  # Medical Alerts
  if (! $arg_medical_alerts_OK) { printf("<b><i>"); }
  printf("Medical Alerts:<br>\n");
  if (! $arg_medical_alerts_OK) { printf("</i></b>"); }
  printf("<textarea name=\"medical_alerts\" rows=6 cols=64>%s</textarea><p>\n",$arg_medical_alerts);

  # Contrast Allergies
  if (! $arg_contrast_allergies_OK) { printf("<b><i>"); }
  printf("Contrast Allergies:<br>\n");
  if (! $arg_contrast_allergies_OK) { printf("</i></b>"); }
  printf("<textarea name=\"contrast_allergies\" rows=6 cols=64>%s</textarea><p>\n",$arg_contrast_allergies);

  # ----- PROCEDURE SECTION -----
  printf("<HR><H3>Requested Procedure</H3>\n");
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");

  # Requested Procedure ID
  printf("<TR><TD nowrap>Requested Procedure ID:</TD>\n");
  printf("<TD nowrap>%s</TD></TR>\n", $arg_requested_procedure_ID);
  printf("<INPUT name=\"requested_procedure_ID\" type=hidden value=\"%s\">\n", $arg_requested_procedure_ID);

  # Requested Procedure Description
  printf("<TR><TD nowrap>");
  if (! $arg_requested_procedure_description_OK) { printf("<b><i>"); }
  printf("Requested Procedure Description:");
  if (! $arg_requested_procedure_description_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"requested_procedure_description\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n",$arg_requested_procedure_description);

  # Study Instance UID
  printf("<TR><TD nowrap>Study Instance UID:</TD>\n<TD nowrap>");
  printf("%s</TD></TR>", (($arg_study_instance_UID eq '') ?
    'will be assigned automatically.' : $arg_study_instance_UID));
  printf("<INPUT name=\"study_instance_UID\" type=hidden value=\"%s\">\n", $arg_study_instance_UID);

  # Requested Procedure Priority
  printf("<TR><TD nowrap>Requested Procedure Priority:</TD><TD nowrap>");
  printf("<select name=\"requested_procedure_priority\">\n");
  printf("<option value=\"STAT\" %s>STAT\n",($arg_requested_procedure_priority eq 'STAT' ? 'selected' : ''));
  printf("<option value=\"HIGH\" %s>HIGH\n",($arg_requested_procedure_priority eq 'HIGH' ? 'selected' : ''));
  printf("<option value=\"ROUTINE\" %s>ROUTINE\n",(($arg_requested_procedure_priority eq 'ROUTINE')||
    ($arg_requested_procedure_priority eq '') ? 'selected' : ''));
  printf("<option value=\"MEDIUM\" %s>MEDIUM\n",($arg_requested_procedure_priority eq 'MEDIUM' ? 'selected' : ''));
  printf("<option value=\"LOW\" %s>LOW\n",($arg_requested_procedure_priority eq 'LOW' ? 'selected' : ''));
  printf("</select></TD></TR></TABLE><P>\n");

  # ----- PROCEDURE STEP SECTION -----

  @keys = @STATION_KEYS;
  @values = ();
  for (@keys){ push(@values,join("\000",$STATION_VALUES{"$_\\name"},$_)); }
  @SORTED_STATIONS = sort(@values);

  printf("<HR><H3>Scheduled Procedure Step</H3>\n");
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");

  # Scheduled Procedure Step ID
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_procedure_step_ID_OK) { printf("<b><i>"); }
  printf("Scheduled Procedure Step ID:");
  if (! $arg_scheduled_procedure_step_ID_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_procedure_step_ID\" maxlength=16 size=16 value=\"%s\"></TD></TR>\n", $arg_scheduled_procedure_step_ID);

  # Modality
  printf("<TR><TD nowrap>Modality:</TD>\n<TD nowrap>");
  printf("<select name=\"modality\">\n");
  printf("<option value=\"OT\" %s>Other\n",($arg_modality eq 'OT' ? 'selected' : ''));
  printf("<option value=\"AS\" %s>Angioscopy\n",($arg_modality eq 'AS' ? 'selected' : ''));
  printf("<option value=\"BI\" %s>Biomagnetic Imaging\n",($arg_modality eq 'BI' ? 'selected' : ''));
  printf("<option value=\"CF\" %s>Cinefluorography\n",($arg_modality eq 'CF' ? 'selected' : ''));
  printf("<option value=\"CD\" %s>Color flow Doppler\n",($arg_modality eq 'CD' ? 'selected' : ''));
  printf("<option value=\"CR\" %s>Computed Radiography\n",($arg_modality eq 'CR' ? 'selected' : ''));
  printf("<option value=\"CT\" %s>Computed Tomography\n",($arg_modality eq 'CT' ? 'selected' : ''));
  printf("<option value=\"CP\" %s>Culposcopy\n",($arg_modality eq 'CP' ? 'selected' : ''));
  printf("<option value=\"CS\" %s>Cystoscopy\n",($arg_modality eq 'CS' ? 'selected' : ''));
  printf("<option value=\"DG\" %s>Diaphanography\n",($arg_modality eq 'DG' ? 'selected' : ''));
  printf("<option value=\"DF\" %s>Digital Fluoroscopy\n",($arg_modality eq 'DF' ? 'selected' : ''));
  printf("<option value=\"DM\" %s>Digital Microscopy\n",($arg_modality eq 'DM' ? 'selected' : ''));
  printf("<option value=\"DS\" %s>Digital Subtraction Angiography\n",($arg_modality eq 'DS' ? 'selected' : ''));
  printf("<option value=\"DD\" %s>Duplex Doppler\n",($arg_modality eq 'DD' ? 'selected' : ''));
  printf("<option value=\"EC\" %s>Echocardiography\n",($arg_modality eq 'EC' ? 'selected' : ''));
  printf("<option value=\"EM\" %s>Electron Microscopy\n",($arg_modality eq 'EM' ? 'selected' : ''));
  printf("<option value=\"ES\" %s>Endoscopy\n",($arg_modality eq 'ES' ? 'selected' : ''));
  printf("<option value=\"XC\" %s>External Camera\n",($arg_modality eq 'XC' ? 'selected' : ''));
  printf("<option value=\"FA\" %s>Fluorescein Angiography\n",($arg_modality eq 'FA' ? 'selected' : ''));
  printf("<option value=\"FS\" %s>Fundoscopy\n",($arg_modality eq 'FS' ? 'selected' : ''));
  printf("<option value=\"GM\" %s>Gross Microscopy\n",($arg_modality eq 'GM' ? 'selected' : ''));
  printf("<option value=\"LP\" %s>Laparoscopy\n",($arg_modality eq 'LP' ? 'selected' : ''));
  printf("<option value=\"LS\" %s>Laser Surface Scan\n",($arg_modality eq 'LS' ? 'selected' : ''));
  printf("<option value=\"MR\" %s>Magnetic Resonance\n",($arg_modality eq 'MR' ? 'selected' : ''));
  printf("<option value=\"MA\" %s>Magnetic Resonance Angiography\n",($arg_modality eq 'MA' ? 'selected' : ''));
  printf("<option value=\"MS\" %s>Magnetic Resonance Spectroscopy\n",($arg_modality eq 'MS' ? 'selected' : ''));
  printf("<option value=\"NM\" %s>Nuclear Medicine\n",($arg_modality eq 'NM' ? 'selected' : ''));
  printf("<option value=\"PT\" %s>PET\n",($arg_modality eq 'PT' ? 'selected' : ''));
  printf("<option value=\"RF\" %s>Radiofluoroscopy\n",($arg_modality eq 'RF' ? 'selected' : ''));
  printf("<option value=\"RG\" %s>Radiography\n",($arg_modality eq 'RG' ? 'selected' : ''));
  printf("<option value=\"SM\" %s>Slide Microscopy\n",($arg_modality eq 'SM' ? 'selected' : ''));
  printf("<option value=\"ST\" %s>SPECT\n",($arg_modality eq 'ST' ? 'selected' : ''));
  printf("<option value=\"TG\" %s>Thermography\n",($arg_modality eq 'TG' ? 'selected' : ''));
  printf("<option value=\"US\" %s>Ultrasound\n",($arg_modality eq 'US' ? 'selected' : ''));
  printf("<option value=\"VF\" %s>Videofluorography\n",($arg_modality eq 'VF' ? 'selected' : ''));
  printf("<option value=\"XA\" %s>X-Ray Angiography\n",($arg_modality eq 'XA' ? 'selected' : ''));
  printf("</select></TD></TR>\n");

  # Scheduled Procedure Step Description
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_procedure_step_description_OK) { printf("<b><i>"); }
  printf("Scheduled Procedure Step Description:");
  if (! $arg_scheduled_procedure_step_description_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_procedure_step_description\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n", $arg_scheduled_procedure_step_description);

  # Pre-Medication
  printf("<TR><TD nowrap>");
  if (! $arg_pre_medication_OK) { printf("<b><i>"); }
  printf("Pre-Medication:");
  if (! $arg_pre_medication_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"pre_medication\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n", $arg_pre_medication);

  # Requested Contrast Agent
  printf("<TR><TD nowrap>");
  if (! $arg_requested_contrast_agent_OK) { printf("<b><i>"); }
  printf("Requested Contrast Agent:");
  if (! $arg_requested_contrast_agent_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"requested_contrast_agent\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n", $arg_requested_contrast_agent);

  # Scheduled Procedure Step Location
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_procedure_step_location_OK) { printf("<b><i>"); }
  printf("Scheduled Procedure Step Location:");
  if (! $arg_scheduled_procedure_step_location_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_procedure_step_location\" maxlength=16 size=16 value=\"%s\"></TD></TR>\n", $arg_scheduled_procedure_step_location);

  # Scheduled Performing Physician's Name
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_performing_physicians_name_OK) { printf("<b><i>"); }
  printf("Scheduled Performing Physician's Name:");
  if (! $arg_scheduled_performing_physicians_name_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_performing_physicians_name\" maxlength=48 size=48 value=\"%s\"></TD></TR>\n", $arg_scheduled_performing_physicians_name);

  # Scheduled Procedure Step Start Date
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_procedure_step_start_date_OK) { printf("<b><i>"); }
  printf("Scheduled Procedure Step Start Date:");
  if (! $arg_scheduled_procedure_step_start_date_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_procedure_step_start_date\" maxlength=10 size=8 value=\"%s\"></TD></TR>\n", $arg_scheduled_procedure_step_start_date);

  # Scheduled Procedure Step Start Time
  printf("<TR><TD nowrap>");
  if (! $arg_scheduled_procedure_step_start_time_OK) { printf("<b><i>"); }
  printf("Scheduled Procedure Step Start Time:");
  if (! $arg_scheduled_procedure_step_start_time_OK) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"scheduled_procedure_step_start_time\" maxlength=10 size=4 value=\"%s\"></TD></TR>\n", $arg_scheduled_procedure_step_start_time);
  printf("</TABLE><P>\n");

  # Scheduled Stations (AE Titles and Names)
  if (! $arg_scheduled_station_AEtitle_OK) { printf("<b><i>"); }
  printf("Scheduled Stations:<br>\n");
  if (! $arg_scheduled_station_AEtitle_OK) { printf("</i></b>"); }
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");
  $arg_scheduled_station_AEtitle = "\\$arg_scheduled_station_AEtitle\\";

  for (@SORTED_STATIONS)
  {
      ($name, $key) = split(/\000/);
       printf("<TD nowrap><input name=\"ae_%s\" type=checkbox value=\"yes\" %s></TD>\n",
        unpack("H*",$key), ($arg_scheduled_station_AEtitle =~ /\\$key\\/ ? 'checked' : ''));
      printf("<TD nowrap>%s</TD><TD nowrap>Modality: %s, Application Entity Title: %s</TD></TR>\n",
        $name, $STATION_VALUES{"$key\\modality"}, $key);
  }
  printf("</TABLE><P>\n");

  # Comments on the Scheduled Procedure Step
  if (! $arg_scheduled_procedure_step_comments_OK) { printf("<b><i>"); }
  printf("Comments on the Scheduled Procedure Step:<br>\n");
  if (! $arg_scheduled_procedure_step_comments_OK) { printf("</i></b>"); }
  printf("<textarea name=\"scheduled_procedure_step_comments\" rows=6 cols=64>%s</textarea><p>\n", $arg_scheduled_procedure_step_comments);

  # ----- FINISH -----

  printf("<HR>");
  if ($arg_isNewWorklist)
  {
    printf("<P><INPUT TYPE=SUBMIT name=\"action\" value=\"Save\">\n");
    printf("<INPUT TYPE=RESET name=\"action\" value=\"Reset Form\">\n");
    printf("<INPUT TYPE=SUBMIT name=\"action\" value=\"Cancel\">\n");
  } else {
    printf("<P><INPUT TYPE=SUBMIT name=\"action\" value=\"Update\">\n");
    printf("<INPUT TYPE=RESET name=\"action\" value=\"Reset Form\">\n");
    printf("<INPUT TYPE=SUBMIT name=\"action\" value=\"Cancel\">\n");
  }
  printf("</FORM>\n");
  &page_footer;
}

