/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Purpose: Handle console applications (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-27 16:24:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofconapp.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFCONAPP_H
#define __OFCONAPP_H

#include "osconfig.h"

#include "oftypes.h"
#include "ofcmdln.h"

#include <iostream.h>


/*------------------------*
 *  forward declerations  *
 *------------------------*/

class OFString;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class to handle console application.
 *  Builds an envelope to the OFCommandLine class to provide a consistent
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
     *  @param  output  output stream (default: cerr, if parameter = NULL)
     */
    OFConsoleApplication(const char *app,
                         const char *desc = NULL,
                         const char *rcsid = NULL,
                         ostream *output = NULL);
    
    /** destructor
     */
    ~OFConsoleApplication();
    
    /** parse command line
     *
     ** @param  cmd       application name
     *  @param  argCount  number of arguments (argc)
     *  @param  argValue  pointer to argument arry (argv[])
     *  @param  flags     flags to be used for parsing (e.g. OFCommandLine::ExpandWildcards)
     *  @param  startPos  first argument to be parsed (default: 1, omit program name)
     *
     ** @result status of parsing process, true if successful, false otherwise
     */
    OFBool parseCommandLine(OFCommandLine &cmd,
                            int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

    /** print header of console application (consisting of identifier, name and description)
     */
    void printHeader();

    /** print usage (syntax of command line options)
     *
     ** @param  cmd   reference to command line class (default: object used for parsing)
     */
    void printUsage(const OFCommandLine *cmd = NULL);

    /** print error message (incl. header)
     *
     ** @param  str  error message to be printed
     */
    void printError(const char *str);

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

    /** check dependence between sub and base option
     *
     ** @param  subOpt     option to be checked
     *  @param  baseOpt    base option required for sub option
     *  @param  condition  if false error message is printed (i.e. base option is absent)
     */
    void checkDependence(const char *subOpt,
                         const char *baseOpt,
                         OFBool condition);

    /** check conflict between two options
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

    /// Pointer to output stream for error messages (default: err)
    ostream  *Output;
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
 * Revision 1.5  1999-04-27 16:24:53  joergr
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
