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
#   This perl script allows to create and update procedure data
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: procedit.pl,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:12  wilkens
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
$procid = '';
if ($path_info ne '')
{
  ($dummy, $aetitle, $passwd, $procid, $rest) = split(/\//, $path_info);
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
      printf("Location: %s/%s/%s\n\n", $prefs{'procedur.pl'}, $aetitle, $passwd);
    } else {
      # We have received a filled-in form.
      # We save/update it and return a URL to the updated form.
      $title = &makeLO($rqpairs{'title'});
      $titleOK = (&checkLO($title))&&(length($title)>0);
      $description = &makeLO($rqpairs{'description'});
      $descriptionOK = (&checkLO($description))&&(length($description)>0);

      if (($titleOK)&&($descriptionOK))
      {
        &set_writelock("$prefs{'data_path'}/$aetitle");
        if ($procid eq '')
        {
          $procid = unpack("H*",&nextProcID($aetitle));
        }
        $decodedid = pack("H*",$procid);
        $newentry = 1;
        foreach(@PROCEDURE_KEYS) { if ($decodedid eq $_) { $newentry = 0; } }
        if ($newentry) { push(@PROCEDURE_KEYS, $decodedid); }
        $PROCEDURE_VALUES{"$decodedid\\title"} = $title;
        $PROCEDURE_VALUES{"$decodedid\\description"} = $description;
        $PROCEDURE_VALUES{"$decodedid\\priority"} = $rqpairs{'priority'};
        &write_environment($filename);
        printf("Location: %s/%s/%s\n\n", $prefs{'procedur.pl'}, $aetitle, $passwd);
      } else {
        &create_procedure_form("$prefs{'procedit.pl'}$path_info",
          pack("H*",$procid), $title, $description,
          $rqpairs{'priority'}, $titleOK, $descriptionOK, 0);
      }
    }
  } else {
    # We have received a request for a form.
    if ($procid eq '')
    {
      &create_procedure_form("$prefs{'procedit.pl'}/$aetitle/$passwd",
        '', '', '', '', 1, 1, 1);
    } else {
      $key = pack("H*",$procid);
      &create_procedure_form("$prefs{'procedit.pl'}/$aetitle/$passwd/$procid",
        $key,
        $PROCEDURE_VALUES{"$key\\title"},
        $PROCEDURE_VALUES{"$key\\description"},
        $PROCEDURE_VALUES{"$key\\priority"},
        1, 1, 0);
    }
  }
  &release_lock;
}

#
#  void create_procedure_form(
#   0 string action,
#   1 string procid,
#   2 string title,
#   3 string description,
#   4 string priority,
#   5 bool titleOK,
#   6 bool descriptionOK,
#   7 bool isNewProcedure)
#
sub create_procedure_form
{
  &page_title("Procedure Form");

  if ((! @_[5])||(! @_[6]))
  {
    printf("<b>Errors</b> have been found in the data you typed in.\n");
    printf("The labels of the offending fields have been marked bold/italic.\n");
    printf("Please correct the data and press the update button.<p>\n");
  }
  printf("<FORM METHOD=get ACTION=\"%s\">\n",@_[0]);
  printf("<TABLE  CELLSPACING=\"3\" BORDER=0  WIDTH=80%%>\n");
  printf("<TR><TD nowrap>Procedure ID:</TD>\n");
  printf("<TD nowrap>%s<TR><TD nowrap>", ((@_[1] eq '') ? 'will be assigned automatically.' : @_[1]));
  if (! @_[5]) { printf("<b><i>"); }
  printf("Title:");
  if (! @_[5]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"title\" maxlength=48 size=48 value=\"%s\"></TD></TR>\n",@_[2]);

  printf("<TR><TD nowrap>");
  if (! @_[6]) { printf("<b><i>"); }
  printf("Description:");
  if (! @_[6]) { printf("</i></b>"); }
  printf("</TD>\n<TD nowrap>");
  printf("<input name=\"description\" maxlength=64 size=64 value=\"%s\"></TD></TR>\n",@_[3]);

  printf("<TR><TD nowrap>Default Priority:</TD><TD nowrap>");
  printf("<select name=\"priority\">\n");
  printf("<option value=\"STAT\" %s>STAT\n",(@_[4] eq 'STAT' ? 'selected' : ''));
  printf("<option value=\"HIGH\" %s>HIGH\n",(@_[4] eq 'HIGH' ? 'selected' : ''));
  printf("<option value=\"ROUTINE\" %s>ROUTINE\n",((@_[4] eq 'ROUTINE')||(@_[4] eq '') ? 'selected' : ''));
  printf("<option value=\"MEDIUM\" %s>MEDIUM\n",(@_[4] eq 'MEDIUM' ? 'selected' : ''));
  printf("<option value=\"LOW\" %s>LOW\n",(@_[4] eq 'LOW' ? 'selected' : ''));
  printf("</select></TD></TR></TABLE><P>\n");

  if (! @_[7])
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
