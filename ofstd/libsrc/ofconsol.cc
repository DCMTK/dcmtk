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
 *  Update Date:      $Date: 2000-03-03 14:02:51 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofconsol.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "ofconsol.h"

#ifdef DCMTK_GUI

#ifdef HAVE_SSTREAM_H
  ostringstream CIN; 
  ostringstream COUT; 
  ostringstream CERR; 
  ostringstream CLOG; 
#else
  ostrstream CIN; 
  ostrstream COUT; 
  ostrstream CERR; 
  ostrstream CLOG; 
#endif

#else /* DCMTK_GUI */

/*
 * On DEC alpha the linker moans if a library is empty.
 * So define a dummy variable.
 */
char ofconsol_functionDefinedOnlyToStopLinkerMoaning;

#endif /* DCMTK_GUI */

/*
 *
 * CVS/RCS Log:
 * $Log: ofconsol.cc,v $
 * Revision 1.1  2000-03-03 14:02:51  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 *
 */
