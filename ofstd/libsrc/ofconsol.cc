/*
 *
 *  Copyright (C) 1999-2002, OFFIS
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
 *  Update Date:      $Date: 2002-05-16 15:56:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofconsol.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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
  OFOStringStream COUT;
  OFOStringStream CERR;
#endif

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

ostream *OFConsole::setCout(ostream *newCout)
{
  lockCout();
  ostream *tmpCout = currentCout;
#ifdef DCMTK_GUI
  if (newCout) currentCout = newCout; else currentCout = &COUT;
#else
  if (newCout) currentCout = newCout; else currentCout = &cout;
#endif
  unlockCout();
  return tmpCout;
}

ostream *OFConsole::setCerr(ostream *newCerr)
{
  lockCerr();
  ostream *tmpCerr = currentCerr;
#ifdef DCMTK_GUI
  if (newCerr) currentCerr = newCerr; else currentCerr = &CERR;
#else
  if (newCerr) currentCerr = newCerr; else currentCerr = &cerr;
#endif
  unlockCerr();
  return tmpCerr;
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

OFConsole& OFConsole::instance()
{
  static OFConsole instance_;
  return instance_;
}


class OFConsoleInitializer
{
public:
  OFConsoleInitializer()
  {
  	OFConsole::instance();
  }  
};

/* the constructor of this global object makes sure
 * that ofConsole is initialized before main starts.
 * Required to make ofConsole thread-safe.
 */
OFConsoleInitializer ofConsoleInitializer;


/*
 *
 * CVS/RCS Log:
 * $Log: ofconsol.cc,v $
 * Revision 1.8  2002-05-16 15:56:35  meichel
 * Changed ofConsole into singleton implementation that can safely
 *   be used prior to start of main, i.e. from global constructors
 *
 * Revision 1.7  2002/05/16 08:16:46  meichel
 * changed return type of OFConsole::setCout() and OFConsole::setCerr()
 *   to pointer instead of reference.
 *
 * Revision 1.6  2002/05/02 14:06:07  joergr
 * Added support for standard and non-standard string streams (which one is
 * supported is detected automatically via the configure mechanism).
 * Thanks again to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.5  2002/04/16 13:36:26  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.4  2001/06/01 15:51:38  meichel
 * Updated copyright header
 *
 * Revision 1.3  2000/12/13 15:14:35  joergr
 * Introduced dummy parameter for "default" constructor of class OFConsole
 * to "convince" linker of gcc 2.5.8 (NeXTSTEP) to allocate memory for global
 * variable 'ofConsole'.
 *
 * Revision 1.2  2000/04/14 15:16:13  meichel
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
