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
#   This module contains perl procedures which produce "standard"
#   HTML headers and footers for the dcmwlm WWW component.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: layout.ph,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:05  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#

require 'prefs.ph';

#
#  void page_title(string title)
#    creates a HTML header (with given title) for a dcmwlm WWW Page.
#
sub page_title
{
# ---------------------------------------------------
print <<EOF;
Content-type: text/html

<HTML>
<HEAD>
<TITLE>
DICOM Basic Worklist Management
</TITLE>
</HEAD>

<BODY>
<CENTER>
EOF
# ---------------------------------------------------
  printf("<IMG SRC = \"%s\"></IMG>\n",$prefs{'offis_gif'});
  printf("<H3>DICOM Basic Worklist Management</H3>\n");
  printf("<H1>%s</H1></CENTER><HR><P>\n\n",@_[0]);
}

#
#  void page_footer()
#    creates a HTML footer for a dcmwlm WWW Page.
#
sub page_footer
{
# ---------------------------------------------------
print <<EOF;
<P>
<HR>
<CENTER><SMALL>&#169;1996 OFFIS /
Carl-von-Ossietzky Universit&auml;t Oldenburg</SMALL></CENTER>
</BODY>
</HTML>
EOF
}

1;
