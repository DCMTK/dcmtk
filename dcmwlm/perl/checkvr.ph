#!/usr/local/bin/perl
#
#  Copyright (C) 1996-2002, OFFIS
#
#  This software and supporting documentation were developed by
#
#    Kuratorium OFFIS e.V.
#    Forschungsbereich 2: Kommunikationssysteme
#    Escherweg 2
#    D-26121 Oldenburg, Germany
#
#  for CEN/TC251/WG4 as a contribution to the Computer Assisted Radiology
#  (CAR) 1996 DICOM Demonstration.
#
#  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
#  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
#  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
#  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
#  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
#
#  Copyright of the software  and  supporting  documentation  is,  unless
#  otherwise stated, owned by OFFIS, and free access is hereby granted as
#  a license to  use  this  software,  copy  this  software  and  prepare
#  derivative works based upon this software.  However, any  distribution
#  of this software source code or supporting documentation or derivative
#  works  (source code and  supporting documentation)  must  include  the
#  three paragraphs of this copyright notice.
#
#
# Module: dcmwlm (WWW Component)
#
# Author: Marco Eichelberg
#
# Purpose:
#   This module contains perl procedures which check (more or less)
#   the correctness of strings as DICOM value representations.
#
# Last Update:      $Author: wilkens $
# Update Date:      $Date: 2002-12-03 12:16:04 $
# Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/perl/checkvr.ph,v $
# CVS/RCS Revision: $Revision: 1.1 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: checkvr.ph,v $
#   Revision 1.1  2002-12-03 12:16:04  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#

#
# string stripWS(string S)
#   strips leading and trailing whitespace from a string.
sub stripWS
{
  local($s) = @_[0];
  $s =~ s/^[ \t\n]*//;
  $s =~ s/[ \t\n]*$//;
  $s;
}

#
# string makeDA(string DA)
#   strips whitespace etc. from a DA (date string)
#   argument must be in HTML format (not ISO Latin)
sub makeDA
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkDA(string DA)
#   checks if a DA (date string) is correct
#   argument must be in HTML format (not ISO Latin)
sub checkDA
{
  local($result) = 1;
  local($s);
  if (@_[0] =~ /[0-9][0-9][0-9][0-9]\.[0-9][0-9]\.[0-9][0-9]/)
  {
    $s = substr(@_[0],0,4);
    if (($s < 1000)||($s > 2200)) { $result=0; }
    $s = substr(@_[0],5,2);
    if (($s < 1)||($s > 12)) { $result=0; }
    $s = substr(@_[0],8,2);
    if (($s < 1)||($s > 31)) { $result=0; }
  } elsif (@_[0] =~ /[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]/)
  {
    $s = substr(@_[0],0,4);
    if (($s < 1000)||($s > 2200)) { $result=0; }
    $s = substr(@_[0],4,2);
    if (($s < 1)||($s > 12)) { $result=0; }
    $s = substr(@_[0],6,2);
    if (($s < 1)||($s > 31)) { $result=0; }
  } else { $result = 0; }
  $result;
}

#
# string makeTM(string TM)
#   strips whitespace etc. from a TM (time string)
#   argument must be in HTML format (not ISO Latin)
sub makeTM
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkTM(string TM)
#   checks if a TM (time string) is correct
#   argument must be in HTML format (not ISO Latin)
sub checkTM
{
  local($result) = 1;
  local($s);
  if (@_[0] =~ /^[0-9][0-9][0-9][0-9]/)
  {
    $s = substr(@_[0],0,2);
    if ($s > 23) { $result=0; }
    $s = substr(@_[0],2,2);
    if ($s > 59) { $result=0; }
    $s = substr(@_[0],4);
    if (length($s) >0 )
    {
      if (($s =~ /^[0-9][0-9]\.[0-9]+/) || ($s =~ /^[0-9][0-9]$/))
      {
        if (substr($s,0,2) > 59) { $result=0; }
      } else { $result = 0; }
    }
  } else { $result = 0; printf(stderr "Error1"); } 
  $result;
}

#
# string makePN(string PN)
#   strips whitespace etc. from a PN (Person Name)
#   argument must be in HTML format (not ISO Latin)
sub makePN
{
  local($s) = &stripWS(@_[0]);
  $s =~ s/[\t\n]/ /g;
  $s;
}

#
# bool checkPN(string PN)
#   checks if a PN (person name) is correct
#   argument must be in HTML format (not ISO Latin)
sub checkPN
{
  local($result) = 0;
  local($s);
  # We allow single component names for ACR-NEMA compatibility reasons.
  # strict test would be: if (@_[0] =~ /..*\^..*/) { $result = 1; }
  if (length(@_[0])>0) { $result = 1; }
  $result;
}

#
# string makeSH(string SH)
#   strips whitespace etc. from a SH (Short String)
#   argument must be in HTML format (not ISO Latin)
sub makeSH
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkSH(string SH)
#   checks if a SH (Short String) is correct
#   argument must be in HTML format (not ISO Latin)
sub checkSH
{
  1; # we currently don't check SHs.
}

#
# string makeAE(string AE)
#   strips whitespace etc. from a AE (Application Entity Title)
#   argument must be in HTML format (not ISO Latin)
sub makeAE
{
  local($s) = &stripWS(@_[0]);
  $s =~ s/\\//g; # remove backslashes
  $s;
}

#
# bool checkAE(string AE)
#   checks if a AE (Application Entity Title) is correct
#   argument must be in HTML format (not ISO Latin)
sub checkAE
{
  local($result) = length(@_[0]) > 0;
  $result;
}

#
# string makeLO(string LO)
#   strips whitespace etc. from a LO (Long String).
#   argument must be in HTML format (not ISO Latin)
sub makeLO
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkLO(string LO)
#   checks if a LO (Long String) is correct.
#   argument must be in HTML format (not ISO Latin)
sub checkLO
{
  1; # we currently don't check LOs.
}

#
# string makeLON(string LON)
#   strips whitespace etc. from a LO 1-n (Multiline Long String).
#   argument must be in HTML format (not ISO Latin)
sub makeLON
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkLON(string LON)
#   checks if a LO 1-n (Multiline Long String) is correct.
#   argument must be in HTML format (not ISO Latin)
sub checkLON
{
  1; # we currently don't check LOs.
}

#
# string makeLT(string LT)
#   strips whitespace etc. from a LT (Long Text).
#   argument must be in HTML format (not ISO Latin)
sub makeLT
{
  local($s) = &stripWS(@_[0]);
  $s;
}

#
# bool checkLT(string LT)
#   checks if a LO 1-n (Long Text) is correct.
#   argument must be in HTML format (not ISO Latin)
sub checkLT
{
  1; # we currently don't check LTs.
}

1;
