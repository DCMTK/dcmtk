/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Author:  Andreas Barth
 *
 *  Purpose: C++ header to handle standard and old stream libraries.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-16 13:36:03 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstream.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFSTREAM_H
#define __OFSTREAM_H

#include "osconfig.h"

#ifdef USE_STD_CXX_INCLUDES
#include <iostream>
#ifdef HAVE_IOS
#include <ios>
#endif
#include <strstream>
#include <fstream>
#include <iomanip>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
#else /* USE_STD_CXX_INCLUDES */
#include <iostream.h>
#include <fstream.h>
#ifdef HAVE_STRSTREA_H
#include <strstrea.h>
#endif
#ifdef HAVE_STRSTREAM_H
#include <strstream.h>
#endif
#ifdef HAVE_SSTREAM_H
#include <sstream.h>
#endif
#include <iomanip.h>
#endif

#endif /* USE_STD_CXX_INCLUDES */


/*
 *
 * CVS/RCS Log:
 * $Log: ofstream.h,v $
 * Revision 1.1  2002-04-16 13:36:03  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 *
 *
 */
