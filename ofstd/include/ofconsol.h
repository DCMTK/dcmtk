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
 *  Purpose: Define general purpose facility for console output
 *
 *  class OFConsole and its global instance, ofConsole,
 *  provide access to the standard console output and error streams
 *  in a way that allows multiple threads to concurrently create output
 *  even if that output is redirected, e. g. to file or memory.
 *  Protection is implemented if the module is compiled with -D_REENTRANT
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
 *  streams are created as ostringstream or ostrstream (depending on
 *  whether <sstream.h> is found on the target system). This will allow
 *  a GUI based application to extract the messages and either present them
 *  to the user or store them in a log file.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-12-13 15:14:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofconsol.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFCONSOL_H
#define __OFCONSOL_H

#include "osconfig.h"
#include <iostream.h>
#include "ofthread.h"

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h> /* for NULL */
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

/** provides thread-safe access to the standard console output and error streams.
 *  Allows multiple threads to concurrently create output
 *  even if that output is redirected to file or memory.
 *  Protection is implemented if the module is compiled with -D_REENTRANT
 *  and is based on Mutexes.
 */

class OFConsole
{
public:

  /** default constructor. After construction, the cout methods refer to the
   *  standard output stream and the cerr methods refer to the standard
   *  error stream. If compiled with -DDCMTK_GUI, string streams named
   *  COUT and CERR are used instead.
   *  @param dummy used to "convince" linker of gcc 2.5.8 on NeXTSTEP to
   *         allocate memory for the global variable 'ofConsole'
   */
  OFConsole(int dummy = 0);

  /** destructor.
   */
  virtual ~OFConsole(){ }

  /** acquires a lock on the cout stream and returns a reference
   *  to the stream.
   *  @return reference to cout stream
   */
  ostream& lockCout()
  {
#ifdef _REENTRANT
    coutMutex.lock();
#endif
    return *currentCout;
  }

  /** releases the lock on the cout stream.
   */
  void unlockCout()
  {
#ifdef _REENTRANT
    coutMutex.unlock();
#endif
  }

  /** returns a reference to the current cout stream.
   *  This method neither locks nor unlocks the stream - the called
   *  must ensure that the stream is locked and unlocked appropriately.
   *  @return reference to cout stream
   */
  ostream& getCout()
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
   *  @return reference to replaced cout stream.
   */
  ostream& setCout(ostream *newCout=NULL);

  /** acquires a lock on the cerr stream and returns a reference
   *  to the stream.
   *  @return reference to cerr stream
   */
  ostream& lockCerr()
  {
#ifdef _REENTRANT
    cerrMutex.lock();
#endif
    if (joined)
    {
#ifdef _REENTRANT
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
  ostream& getCerr()
  {
    if (joined) return *currentCout;
    else return *currentCerr;
  }

  /** releases the lock on the cerr stream.
   */
  void unlockCerr()
  {
#ifdef _REENTRANT
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
   *  @return reference to replaced cerr stream.
   */
  ostream& setCerr(ostream *newCerr=NULL);

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

private:
  /** private undefined copy constructor */
  OFConsole(const OFConsole &arg);

  /** private undefined assignment operator */
  OFConsole& operator=(const OFConsole &arg);

  /** pointer to current cout stream, never NULL */
  ostream *currentCout;

  /** pointer to current cerr stream, never NULL */
  ostream *currentCerr;

  /** true if streams are combined, false otherwise */
  int joined;

#ifdef _REENTRANT
  /** mutex protecting access to cout */
  OFMutex coutMutex;

  /** mutex protecting access to cerr */
  OFMutex cerrMutex;
#endif
};


/**
 *  the global console object.
 *  Can safely be used by multiple threads concurrently.
 */
extern OFConsole ofConsole;


/*
 * definitions for COUT, CERR, CLOG.
 *
 * NOTE: DIRECT USE OF THESE MACROS IS UNSAFE IN MULTITHREAD APPLICATIONS.
 */

#ifdef DCMTK_GUI

#ifdef HAVE_SSTREAM_H
#include <sstream.h>
  extern ostringstream COUT;
  extern ostringstream CERR;
#else /* HAVE_SSTREAM_H */

#ifdef HAVE_STRSTREAM_H
#include <strstream.h>
#else
#include <strstrea.h>
#endif

  extern ostrstream COUT;
  extern ostrstream CERR;

#endif /* HAVE_SSTREAM_H */

#else /* DCMTK_GUI */

#define COUT cout
#define CERR cerr

#endif /* DCMTK_GUI */

#endif

/*
 *
 * CVS/RCS Log:
 * $Log: ofconsol.h,v $
 * Revision 1.7  2000-12-13 15:14:25  joergr
 * Introduced dummy parameter for "default" constructor of class OFConsole
 * to "convince" linker of gcc 2.5.8 (NeXTSTEP) to allocate memory for global
 * variable 'ofConsole'.
 *
 * Revision 1.6  2000/10/10 12:01:21  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.5  2000/09/26 13:46:12  meichel
 * Simplified inline code in ofconsol.h, required by Sun CC 2.x
 *
 * Revision 1.4  2000/06/21 15:47:54  meichel
 * Including stdlib.h, required for Sun CC 4.2
 *
 * Revision 1.3  2000/04/14 15:41:40  meichel
 * Added unprotected get methods, required for the cmdata debug facility.
 *
 * Revision 1.2  2000/04/14 15:16:08  meichel
 * Added new class OFConsole and global instance ofConsole which provide
 *   access to standard output and error streams in a way that allows multiple
 *   threads to safely create output concurrently.
 *
 * Revision 1.1  2000/03/03 14:02:47  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 *
 */
