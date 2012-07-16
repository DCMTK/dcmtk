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
