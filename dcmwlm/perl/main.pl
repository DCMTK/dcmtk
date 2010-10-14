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
#   This perl script displays the main menu for the dcmwlm WWW component.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: main.pl,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:07  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#

require 'layout.ph';
require 'password.ph';
require 'urldecod.ph';
require 'lock.ph';
require 'write.ph';

&get_request;
$path_info=$ENV{'PATH_INFO'};

if ($path_info eq '')
{
  # No path contained in the URL. We create a list of available storage areas.
  &page_title("Available Storage Areas");
  printf("<ul>\n");
  opendir(DIRHANDLE,"$prefs{'data_path'}");
  local(@dirlist) = readdir(DIRHANDLE);
  closedir(DIRHANDLE);
  foreach (sort(@dirlist))
  {
    if (($_ !~ /\..*/)&&(-f "$prefs{'data_path'}/$_/$prefs{'lockfile'}"))
    {
      printf("<li><A HREF=\"%s/%s\">%s</A>\n",$prefs{'main.pl'},$_,$_);
    }
  }
  printf("</ul>\n");
  &page_footer;
} else {
  # We have some path info
  $aetitle = '';
  $passwd = '';
  ($dummy, $aetitle, $passwd, $rest) = split(/\//, $path_info);
  if (-f "$prefs{'data_path'}/$aetitle/$prefs{'lockfile'}")
  {
    # AE Title is OK.
    if ($passwd eq '')
    {
      # No Password contained in path.
      if ($request eq '')
      {
        $testpasswd = &makepasswd('', $aetitle);
        if (&checkurlcode($testpasswd, $aetitle))
        {
          printf("Location: %s/%s/%s\n\n", $prefs{'main.pl'}, $aetitle, $testpasswd);
        } else {
          &page_title("Password Request");
          printf("Please enter the password for storage area '%s':\n",$aetitle);
          printf("<FORM METHOD=get ACTION=\"%s/%s\">\n",$prefs{'main.pl'}, $aetitle);
          printf("<INPUT TYPE=HIDDEN name=\"request\" value=\"                                                            \">\n");
          printf("<CENTER><INPUT TYPE=PASSWORD name=\"passwd\" size=20 value=\"\"><p>\n");
          printf("<INPUT TYPE=SUBMIT name=\"action\" value=\"Submit\"></CENTER>\n");
          &page_footer;
        }
      } else {
        $passwd = $rqpairs{'passwd'};
        printf("Location: %s/%s/%s\n\n", $prefs{'main.pl'}, $aetitle,  &makepasswd($passwd, $aetitle));
      }
    } else {
      # Password is present.
      if (&checkurlcode($passwd, $aetitle))
      {
        # Password is correct.
        &page_title("Main Menu - '$aetitle'");
        &set_readlock("$prefs{'data_path'}/$aetitle");
        $filename = "$prefs{'data_path'}/$aetitle/$prefs{'datafile'}";
        &read_environment($filename);
        $count = 0;
        if (opendir(DIRHANDLE, "$prefs{'data_path'}/$aetitle"))
        {
          $dicomfiles = readdir(DIRHANDLE);
          while ($dicomfiles ne '')
          {
            if ($dicomfiles =~ /.*\.$prefs{'dicom_ext'}$/) { $count++; }
            $dicomfiles = readdir(DIRHANDLE);
          }
          closedir(DIRHANDLE);
        }
        &release_lock;
        # Now we check if there are patients, stations, procedure steps and so on...
        printf("<ul><li><A HREF=\"%s/%s/%s\">Manage Patients</A><p>\n",
          $prefs{'patient.pl'}, $aetitle, $passwd);
        printf("currently %d patient(s) exist.<p>\n", $#PATIENT_KEYS+1);
        printf("<li><A HREF=\"%s/%s/%s\">Manage Stations</A><p>\n",
          $prefs{'station.pl'}, $aetitle, $passwd);
        printf("currently %d station(s) exist.<p>\n", $#STATION_KEYS+1);
        printf("<li><A HREF=\"%s/%s/%s\">Manage Procedures</A><p>\n",
          $prefs{'procedur.pl'}, $aetitle, $passwd);
        printf("currently %d procedure(s) exist.<p>\n", $#PROCEDURE_KEYS+1);
        printf("<li><A HREF=\"%s/%s/%s\">Manage Procedure Steps</A><p>\n",
          $prefs{'procstep.pl'}, $aetitle, $passwd);
        printf("currently %d procedure step(s) exist.<p>\n", $#PROCEDURESTEP_KEYS+1);
        printf("<li><A HREF=\"%s/%s/%s\">Manage Worklist Entries</A><p>\n",
          $prefs{'worklist.pl'}, $aetitle, $passwd);
        printf("currently %d worklist entries exist.<p></ul>\n", $count);
        if (! -f "$prefs{'data_path'}/$aetitle/$prefs{'publicfile'}")
        {
          printf("<hr><ul><li><A HREF=\"%s/%s/%s\">Change Password</A></ul><p>\n",
            $prefs{'changepw.pl'}, $aetitle, $passwd);
        }
        &page_footer;
      } else {
        # Password is incorrect.
        &page_title("Password invalid");
        printf("The password for storage area '%s' is invalid.<p>\n", $aetitle);
        printf("<A HREF=\"%s/%s\">Click here</A> to retry.\n",$prefs{'main.pl'}, $aetitle);
        &page_footer;
      }
    }
  } else {
    # This AE title does not exist
    &page_title("Storage area not found");
    printf("The storage area '%s' was not found on the server.<p>\n", $aetitle);
    printf("<A HREF=\"%s\">Click here</A> to return to main menu.\n",$prefs{'main.pl'});
    &page_footer;
  }

}
