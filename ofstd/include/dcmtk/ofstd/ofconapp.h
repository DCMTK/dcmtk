/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Handle console applications (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCONAPP_H
#define OFCONAPP_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** support class for console applications.
 *  Builds an envelope for the OFCommandLine class to provide a consistent
 *  behaviour for all DCMTK console applications.
 *  Performs console output operations and error checking.
 */
class OFConsoleApplication
{

 public:

    /** constructor
     *
     ** @param  app     application name
     *  @param  desc    (short) description of what the application does
     *  @param  rcsid   identifier for console application
     */
    OFConsoleApplication(const char *app,
                         const char *desc = NULL,
                         const char *rcsid = NULL);

    /** destructor
     */
    ~OFConsoleApplication();

    /** parse command line.
     *  If the command line has no argument (in case at least one argument is required) and
     *  if the command line has only one argument, namely "--help" or the specified shortcut,
     *  (in all cases) the usage is printed (see printUsage).
     *
     ** @param  cmd       reference to the OFCommandLine object
     *  @param  argCount  number of arguments (argc)
     *  @param  argValue  pointer to argument array (argv[])
     *  @param  flags     flags to be used for parsing (e.g. OFCommandLine::PF_ExpandWildcards)
     *  @param  startPos  first argument to be parsed (default: 1, i.e. omit program name)
     *
     ** @return status of parsing process, true if successful, false otherwise
     */
    OFBool parseCommandLine(OFCommandLine &cmd,
                            int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

    /** print header of console application (consisting of identifier, name and description)
     *
     ** @param  hostInfo  print host information as reported by 'config.guess' if OFTrue
     *  @param  stdError  print to standard error stream if OFTrue (default: standard output)
     */
    void printHeader(const OFBool hostInfo = OFFalse,
                     const OFBool stdError = OFFalse);

    /** print usage (syntax of command line options) to standard output stream
     *
     ** @param  cmd   reference to command line class (default: object used for parsing)
     */
    void printUsage(const OFCommandLine *cmd = NULL);

    /** print expanded command line arguments to standard error stream
     *
     ** @param  cmd   reference to command line class (default: object used for parsing)
     */
    void printArguments(OFCommandLine *cmd = NULL);

    /** print resource identifier to standard error stream
     */
    void printIdentifier();

    /** print error message (incl. header) to standard error stream and exit with error code
     *
     ** @param  str   error message to be printed
     *  @param  code  error code to be returned (exit)
     */
    void printError(const char *str,
    				const int code = 1);

    /** print warning message (w/o header) to standard error stream
     *
     ** @param  str     warning message to be printed
     *  @param  prefix  prefix printed in front of the message
     */
    void printWarning(const char *str,
                      const char *prefix = "warning");

    /** print message (w/o header) to standard error stream.
     *
     ** @param  str  message to be printed
     */
    void printMessage(const char *str);

    /** check quiet mode
     *
     ** @return OFTrue if quiet mode is active, OFFalse otherwise
     */
    OFBool quietMode() const;

    /** switch on/off quiet mode
     *
     ** @param  mode  if OFTrue all error and warning messages will be suppressed
     */
    void setQuietMode(const OFBool mode = OFTrue);

    /** check value status and print error message if necessary
     *
     ** @param  status  status of OFCommandLine::getValue() call
     *  @param  cmd     reference to command line class (default: object used for parsing)
     */
    void checkValue(const OFCommandLine::E_ValueStatus status,
                    OFCommandLine *cmd = NULL);

    /** check parameter status and print error message if necessary
     *
     ** @param  status  status of OFCommandLine::getParam() call
     *  @param  cmd     reference to command line class (default: object used for parsing)
     */
    void checkParam(const OFCommandLine::E_ParamValueStatus status,
                    OFCommandLine *cmd = NULL);

    /** check dependence between sub and base option and report an error if required.
     *  Message format: "error: " + subOpt + " only allowed with " + baseOpt
     *
     ** @param  subOpt     option to be checked
     *  @param  baseOpt    base option required for sub option
     *  @param  condition  if false error message is printed (i.e. base option is absent)
     */
    void checkDependence(const char *subOpt,
                         const char *baseOpt,
                         OFBool condition);

    /** check conflict between two options and report an error if required.
     *  Message format: "error: " + firstOpt + " not allowed with " + secondOpt
     *
     ** @param  firstOpt   first option to be checked
     *  @param  secondOpt  second option to be checked
     *  @param  condition  if true error message is printed (i.e. a conflict exists)
     */
    void checkConflict(const char *firstOpt,
                       const char *secondOpt,
                       OFBool condition);


 private:

    /// Name of the application (short form)
    OFString Name;
    /// Short description of the application
    OFString Description;
    /// Identification string (rcsid)
    OFString Identification;

    /// Switch on/off error and warning messages
    OFBool QuietMode;

    /// Pointer to associated CommandLine class
    OFCommandLine *CmdLine;


 // --- declarations to avoid compiler warnings

    OFConsoleApplication(const OFConsoleApplication &);
    OFConsoleApplication &operator=(const OFConsoleApplication &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofconapp.h,v $
 * Revision 1.25  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.24  2008-09-25 11:14:54  joergr
 * Added method for printing the resource identifier of an application.
 *
 * Revision 1.23  2008-09-25 10:10:16  joergr
 * Print expanded command line arguments to stderr and not to stdout.
 *
 * Revision 1.22  2008-09-24 13:24:00  joergr
 * Added support for printing the expanded command line arguments to standard
 * output stream.
 *
 * Revision 1.21  2006/07/27 13:17:23  joergr
 * Prepended prefix "PF_" to parseLine() flags.
 *
 * Revision 1.20  2005/12/08 16:05:49  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.19  2004/01/16 10:30:12  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.18  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.17  2003/07/04 13:29:51  meichel
 * Replaced forward declarations for OFString with explicit includes,
 *   needed when compiling with HAVE_STD_STRING
 *
 * Revision 1.16  2003/06/12 13:17:51  joergr
 * Enhanced method printWarning(). Added method quietMode().
 *
 * Revision 1.15  2002/11/26 12:55:02  joergr
 * Changed syntax usage output for command line applications from stderr to
 * stdout.
 *
 * Revision 1.14  2002/09/23 14:56:55  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.13  2002/04/16 13:36:02  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.12  2001/06/01 15:51:33  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/10/10 12:01:20  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.10  2000/04/14 15:17:12  meichel
 * Adapted all ofstd library classes to consistently use ofConsole for output.
 *
 * Revision 1.9  2000/03/08 16:36:01  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/07 15:38:50  joergr
 * Changed behaviour of class OFConsoleApplication to support automatic
 * evaluation of "--help" option for command line application with no
 * mandatory parameter.
 *
 * Revision 1.7  1999/09/13 16:37:15  joergr
 * Added methods for output of warning and other messages.
 * Added method to switch on/off all output messages (quiet mode).
 *
 * Revision 1.6  1999/08/17 10:23:10  joergr
 * Corrected Doc++ comment.
 *
 * Revision 1.5  1999/04/27 16:24:53  joergr
 * Introduced list of valid parameters used for syntax output and error
 * checking.
 * Added method to check conflicts between two options (incl. error output).
 *
 * Revision 1.4  1999/04/26 16:34:34  joergr
 * Added support to check dependences between different options and report
 * error messages if necessary.
 *
 * Revision 1.3  1999/04/21 12:41:03  meichel
 * Added method OFConsoleApplication::checkParam()
 *
 * Revision 1.2  1999/03/22 09:00:49  joergr
 * Added/Changed comments.
 *
 * Revision 1.1  1999/02/08 12:00:41  joergr
 * Added class to handle console applications (with or w/o command line
 * arguments).
 *
 *
 */
