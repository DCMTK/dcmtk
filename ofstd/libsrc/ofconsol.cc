/*
 *
 *  Copyright (C) 1999-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Define alias for cout, cerr and clog
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-04-14 15:16:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofconsol.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include <assert.h>

#include "ofconsol.h"
#include "ofthread.h"

#ifdef DCMTK_GUI
#ifdef HAVE_SSTREAM_H
  ostringstream COUT; 
  ostringstream CERR; 
#else
  ostrstream COUT; 
  ostrstream CERR; 
#endif
#endif

/* global console object */
OFConsole ofConsole;


OFConsole::OFConsole()
#ifdef DCMTK_GUI
: currentCout(&COUT)
, currentCerr(&CERR)
#else
: currentCout(&cout)
, currentCerr(&cerr)
#endif
, joined(0)
#ifdef _REENTRANT
, coutMutex()
, cerrMutex()
#endif
{
}

ostream& OFConsole::setCout(ostream *newCout)
{
  lockCout();
  ostream *tmpCout = currentCout;
#ifdef DCMTK_GUI
  if (newCout) currentCout = newCout; else currentCout = &COUT;
#else
  if (newCout) currentCout = newCout; else currentCout = &cout;
#endif
  unlockCout();
  return *tmpCout;
}

ostream& OFConsole::setCerr(ostream *newCerr)
{
  lockCerr();
  ostream *tmpCerr = currentCerr;
#ifdef DCMTK_GUI
  if (newCerr) currentCerr = newCerr; else currentCerr = &CERR;
#else
  if (newCerr) currentCerr = newCerr; else currentCerr = &cerr;
#endif
  unlockCerr();
  return *tmpCerr;
}

void OFConsole::join()
{
  lockCerr();
  if (!joined)
  {
    // changing the state of "joined" requires that both mutexes are locked.
    // Mutexes must always be locked in the same order to avoid deadlocks.
    lockCout();
    joined = 1;
    unlockCout();
  }
  unlockCerr();
  return;
}
  
void OFConsole::split()
{
  lockCerr();
  if (joined)
  {
    // changing the state of "joined" requires that both mutexes are locked.
    // Mutexes must always be locked in the same order to avoid deadlocks.
    lockCout();
    joined = 0;
    unlockCout();
  }
  unlockCerr();
  return;
}

OFBool OFConsole::isJoined()
{
  lockCerr();
  // nobody will change "joined" if we have locked either mutex
  int result = joined;
  unlockCerr();
  if (result) return OFTrue; else return OFFalse;
}

/*
 *
 * CVS/RCS Log:
 * $Log: ofconsol.cc,v $
 * Revision 1.2  2000-04-14 15:16:13  meichel
 * Added new class OFConsole and global instance ofConsole which provide
 *   access to standard output and error streams in a way that allows multiple
 *   threads to safely create output concurrently.
 *
 * Revision 1.1  2000/03/03 14:02:51  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 *
 */
