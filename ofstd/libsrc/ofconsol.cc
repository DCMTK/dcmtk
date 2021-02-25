/*
 *
 *  Copyright (C) 2000-2021, OFFIS e.V.
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

#include <cassert>
#ifdef HAVE_UNISTD_H
BEGIN_EXTERN_C
#include <unistd.h>
END_EXTERN_C
#endif


BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
END_EXTERN_C


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

int OFConsole::old_stderr = -1;

void OFConsole::mergeStderrStdout()
{
    fflush(stderr);
    if (fileno(stderr) != fileno(stdout))
    {
        /* duplicate the stderr file descriptor to be the same as stdout */
        if (old_stderr < 0) old_stderr = dup(fileno(stderr));

        /* now duplicate the file descriptor of stdout into the file descriptor of stderr.
         * This will silently close the previous file descriptor of stderr.
         */
        if (0 != dup2(fileno(stdout), fileno(stderr)))
        {
            OFConsole::instance().lockCerr() << "Unable to redirect stderr to stdout" << OFendl;
            OFConsole::instance().unlockCerr();
        }
    }

#ifndef __BORLANDC__  /* setvbuf on stdout/stderr does not work with Borland C++ */
    /* set stdout and stderr to unbuffered mode */
    if (setvbuf(stdout, NULL, _IONBF, 0 ) != 0 )
    {
        OFConsole::instance().lockCerr() << "Unable to switch stdout to unbuffered mode" << OFendl;
        OFConsole::instance().unlockCerr();
    }
    if (setvbuf(stderr, NULL, _IONBF, 0 ) != 0 )
    {
        OFConsole::instance().lockCerr() << "Unable to switch stderr to unbuffered mode" << OFendl;
        OFConsole::instance().unlockCerr();
    }
#endif /* __BORLANDC__ */
}


void OFConsole::unmergeStderrStdout()
{
    /* only execute this code if stderr was actually redirected before */
    if (old_stderr > 0)
    {
        if (0 != dup2(old_stderr, fileno(stderr)))
        {
            OFConsole::instance().lockCerr() << "Error: Unable to release redirection of stderr to stdout" << OFendl;
            OFConsole::instance().unlockCerr();
        }

#ifndef __BORLANDC__
        /* switch stdout to buffered mode */
        if (setvbuf(stdout, NULL, _IOFBF, BUFSIZ ) != 0 )
        {
            OFConsole::instance().lockCerr() << "Error: Unable to switch stdout to buffered mode" << OFendl;
            OFConsole::instance().unlockCerr();

        }
#endif /* __BORLANDC__ */
    }
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
