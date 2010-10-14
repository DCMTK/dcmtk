/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Define alias for cout, cerr and clog
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:53 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofthread.h"

#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"


#ifdef DCMTK_GUI
  OFOStringStream COUT;
  OFOStringStream CERR;
#endif

OFConsole::OFConsole()
#ifdef DCMTK_GUI
: currentCout(&COUT)
, currentCerr(&CERR)
#else
: currentCout(&STD_NAMESPACE cout)
, currentCerr(&STD_NAMESPACE cerr)
#endif
, joined(0)
#ifdef WITH_THREADS
, coutMutex()
, cerrMutex()
#endif
{
}

STD_NAMESPACE ostream *OFConsole::setCout(STD_NAMESPACE ostream *newCout)
{
  lockCout();
  STD_NAMESPACE ostream *tmpCout = currentCout;
#ifdef DCMTK_GUI
  if (newCout) currentCout = newCout; else currentCout = &COUT;
#else
  if (newCout) currentCout = newCout; else currentCout = &STD_NAMESPACE cout;
#endif
  unlockCout();
  return tmpCout;
}

STD_NAMESPACE ostream *OFConsole::setCerr(STD_NAMESPACE ostream *newCerr)
{
  lockCerr();
  STD_NAMESPACE ostream *tmpCerr = currentCerr;
#ifdef DCMTK_GUI
  if (newCerr) currentCerr = newCerr; else currentCerr = &CERR;
#else
  if (newCerr) currentCerr = newCerr; else currentCerr = &STD_NAMESPACE cerr;
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
  }

  // now status is joined, so unlockCerr implicitly unlocks both mutexes
  unlockCerr();
  return;
}

void OFConsole::split()
{
  lockCerr();
  if (joined)
  {
    // since status is joined, lockCerr() has locked both mutexes
    joined = 0;

    // now status is unjoined, we have to unlock both mutexes manually
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
 * Revision 1.15  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-10-04 14:44:49  joergr
 * Replaced "#ifdef _REENTRANT" by "#ifdef WITH_THREADS" where appropriate (i.e.
 * in all cases where OFMutex, OFReadWriteLock, etc. are used).
 *
 * Revision 1.13  2006/08/14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.12  2005/12/08 15:48:53  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2004/01/16 10:35:09  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.10  2002/11/27 11:23:09  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.9  2002/06/14 10:44:41  meichel
 * Fixed bug in ofConsole join/split mutex locking behaviour
 *
 * Revision 1.8  2002/05/16 15:56:35  meichel
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
 *
 * Revision 1.5  2002/04/16 13:36:26  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
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
