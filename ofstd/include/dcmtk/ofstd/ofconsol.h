/*
 *
 *  Copyright (C) 1999-2011, OFFIS e.V.
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
 *  Purpose: Define general purpose facility for console output
 *
 *  class OFConsole and its global instance, ofConsole,
 *  provide access to the standard console output and error streams
 *  in a way that allows multiple threads to concurrently create output
 *  even if that output is redirected, e. g. to file or memory.
 *  Protection is implemented if the module is compiled with -DWITH_THREADS
 *  and is based on Mutexes.
 *
 *  In cases where DCMTK is used for GUI development, the fact that the
 *  libraries send many error messages to the standard or error streams
 *  are annoying since these streams are not present in a GUI environment.
 *  Either the messages just go lost or they even cause the GUI
 *  application to fail.  This file introduces aliases for the standard
 *  stream handles called COUT and CERR, which are normally only
 *  preprocessor macros for cout and cerr, respectively. If the
 *  toolkit is compiled with the flag DCMTK_GUI defined, however, these
 *  streams are created as OFOStringStream. This will allow a GUI based
 *  application to extract the messages and either present them to the
 *  user or store them in a log file.
 *
 *  GUI based applications making use of this feature should periodically
 *  check and clear these streams in order to avoid increasing consumption
 *  of heap memory.
 *
 *  Caveat 1: The DCMTK command line tools do not yet support the DCMTK_GUI
 *  flag, and will most likely exhibit all kinds of undesired behaviour
 *  if this flag is used.
 *
 *  Caveat 2: The direct use of the COUT and CERR macros is unsafe
 *  in multithread applications. Use ofConsole instead.
 *
 */


#ifndef OFCONSOL_H
#define OFCONSOL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofthread.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"


/** Singleton class which provides thread-safe access to the standard console
 *  output and error streams. Allows multiple threads to concurrently create
 *  output even if that output is redirected to file or memory.
 *  Protection is implemented if the module is compiled with -DWITH_THREADS
 *  and is based on Mutexes.
 *  Use of the singleton prior to start of main (i.e. from global constructors)
 *  is allowed, but any use after the end of main is undefined.
 */
class DCMTK_OFSTD_EXPORT OFConsole
{
public:

  /** destructor.
   */
  virtual ~OFConsole(){ }

  /** acquires a lock on the cout stream and returns a reference
   *  to the stream.
   *  @return reference to cout stream
   */
  STD_NAMESPACE ostream& lockCout()
  {
#ifdef WITH_THREADS
    coutMutex.lock();
#endif
    return *currentCout;
  }

  /** releases the lock on the cout stream.
   */
  void unlockCout()
  {
#ifdef WITH_THREADS
    coutMutex.unlock();
#endif
  }

  /** returns a reference to the current cout stream.
   *  This method neither locks nor unlocks the stream - the called
   *  must ensure that the stream is locked and unlocked appropriately.
   *  @return reference to cout stream
   */
  STD_NAMESPACE ostream& getCout()
  {
    return *currentCout;
  }

  /** exchanges the cout stream object.
   *  This method acquires its own lock. Cout must not
   *  be locked by the calling thread, otherwise a deadlock may occur.
   *  The caller must ensure that the same stream object is not set
   *  both as cout and cerr because this might result in a conflict
   *  if one thread locks and uses cout, and another one locks and uses cerr.
   *  Use the join() method instead, see below.
   *  @param newCout new cout stream, default: restore the stream that was
   *         active upon creation of the console object.
   *  @return pointer to replaced cout stream.
   */
  STD_NAMESPACE ostream *setCout(STD_NAMESPACE ostream *newCout=NULL);

  /** acquires a lock on the cerr stream and returns a reference
   *  to the stream.
   *  @return reference to cerr stream
   */
  STD_NAMESPACE ostream& lockCerr()
  {
#ifdef WITH_THREADS
    cerrMutex.lock();
#endif
    if (joined)
    {
#ifdef WITH_THREADS
      coutMutex.lock();
#endif
      return *currentCout;
    }
    else return *currentCerr;
  }

