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
#   This module contains perl procedures which allow to set/unset
#   read and write locks on one certain file (which is stored in a
#   global variable).
#   The procedures use the perl fcntl() call instead of flock() in order
#   to be "compatible" with the dcmtk Basic Worklist Management components
#   written in C++, on both BSD and System V platforms.
#   Since the "struct flock" required for fcntl() is defined differently
#   on every system, this perl script relies on a C program "preplock" which
#   initialises the flock structure from <fcntl.h> and passes the result
#   to perl.
#
# Last Update:      $Author: joergr $
# Update Date:      $Date: 2010-10-14 13:02:01 $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: lock.ph,v $
#   Revision 1.2  2010-10-14 13:02:01  joergr
#   Updated copyright header. Added reference to COPYRIGHT file.
#
#   Revision 1.1  2002/12/03 12:16:06  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#

require 'prefs.ph';

$fcntldata=`$prefs{'preplock'}`;
eval $fcntldata;

$fd_inuse=0;   # 0=no_lock, 1=read_lock, 2=write_lock

#
# usage: &set_readlock('pathname');
#   returns 1 if successful, 0 upon failure.
#   modifies the global variable $fd_inuse and the file descriptor fd.
#
sub set_readlock
{
  local($lockname, $last, $fcntldata);

  if ((@_ != 1)||($_[0] eq ''))
  {
    printf(stderr "error: NULL path not allowed\n");
    return 0;
  }
  $lockname = $_[0];

  if ($fd_inuse ne 0)
  {
    printf(stderr "error: nested read locks not allowed!\n");
    return 0;
  }

  $last = chop($lockname);
  if ($last eq '/')
  {
    $lockname = join('','+<',$lockname,$last,$prefs{'lockfile'});
  } else {
    $lockname = join('','+<',$lockname,$last,'/',$prefs{'lockfile'});
  }

  if (!open(fd, $lockname))
  {
     $fd_inuse = 0;
     printf(stderr "error: cannot open file %s\n",$lockname);
     return 0;
  } else {
    $fd_inuse=1;
  }

  $fcntldata = $const_RDLCK_STRUCT;

  if (-1 eq fcntl(fd,$const_F_SETLKW, $fcntldata))
  {
    printf(stderr "error: cannot set read lock on file %s\n",$lockname);
    close(fd);
    $fd_inuse = 0;
    return 0;
  }

  return 1;
}

#
# usage: &set_writelock('pathname');
#   returns 1 if successful, 0 upon failure.
#   modifies the global variable $fd_inuse and the file descriptor fd.
#
sub set_writelock
{
  local($lockname, $last, $fcntldata);

  if ((@_ != 1)||($_[0] eq ''))
  {
    printf(stderr "error: NULL path not allowed\n");
    return 0;
  }
  $lockname = $_[0];

  if ($fd_inuse eq 2)
  {
    printf(stderr "error: nested write locks not allowed!\n");
    return 0;
  }

  $last = chop($lockname);
  if ($last eq '/')
  {
    $lockname = join('','+<',$lockname,$last,$prefs{'lockfile'});
  } else {
    $lockname = join('','+<',$lockname,$last,'/',$prefs{'lockfile'});
  }

  if ($fd_inuse eq 0)
  {
    if (!open(fd, $lockname))
    {
       $fd_inuse = 0;
       printf(stderr "error: cannot open file %s\n",$lockname);
       return 0;
    }
  }

  $fcntldata = $const_WRLCK_STRUCT;

  if (-1 eq fcntl(fd,$const_F_SETLKW, $fcntldata))
  {
    printf(stderr "error: cannot set write lock on file %s\n",$lockname);
    close(fd);
    $fd_inuse = 0;
    return 0;
  }

  $fd_inuse=2;
  return 1;
}

#
# usage: &release_lock;
#   returns 1 if successful, 0 upon failure.
#   modifies the global variable $fd_inuse and the file descriptor fd.
#
sub release_lock
{
  local($lockname, $last, $fcntldata);

  if ($fd_inuse eq 0)
  {
    printf(stderr "error: no lock to release!\n");
    return 0;
  }

  $fcntldata = $const_UNLCK_STRUCT;

  if (-1 eq fcntl(fd,$const_F_SETLKW, $fcntldata))
  {
    printf(stderr "error: cannot release lock.\n");
    close(fd);
    $fd_inuse = 0;
    return 0;
  }

  $fd_inuse = 0;
  return 1;
}

1;
