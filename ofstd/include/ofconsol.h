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
 *  In cases where DCMTK is used for GUI development, the fact that the 
 *  libraries send many error messages to the standard or error streams 
 *  are annoying since these streams are not present in a GUI environment.
 *  Either the messages just go lost or they even cause the GUI 
 *  application to fail.  This file introduces aliases for the standard 
 *  stream handles called COUT, CERR, CLOG, which are normally only 
 *  preprocessor macros for cout, cerr and clog, respectively. If the 
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
 *  Caveat: The DCMTK command line tools do not yet support the DCMTK_GUI
 *  flag, and will most likely exhibit all kinds of undesired behaviour
 *  if this flag is used.
 *
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:02:47 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofconsol.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFCONSOL_H
#define __OFCONSOL_H

#include "osconfig.h"
#include <iostream.h>


#ifdef DCMTK_GUI

#ifdef HAVE_SSTREAM_H
#include <sstream.h>
  extern ostringstream COUT; 
  extern ostringstream CERR; 
  extern ostringstream CLOG; 
#else /* HAVE_SSTREAM_H */

#ifdef HAVE_STRSTREAM_H     
#include <strstream.h>
#else
#include <strstrea.h>
#endif

  extern ostrstream COUT; 
  extern ostrstream CERR; 
  extern ostrstream CLOG; 

#endif /* HAVE_SSTREAM_H */

#else /* DCMTK_GUI */

#define COUT cout
#define CERR cerr
#define CLOG clog 

#endif /* DCMTK_GUI */

#endif

/*
 *
 * CVS/RCS Log:
 * $Log: ofconsol.h,v $
 * Revision 1.1  2000-03-03 14:02:47  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 *
 */
