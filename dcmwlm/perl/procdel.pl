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
#   This perl script allows to delete a procedure entry from a storage area.
#
# Last Update:      $Author: wilkens $
# Update Date:      $Date: 2002-12-03 12:16:11 $
# Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/perl/procdel.pl,v $
# CVS/RCS Revision: $Revision: 1.1 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: procdel.pl,v $
#   Revision 1.1  2002-12-03 12:16:11  wilkens
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
$command = '';
if ($path_info ne '')  
{
  ($dummy, $aetitle, $passwd, $procid, $command, $rest) = split(/\//, $path_info);
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
    &page_title("Delete Procedure");
    $decodedid = pack("H*",$procid);
    printf("You have requested the deletion of procedure ID <b>'%s'</b>.<br>\n",$decodedid);
    printf("<b>Attention:</b> Worklist entries referring to this procedure will <b>not</b> be removed!<p>\n");
    # check if there are procedure steps for this procedure
    $count=0;
    foreach(@PROCEDURESTEP_KEYS) 
    { 
      if ($PROCEDURESTEP_VALUES{"$_\\procedure"} eq $decodedid) { $count++; }
    }
    if ($count>0)
    {
      printf("<h4>Warning</h4>\n");
      printf("%d Procedure Step(s) assigned to this procedure will also be deleted:\n<UL>",$count);
      foreach(@PROCEDURESTEP_KEYS) 
      { 
        if ($PROCEDURESTEP_VALUES{"$_\\procedure"} eq $decodedid) 
        {
          printf("<li>%s: %s\n", $_, $PROCEDURESTEP_VALUES{"$_\\title"});
        }
      }
      printf("</ul><p>\n");
    }
    printf("<A HREF=\"%s%s/DELETE\">Delete</A> ", $prefs{'procdel.pl'}, $path_info);
    printf("or <A HREF=\"%s%s\">Cancel</A> and return to procedure overview.\n", 
      $prefs{'procedur.pl'}, $path_info);
    &page_footer;
  } else {
    # erase and back to procedure overview
    &set_writelock("$prefs{'data_path'}/$aetitle");
    $decodedid = pack("H*",$procid);
    # remove procedure key
    undef @temp;
    foreach(@PROCEDURE_KEYS) { if ($decodedid ne $_) { push(@temp, $_); } }
    @PROCEDURE_KEYS = @temp;
    # remove values    
    delete $PROCEDURE_VALUES{"$decodedid\\title"};
    delete $PROCEDURE_VALUES{"$decodedid\\description"};
    delete $PROCEDURE_VALUES{"$decodedid\\priority"};
    # now delete procedure steps assigned to this procedure...
    @pstemp = @PROCEDURESTEP_KEYS;
    foreach(@pstemp) 
    { 
      if ($PROCEDURESTEP_VALUES{"$_\\procedure"} eq $decodedid) 
      {
        $key = $_;
        undef @temp;
        foreach(@PROCEDURESTEP_KEYS) { if ($key ne $_) { push(@temp, $_); } }
        @PROCEDURESTEP_KEYS = @temp;
        # remove values 
        delete $PROCEDURESTEP_VALUES{"$key\\procedure"};
        delete $PROCEDURESTEP_VALUES{"$key\\title"};
        delete $PROCEDURESTEP_VALUES{"$key\\modality"};
        delete $PROCEDURESTEP_VALUES{"$key\\medication"};
        delete $PROCEDURESTEP_VALUES{"$key\\contrast"};
        delete $PROCEDURESTEP_VALUES{"$key\\location"};
        delete $PROCEDURESTEP_VALUES{"$key\\comments"};        
      }
    }
    
    &write_environment($filename);
    printf("Location: %s/%s/%s\n\n", $prefs{'procedur.pl'}, $aetitle, $passwd);
  }
  &release_lock;
}

