/*
 *
 *  Copyright (C) 1999-2012, OFFIS e.V.
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
class DCMTK_OFSTD_EXPORT OFConsoleApplication
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
     ** @param  cmd       reference to the OFCommandLine object.  Should be valid at least as
     *                    long as this object exists.
     *  @param  argCount  number of arguments (argc)
     *  @param  argValue  pointer to argument array (argv[])
     *  @param  flags     flags to be used for parsing (e.g. OFCommandLine::PF_NoCommandFiles)
     *  @param  startPos  first argument to be parsed (default: 1, i.e. omit program name)
     *
     ** @return status of parsing process, true if successful, false otherwise
     */
    OFBool parseCommandLine(OFCommandLine &cmd,
                            int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

#ifdef DCMTK_USE_WCHAR_T

    /** parse command line.
     *  This is a Windows-specific version supporting the wide character encoding (UTF-16).
     *  If the command line has no argument (in case at least one argument is required) and
     *  if the command line has only one argument, namely "--help" or the specified shortcut,
     *  (in all cases) the usage is printed (see printUsage).
     *
     ** @param  cmd       reference to the OFCommandLine object.  Should be valid at least as
     *                    long as this object exists.
     *  @param  argCount  number of arguments (argc)
     *  @param  argValue  pointer to argument array (argv[])
     *  @param  flags     flags to be used for parsing (e.g. OFCommandLine::PF_NoCommandFiles)
     *  @param  startPos  first argument to be parsed (default: 1, i.e. omit program name)
     *
     ** @return status of parsing process, true if successful, false otherwise
     */
    OFBool parseCommandLine(OFCommandLine &cmd,
                            int argCount,
                            wchar_t *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

#endif  // HAVE_WINDOWS_H ...

    /** print header of console application (consisting of identifier, name and description)
     *
     ** @param  hostInfo  print host information as reported by 'config.guess' if OFTrue.
     *                    If compiled with 'libiconv' support, the current locale's character
     *                    encoding is also shown.  On Windows systems, either the current OEM
     *                    and ANSI code page identifiers are printed or "Unicode (UTF-16)" if
     *                    support is enabled.  Finally, if the DEBUG macro is defined, a note
     *                    on the presence of debug code is given.
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


 protected:

    /** check parse status of previously parsed command line
     */
    OFBool checkParseStatus(const OFCommandLine::E_ParseStatus status);


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
