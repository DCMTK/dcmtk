/*
 *
 *  Copyright (C) 2000-2006, OFFIS
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
 *  Update Date:      $Date: 2006-08-14 16:42:26 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFLOGFIL_H
#define OFLOGFIL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"


/** Class that provides a general purpose facility for writing log files.
 *  Protection for simultaneous access from different threads is implemented
 *  if the module is compiled with -D_REENTRANT.
 */
class OFLogFile
{
 public:

    /** describes the different status levels used for the log file messages
     */
    enum LF_Level
    {
        /// no log messages (only used as a filter)
        LL_none = 0,
        /// only error messages
        LL_error = 1,
        /// warning messages (includes LL_error)
        LL_warning = 2,
        /// informational messages (includes LL_warning)
        LL_informational = 3,
        /// debug messages (includes LL_informational)
        LL_debug = 4
    };

    /** constructor, creates a log file of the specified name.
     *  Use method good() to check whether the output file could be created.
     *  @param filename name of the log file to be created
     *  @param flags file type used to create the file (default: append)
     */
    OFLogFile(const char *filename, int flags = STD_NAMESPACE ios::app);

    /** destructor
     */
    virtual ~OFLogFile() {}

    /** acquires a lock on the log file stream and returns a reference
     *  to the stream. An optional header (incl. date, time, status level
     *  and module name) is automatically written to the stream if level
     *  is not LL_none.
     *  @param level status level of the following message (optional)
     *  @param module name of the module the message belongs to (optional)
     *  @return reference to log file stream (used to write the message)
     */
    STD_NAMESPACE ofstream &lockFile(LF_Level level = LL_none, const char *module = NULL);

    /** releases the lock on the log file stream.
     */
    void unlockFile()
    {
        File.flush();
#ifdef _REENTRANT
        Mutex.unlock();
#endif
    }

    /** checks whether log file could be created.
     *  @return OFTrue if file status is OK, OFFalse otherwise
     */
    OFBool good()
    {
#ifdef _REENTRANT
        Mutex.lock();
#endif
        OFBool status = File.good() ? OFTrue : OFFalse;
#ifdef _REENTRANT
        Mutex.unlock();
#endif
        return status;
    }

    /** returns a reference to the current log file stream.
     *  This method neither locks nor unlocks the stream - the called
     *  must ensure that the stream is locked and unlocked appropriately.
     *  @return reference to log file stream
     */
    STD_NAMESPACE ostream& getFile()
    {
        return File;
    }

    /** writes a message to the log file stream.
     *  Each line has an indentation of the specified number of spaces.
     *  This method neither locks nor unlocks the stream - the called
     *  must ensure that the stream is locked and unlocked appropriately.
     *  @param message text to be written to the stream (newline with '\n')
     *  @param indent number of spaces each line is started with (optional)
     */
    void writeMessage(const char *message, int indent = 3);

    /** sets a filter to specify which messages are actually written to the log file.
     *  There are five different levels (in ascending order): none, informational, warning,
     *  error, debug. All messages which belong to a 'lower' level are included in the
     *  higher levels, i.e. the level debug includes all messages.
     *  Since this method uses the class mutex the filter cannot be changed while the file
     *  access is locked.
     *  @param level status level specifying the filter
     */
    void setFilter(LF_Level level)
    {
#ifdef _REENTRANT
        Mutex.lock();
#endif
        Filter = level;
#ifdef _REENTRANT
        Mutex.unlock();
#endif
    }

    /** gets the currently active filter.
     *  @return status level specifying the filter
     */
    LF_Level getFilter()
    {
        return Filter;
    }

    /** checks whether the current filter allows out of messages of the given status level.
     *  @param level status level to be checked
     *  @return OFTrue if message should be written, OFFalse otherwise
     */
    OFBool checkFilter(LF_Level level)
    {
        return (level != LL_none) && (level <= Filter);
    }


 private:

    /** private undefined copy constructor */
    OFLogFile(const OFLogFile &arg);

    /** private undefined assignment operator */
    OFLogFile& operator=(const OFLogFile &arg);

    /** log file stream */
    STD_NAMESPACE ofstream File;

    /** log message filter */
    LF_Level Filter;

#ifdef _REENTRANT
    /** mutex protecting access to member variables */
    OFMutex Mutex;
#endif
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oflogfil.h,v $
 * Revision 1.14  2006-08-14 16:42:26  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.13  2005/12/08 16:05:59  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.12  2004/01/16 10:30:12  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.11  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.10  2003/06/12 13:14:29  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.9  2003/06/11 13:26:09  meichel
 * Cleaned up usage of boolean constants
 *
 * Revision 1.8  2002/11/27 11:23:05  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.7  2002/05/16 15:56:18  meichel
 * Minor fixes to make ofstd compile on NeXTStep 3.3
 *
 * Revision 1.6  2002/05/14 08:12:29  joergr
 * Updated comments.
 *
 * Revision 1.5  2002/04/16 13:36:03  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.4  2001/06/01 15:51:34  meichel
 * Updated copyright header
 *
 * Revision 1.3  2000/12/12 17:19:57  joergr
 * Changed type of stream 'open_mode' from long to int to avoid compiler
 * warnings reported by SunCC 2.0.1.
 *
 * Revision 1.2  2000/06/21 15:47:54  meichel
 * Including stdlib.h, required for Sun CC 4.2
 *
 * Revision 1.1  2000/06/05 16:16:23  joergr
 * Added new class for writing standardized status messages to a log file.
 *
 *
 */
