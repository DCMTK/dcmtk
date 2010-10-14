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
#   This module contains perl procedures which allow to convert between
#   HTML encoded strings containing key words like "&uuml;" for ISO Latin 1
#   characters and 8-Bit ISO 8859-1 (ISO Latin 1) encoded string.
#   The HTML strings must obey Unix line end conventions, the ISO strings
#   usw DICOM/DOS conventions (CR/LF).
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: isocode.ph,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:04  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#


# Parameter: Binary coded ISO 8859-1 string
# Result: HTML coded string
sub iso_to_html
{
  local($result) = '';
  for (split(//,@_[0]))
  {
    loop1:
    {
      if ($_ =~ /[\000-\010\013\014\016-\037\015\177-\240]/) { last loop1; }
      if ($_ =~ /[ 0-9A-Za-z]/) { $result = join('',$result,$_); last loop1; }
      $result = sprintf("%s&#%03d;", $result, ord($_));
      last loop1;
    }
  }
  $result;
}

# Parameter: HTML coded string
# Result: Binary coded ISO 8859-1 string
sub html_to_iso
{
  local($result) = '';
  local($in) = @_[0];
  local($temp);
  while (length($in) gt 0)
  {
    if (substr($in,0,1) eq '&')
    {
      loop2:
      {
        if ($in =~ /^&#[0-9]*;/)
        {
          # Numeric escape sequence
          $temp = substr($in,2);
          $temp =~ s/;.*//;
          if ($temp == 10) { $result = join('',$result,"\015\012"); }
          elsif ($temp < 256) { $result = join('',$result,pack("c",$temp)); }
          $in =~ s/^&#[0-9]*;//;
          last loop2;
        }

        if ($in =~ /^&lt;/)
        {
          $result = join('',$result,'<');
          $in =~ s/^&lt;//;
          last loop2;
        }
        if ($in =~ /^&gt;/)
        {
          $result = join('',$result,'>');
          $in =~ s/^&gt;//;
          last loop2;
        }
        if ($in =~ /^&amp;/)
        {
          $result = join('',$result,'&');
          $in =~ s/^&amp;//;
          last loop2;
        }
        if ($in =~ /^&quot;/)
        {
          $result = join('',$result,'"');
          $in =~ s/^&quot;//;
          last loop2;
        }

        if ($in =~ /^&Aacute;/)
        {
          $result = join('',$result,pack("c",192));
          $in =~ s/^&Aacute;//;
          last loop2;
        }
        if ($in =~ /^&Agrave;/)
        {
          $result = join('',$result,pack("c",193));
          $in =~ s/^&Agrave;//;
          last loop2;
        }
        if ($in =~ /^&Acirc;/)
        {
          $result = join('',$result,pack("c",194));
          $in =~ s/^&Acirc;//;
          last loop2;
        }
        if ($in =~ /^&Atilde;/)
        {
          $result = join('',$result,pack("c",195));
          $in =~ s/^&Atilde;//;
          last loop2;
        }
        if ($in =~ /^&Aring;/)
        {
          $result = join('',$result,pack("c",196));
          $in =~ s/^&Aring;//;
          last loop2;
        }
        if ($in =~ /^&Auml;/)
        {
          $result = join('',$result,pack("c",197));
          $in =~ s/^&Auml;//;
          last loop2;
        }
        if ($in =~ /^&AElig;/)
        {
          $result = join('',$result,pack("c",198));
          $in =~ s/^&AElig;//;
          last loop2;
        }
        if ($in =~ /^&Ccedil;/)
        {
          $result = join('',$result,pack("c",199));
          $in =~ s/^&Ccedil;//;
          last loop2;
        }
        if ($in =~ /^&Eacute;/)
        {
          $result = join('',$result,pack("c",200));
          $in =~ s/^&Eacute;//;
          last loop2;
        }
        if ($in =~ /^&Egrave;/)
        {
          $result = join('',$result,pack("c",201));
          $in =~ s/^&Egrave;//;
          last loop2;
        }
        if ($in =~ /^&Ecirc;/)
        {
          $result = join('',$result,pack("c",202));
          $in =~ s/^&Ecirc;//;
          last loop2;
        }
        if ($in =~ /^&Euml;/)
        {
          $result = join('',$result,pack("c",203));
          $in =~ s/^&Euml;//;
          last loop2;
        }
        if ($in =~ /^&Iacute;/)
        {
          $result = join('',$result,pack("c",204));
          $in =~ s/^&Iacute;//;
          last loop2;
        }
        if ($in =~ /^&Igrave;/)
        {
          $result = join('',$result,pack("c",205));
          $in =~ s/^&Igrave;//;
          last loop2;
        }
        if ($in =~ /^&Icirc;/)
        {
          $result = join('',$result,pack("c",206));
          $in =~ s/^&Icirc;//;
          last loop2;
        }
        if ($in =~ /^&Iuml;/)
        {
          $result = join('',$result,pack("c",207));
          $in =~ s/^&Iuml;//;
          last loop2;
        }
        if ($in =~ /^&ETH;/)
        {
          $result = join('',$result,pack("c",208));
          $in =~ s/^&ETH;//;
          last loop2;
        }
        if ($in =~ /^&Ntilde;/)
        {
          $result = join('',$result,pack("c",209));
          $in =~ s/^&Ntilde;//;
          last loop2;
        }
        if ($in =~ /^&Oacute;/)
        {
          $result = join('',$result,pack("c",210));
          $in =~ s/^&Oacute;//;
          last loop2;
        }
        if ($in =~ /^&Ograve;/)
        {
          $result = join('',$result,pack("c",211));
          $in =~ s/^&Ograve;//;
          last loop2;
        }
        if ($in =~ /^&Ocirc;/)
        {
          $result = join('',$result,pack("c",212));
          $in =~ s/^&Ocirc;//;
          last loop2;
        }
        if ($in =~ /^&Otilde;/)
        {
          $result = join('',$result,pack("c",213));
          $in =~ s/^&Otilde;//;
          last loop2;
        }
        if ($in =~ /^&Ouml;/)
        {
          $result = join('',$result,pack("c",214));
          $in =~ s/^&Ouml;//;
          last loop2;
        }
        # There is no keyword for char 215
        if ($in =~ /^&Oslash;/)
        {
          $result = join('',$result,pack("c",216));
          $in =~ s/^&Oslash;//;
          last loop2;
        }
        if ($in =~ /^&Uacute;/)
        {
          $result = join('',$result,pack("c",217));
          $in =~ s/^&Uacute;//;
          last loop2;
        }
        if ($in =~ /^&Ugrave;/)
        {
          $result = join('',$result,pack("c",218));
          $in =~ s/^&Ugrave;//;
          last loop2;
        }
        if ($in =~ /^&Ucirc;/)
        {
          $result = join('',$result,pack("c",219));
          $in =~ s/^&Ucirc;//;
          last loop2;
        }
        if ($in =~ /^&Uuml;/)
        {
          $result = join('',$result,pack("c",220));
          $in =~ s/^&Uuml;//;
          last loop2;
        }
        if ($in =~ /^&Yacute;/)
        {
          $result = join('',$result,pack("c",221));
          $in =~ s/^&Yacute;//;
          last loop2;
        }
        if ($in =~ /^&THORN;/)
        {
          $result = join('',$result,pack("c",222));
          $in =~ s/^&THORN;//;
          last loop2;
        }
        if ($in =~ /^&szlig;/)
        {
          $result = join('',$result,pack("c",223));
          $in =~ s/^&szlig;//;
          last loop2;
        }
        if ($in =~ /^&aacute;/)
        {
          $result = join('',$result,pack("c",224));
          $in =~ s/^&aacute;//;
          last loop2;
        }
        if ($in =~ /^&agrave;/)
        {
          $result = join('',$result,pack("c",225));
          $in =~ s/^&agrave;//;
          last loop2;
        }
        if ($in =~ /^&acirc;/)
        {
          $result = join('',$result,pack("c",226));
          $in =~ s/^&acirc;//;
          last loop2;
        }
        if ($in =~ /^&atilde;/)
        {
          $result = join('',$result,pack("c",227));
          $in =~ s/^&atilde;//;
          last loop2;
        }
        if ($in =~ /^&aring;/)
        {
          $result = join('',$result,pack("c",228));
          $in =~ s/^&aring;//;
          last loop2;
        }
        if ($in =~ /^&auml;/)
        {
          $result = join('',$result,pack("c",229));
          $in =~ s/^&auml;//;
          last loop2;
        }
        if ($in =~ /^&aelig;/)
        {
          $result = join('',$result,pack("c",230));
          $in =~ s/^&aelig;//;
          last loop2;
        }
        if ($in =~ /^&ccedil;/)
        {
          $result = join('',$result,pack("c",231));
          $in =~ s/^&ccedil;//;
          last loop2;
        }
        if ($in =~ /^&eacute;/)
        {
          $result = join('',$result,pack("c",232));
          $in =~ s/^&eacute;//;
          last loop2;
        }
        if ($in =~ /^&egrave;/)
        {
          $result = join('',$result,pack("c",233));
          $in =~ s/^&egrave;//;
          last loop2;
        }
        if ($in =~ /^&ecirc;/)
        {
          $result = join('',$result,pack("c",234));
          $in =~ s/^&ecirc;//;
          last loop2;
        }
        if ($in =~ /^&euml;/)
        {
          $result = join('',$result,pack("c",235));
          $in =~ s/^&euml;//;
          last loop2;
        }
        if ($in =~ /^&iacute;/)
        {
          $result = join('',$result,pack("c",236));
          $in =~ s/^&iacute;//;
          last loop2;
        }
        if ($in =~ /^&igrave;/)
        {
          $result = join('',$result,pack("c",237));
          $in =~ s/^&igrave;//;
          last loop2;
        }
        if ($in =~ /^&icirc;/)
        {
          $result = join('',$result,pack("c",238));
          $in =~ s/^&icirc;//;
          last loop2;
        }
        if ($in =~ /^&iuml;/)
        {
          $result = join('',$result,pack("c",239));
          $in =~ s/^&iuml;//;
          last loop2;
        }
        if ($in =~ /^&eth;/)
        {
          $result = join('',$result,pack("c",240));
          $in =~ s/^&eth;//;
          last loop2;
        }
        if ($in =~ /^&ntilde;/)
        {
          $result = join('',$result,pack("c",241));
          $in =~ s/^&ntilde;//;
          last loop2;
        }
        if ($in =~ /^&oacute;/)
        {
          $result = join('',$result,pack("c",242));
          $in =~ s/^&oacute;//;
          last loop2;
        }
        if ($in =~ /^&ograve;/)
        {
          $result = join('',$result,pack("c",243));
          $in =~ s/^&ograve;//;
          last loop2;
        }
        if ($in =~ /^&ocirc;/)
        {
          $result = join('',$result,pack("c",244));
          $in =~ s/^&ocirc;//;
          last loop2;
        }
        if ($in =~ /^&otilde;/)
        {
          $result = join('',$result,pack("c",245));
          $in =~ s/^&otilde;//;
          last loop2;
        }
        if ($in =~ /^&ouml;/)
        {
          $result = join('',$result,pack("c",246));
          $in =~ s/^&ouml;//;
          last loop2;
        }
        # There is no keyword for character 247
        if ($in =~ /^&oslash;/)
        {
          $result = join('',$result,pack("c",248));
          $in =~ s/^&oslash;//;
          last loop2;
        }
        if ($in =~ /^&uacute;/)
        {
          $result = join('',$result,pack("c",249));
          $in =~ s/^&uacute;//;
          last loop2;
        }
        if ($in =~ /^&ugrave;/)
        {
          $result = join('',$result,pack("c",250));
          $in =~ s/^&ugrave;//;
          last loop2;
        }
        if ($in =~ /^&ucirc;/)
        {
          $result = join('',$result,pack("c",251));
          $in =~ s/^&ucirc;//;
          last loop2;
        }
        if ($in =~ /^&uuml;/)
        {
          $result = join('',$result,pack("c",252));
          $in =~ s/^&uuml;//;
          last loop2;
        }
        if ($in =~ /^&yacute;/)
        {
          $result = join('',$result,pack("c",253));
          $in =~ s/^&yacute;//;
          last loop2;
        }
        if ($in =~ /^&thorn;/)
        {
          $result = join('',$result,pack("c",254));
          $in =~ s/^&thorn;//;
          last loop2;
        }
        if ($in =~ /^&yuml;/)
        {
          $result = join('',$result,pack("c",255));
          $in =~ s/^&yuml;//;
          last loop2;
        }

        # we don't understand this escape sequence. Ignore it.
        # $in =~ s/^&.*;//;
        $result = join('',$result,substr($in,0,1));
        $in = substr($in,1);
        last loop2;
      } # end loop
    }
    else
    {
      # fist character of $in != '&'...
      $temp = substr($in,0,1);
      $in = substr($in,1);
      $result = join('',$result,$temp);
    }
  }

  $result;
}

1;
