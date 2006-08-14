/*
 *
 *  Copyright (C) 1999-2006, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Define general purpose facility for log file output
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2006-08-14 16:42:46 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/Attic/oflogfil.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflogfil.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


OFLogFile::OFLogFile(const char *filename, int flags)
#ifdef HAVE_DECLARATION_STD__IOS_BASE__OPENMODE
  : File(filename, OFstatic_cast(std::ios_base::openmode, flags))
#else
  : File(filename, flags)
#endif
#ifdef DEBUG
  , Filter(LL_debug)
#else
  , Filter(LL_error)
#endif
#ifdef _REENTRANT
  , Mutex()
#endif
{
}


STD_NAMESPACE ofstream &OFLogFile::lockFile(LF_Level level, const char *module)
{
#ifdef _REENTRANT
    Mutex.lock();
#endif
    if (checkFilter(level))
    {
        OFDateTime dateTime;
        dateTime.setCurrentDateTime();
        File << dateTime << ", Level: ";
        switch (level)
        {
          case LL_warning:
             File << "WARNING";
              break;
          case LL_error:
             File << "ERROR";
              break;
          case LL_debug:
             File << "DEBUG";
              break;
          default:
             File << "INFO";
        }
        if ((module != NULL) && (strlen(module) > 0))
            File << ", Module: " << module << OFendl; 
    }
    return File;
}


void OFLogFile::writeMessage(const char *message, int indent)
{
    if (message != NULL)
    {
        size_t pos = 0;
        OFString msgStr = message;
        while (((pos = (msgStr.find('\n', pos))) != OFString_npos) && (pos < msgStr.length()))
            msgStr.insert(++pos, OFString(indent, ' '));
        File << OFString(indent, ' ') << msgStr << OFendl;
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: oflogfil.cc,v $
 * Revision 1.11  2006-08-14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.10  2005/12/08 15:48:59  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2003/07/09 13:58:04  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.8  2003/06/11 12:21:37  meichel
 * Fixed configure test for std::ios_base::openmode
 *
 * Revision 1.7  2002/11/27 11:23:10  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.6  2002/04/11 12:14:03  joergr
 * Introduced new standard classes providing date and time functions.
 *
 * Revision 1.5  2001/11/02 12:04:10  meichel
 * Added new configure tests for std::_Ios_Openmode and ios::nocreate,
 *   required for gcc 3.0.x.
 *
 * Revision 1.4  2001/09/28 12:41:58  joergr
 * Use typecast std::_Ios_Openmode when available (required for gcc 3.0).
 *
 * Revision 1.3  2001/06/01 15:51:39  meichel
 * Updated copyright header
 *
 * Revision 1.2  2000/12/12 17:20:02  joergr
 * Changed type of stream 'open_mode' from long to int to avoid compiler
 * warnings reported by SunCC 2.0.1.
 *
 * Revision 1.1  2000/06/05 16:16:40  joergr
 * Added new class for writing standardized status messages to a log file.
 *
 *
 *
 */