  /** returns a reference to the current cerr stream.
   *  This method neither locks nor unlocks the stream - the called
   *  must ensure that the stream is locked and unlocked appropriately.
   *  @return reference to cerr stream
   */
  STD_NAMESPACE ostream& getCerr()
  {
    if (joined) return *currentCout;
    else return *currentCerr;
  }

  /** releases the lock on the cerr stream.
   */
  void unlockCerr()
  {
#ifdef WITH_THREADS
    if (joined) coutMutex.unlock();
    cerrMutex.unlock();
#endif
  }

  /** exchanges the cerr stream object.
   *  This method acquires its own lock. Cerr must not
   *  be locked by the calling thread, otherwise a deadlock may occur.
   *  The caller must ensure that the same stream object is not set
   *  both as cout and cerr because this might result in a conflict
   *  if one thread locks and uses cout, and another one locks and uses cerr.
   *  Use the join() method instead, see below.
   *  @param newCerr new cerr stream, default: restore the stream that was
   *         active upon creation of the console object.
   *  @return pointer to replaced cerr stream.
   */
  STD_NAMESPACE ostream *setCerr(STD_NAMESPACE ostream *newCerr=NULL);

  /** combines the cerr and cout streams.
   *  After a call to this method, both cout and cerr related methods
   *  lock, unlock and return the cout stream.
   *  This method acquires its own locks. Neither cout nor cerr may
   *  be locked by the calling thread, otherwise a deadlock may occur.
   */
  void join();

  /** splits combined cerr and cout streams.
   *  After a call to this method, cout and cerr related methods
   *  again lock, unlock and return different cout and cerr objects.
   *  This method acquires its own locks. Neither cout nor cerr may
   *  be locked by the calling thread, otherwise a deadlock may occur.
   */
  void split();

  /** Checks whether cout and cerr are currently combined.
   *  This method acquires its own locks. Neither cout nor cerr may
   *  be locked by the calling thread, otherwise a deadlock may occur.
   *  @return OFTrue if streams are combined, OFFalse otherwise.
   */
  OFBool isJoined();

  /** returns the singleton instance of this class.
   *  May be called before main() but not after end of main
   */
  static OFConsole& instance();

private:

  /** default constructor. After construction, the cout methods refer to the
   *  standard output stream and the cerr methods refer to the standard
   *  error stream. If compiled with -DDCMTK_GUI, string streams named
   *  COUT and CERR are used instead.
   */
  OFConsole();

  /** private undefined copy constructor */
  OFConsole(const OFConsole &arg);

  /** private undefined assignment operator */
  OFConsole& operator=(const OFConsole &arg);

  /** pointer to current cout stream, never NULL */
  STD_NAMESPACE ostream *currentCout;

  /** pointer to current cerr stream, never NULL */
  STD_NAMESPACE ostream *currentCerr;

  /** true if streams are combined, false otherwise */
  int joined;

#ifdef WITH_THREADS
  /** mutex protecting access to cout */
  OFMutex coutMutex;

  /** mutex protecting access to cerr */
  OFMutex cerrMutex;
#endif

  // dummy declaration to keep gcc quiet
  friend class OFConsoleDummyFriend;
};


/** macro for accessing the glocal console object.
 *  This used to be an external global variable in earlier DCMTK releases
 */
#define ofConsole (OFConsole::instance())

/*
 * definitions for COUT, CERR, CLOG.
 *
 * NOTE: DIRECT USE OF THESE MACROS IS UNSAFE IN MULTITHREAD APPLICATIONS.
 */

#ifdef DCMTK_GUI

extern DCMTK_OFSTD_EXPORT OFOStringStream COUT;
extern DCMTK_OFSTD_EXPORT OFOStringStream CERR;

#else /* DCMTK_GUI */

#define COUT (ofConsole.getCout())
#define CERR (ofConsole.getCerr())

#endif /* DCMTK_GUI */

#endif /* OFCONSOL_H */
