/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Purpose: Handle command line arguments (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:34:47 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofcmdln.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFCMDLN_H
#define __OFCMDLN_H

#include "osconfig.h"

#include "oftypes.h"
#include "oflist.h"
#include "ofstring.h"

#include <stdio.h>
#include <iostream.h>


/*--------------------*
 *  type declaration  *
 *--------------------*/

/** @name Type definitions for OFCommandLine
 */
//@{

/// signed integer value
typedef signed long OFCmdSignedInt;
    
/// unsigned integer value
typedef unsigned long OFCmdUnsignedInt;
    
/// floating point value
typedef double OFCmdFloat;
    
/// dynamic string value
typedef OFString OFCmdString;

//@}



/*----------------------*
 *  struct declaration  *
 *----------------------*/

/** Internal structure to store valid command line options.
 *  Options are command line arguments to use optional functions and to specify optional properties of a program.
 *  They are all starting with one or more special characters and can therefore be detected.
 */
struct OFCmdOption
{

    /** constructor
     *
     ** @param  longOpt     long option name
     *  @param  shortOpt    short option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option
     */
    OFCmdOption(const char *longOpt,
                const char *shortOpt,
                const int valueCount,
                const char *valueDescr,
                const char *optDescr)
      : LongOption(longOpt),
        ShortOption(shortOpt),
        ValueCount(valueCount),
        ValueDescription(valueDescr),
        OptionDescription(optDescr)
    {
    }

    /// long option name
    const OFString LongOption;
    /// short option name
    const OFString ShortOption;
    /// number of additional values
    const int ValueCount;
    /// description of optional values
    const OFString ValueDescription;
    /// description of command line option
    const OFString OptionDescription;
};


/** Internal structure to handle command line parameters.
 *  Parameters are all command line arguments which are no options (e.g. file names).
 */
struct OFCmdParam
{

    /** constructor
     *
     ** @param  parIter   iterator pointing to a specific parameter
     *  @param  optIter   iterator pointing to first option iterator in front of the parameter
     *  @param  optCount  number of options in front of the parameter
     */
    OFCmdParam(const OFListIterator(OFString) &parIter,
               const OFListIterator(OFListIterator(OFString)) &optIter,
               const int optCount)
      : ParamIter(parIter),
        OptionIter(optIter),
        OptionCount(optCount)
    {
    }

    /// iterator pointing to a specific parameter
    const OFListIterator(OFString) ParamIter;
    /// iterator pointing to first option iterator in front of the parameter
    const OFListIterator(OFListIterator(OFString)) OptionIter;
    /// number of options in front of the parameter
    const int OptionCount;
};


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class to handle command line arguments.
 *  This is the interface to this module. All methods which can be used from outside are defined here.
 */
class OFCommandLine
{

 public:
 
 // --- enumerations

    /** status of command line parsing
     */
    enum E_ParseStatus
    {
        /// normal, no errors
        PS_Normal,
        /// no arguments to be parsed
        PS_NoArguments,
        /// unknown option detected
        PS_UnknownOption,
        /// missing value(s) for an option
        PS_MissingValue
    };


    /** status of converting string argument to value field
     */
    enum E_ValueStatus
    {
        /// normal, no errors
        VS_Normal,
        /// argument contains invalid char(s)
        VS_Invalid,
        /// no more arguments to be converted
        VS_NoMore,
        /// empty string argument
        VS_Empty,
        /// converted value falls below minimum
        VS_Underflow,
        /// converted value exceeds maximum
        VS_Overflow
    };


 // --- constructor and destructor

    /** constructor
     */
    OFCommandLine();

    /** destructor
     */
    virtual ~OFCommandLine();
    

 // --- initialization

    /** sets characters used to detect options
     *
     ** @param  chars  string containing all valid option characters (default: - and +)
     */
    void setOptionChars(const char *chars);
    
    /** adds an item to the list of valid options
     *  (full version)
     *
     ** @param  longOpt     long option name
     *  @param  shortOpt    short option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option
     */
    void addOption(const char *longOpt,
                   const char *shortOpt,
                   const int valueCount,
                   const char *valueDescr,
                   const char *optDescr);
 
    /** adds an item to the list of valid options
     *  (without additional values)
     *
     ** @param  longOpt     long option name
     *  @param  shortOpt    short option name
     *  @param  optDescr    description of command line option
     */
    void addOption(const char *longOpt,
                   const char *shortOpt,
                   const char *optDescr);
 
    /** adds an item to the list of valid options
     *  (without short name)
     *
     ** @param  longOpt     long option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option
     */
    void addOption(const char *longOpt,
                   const int valueCount,
                   const char *valueDescr,
                   const char *optDescr);
 
    /** adds an item to the list of valid options
     *  (without short name and additional values)
     *
     ** @param  longOpt   long option name
     *  @param  optDescr  description of command line option
     */
    void addOption(const char *longOpt,
                   const char *optDescr);
                   
    /** adds ...
     *
     ** @param  name  name of the group
     */
    void addGroup(const char *name);

    /** adds ...
     *
     ** @param  name  name of the subgroup
     */
    void addSubGroup(const char *name);


 // --- get information

    int getArgCount() const
    {
        return ArgumentList.size();
    }
    
    OFBool getCurrentArg(const char *&arg);

    OFBool getCurrentArg(OFCmdString &arg);

    OFBool getLastArg(const char *&arg);

    OFBool getLastArg(OFCmdString &arg);

    int getParamCount() const
    {
        return ParamPosList.size();
    }
    

 // --- find/get parameter

    OFBool findParam(const int pos);

    OFBool getParam(const int pos,
                    const char *&param);

    OFBool getParam(const int pos,
                    OFCmdString &param);


 // --- find/get option

    OFBool findOption(const char *longOpt,
                      const int pos = 0,
                      const int next = 0);

    OFBool getCurrentOption(const char *&opt);

    OFBool getCurrentOption(OFCmdString &opt);

    void beginOptionBlock();
    
    void endOptionBlock();


 // --- get value

    E_ValueStatus getValue(OFCmdSignedInt &value);

    E_ValueStatus getValue(OFCmdSignedInt &value,
                           const OFCmdSignedInt low,
                           const OFBool incl = OFTrue);

    E_ValueStatus getValue(OFCmdSignedInt &value,
                           const OFCmdSignedInt low,
                           const OFCmdSignedInt high);

    E_ValueStatus getValue(OFCmdUnsignedInt &value);

    E_ValueStatus getValue(OFCmdUnsignedInt &value,
                           const OFCmdUnsignedInt low,
                           const OFBool incl = OFTrue);

    E_ValueStatus getValue(OFCmdUnsignedInt &value,
                           const OFCmdUnsignedInt low,
                           const OFCmdUnsignedInt high);

    E_ValueStatus getValue(OFCmdFloat &value);

    E_ValueStatus getValue(OFCmdFloat &value,
                           const OFCmdFloat low,
                           const OFBool incl = OFTrue);

    E_ValueStatus getValue(OFCmdFloat &value,
                           const OFCmdFloat low,
                           const OFCmdFloat high);

    E_ValueStatus getValue(const char *&value);

    E_ValueStatus getValue(OFCmdString &value);


 // --- parsing

    E_ParseStatus parseLine(int argCount,
                            char *argValue[],
                            const int startPos = 1);


 // --- get usage/status strings

    void getOptionString(OFString &string) const;

    void getStatusString(const E_ParseStatus status,
                         OFString &string);

    void getStatusString(const E_ValueStatus status,
                         OFString &string);


 protected:
 
    OFBool findParam(int pos,
                     OFListIterator(OFCmdParam) &pos_iter);

    const OFCmdOption *findCmdOption(const char *option) const;
    

 private:

    OFList<OFCmdOption> ValidOptionList;

    OFList<OFString> ArgumentList;
    OFListIterator(OFString) ArgumentIterator;
    
    OFList<OFCmdParam> ParamPosList;
    OFList<OFListIterator(OFString)> OptionPosList;
    OFListIterator(OFListIterator(OFString)) OptionPosIterator;
    OFListIterator(OFListIterator(OFString)) OptionBlockIterator;

    OFBool OptionBlockMode;
    OFString OptionChars;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: ofcmdln.h,v $
** Revision 1.1  1998-11-27 12:34:47  joergr
** Added class to handle command line arguments.
**
**
**
*/
