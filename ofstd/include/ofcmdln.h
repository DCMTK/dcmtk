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
 *  Update Date:      $Date: 1999-04-29 15:21:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofcmdln.h,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
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


typedef OFListIterator(OFString) OFListIterator_OFString;       // necessary for MSVC5


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
        OptionDescription(optDescr),
        Checked(OFFalse)
    {
    }

    ~OFCmdOption()
    {
#ifdef DEBUG
        if (!Checked && (LongOption.length() > 0) && (LongOption != "--help"))
            cerr << "WARNING: option " << LongOption << " has never been checked !" << endl;
#endif
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
    /// OFTrue if findOption has been applied to this option
    OFBool Checked;
};


/** Internal structure to store valid command line parameters.
 *  Parameters are all command line arguments which are no options (e.g. file names).
 */
struct OFCmdParam
{

    /** mode specifying parameter's cardinality
     */
    enum E_ParamMode
    {
        /// parameter is required (# = 1)
        PM_Mandatory,
        /// parameter is optional (# = 0..1)
        PM_Optional,
        /// parameter is required, more than one value is allowed (# = 1..n)
        PM_MultiMandatory,
        /// parameter is optional, more than one value is allowed (# = 0..n)
        PM_MultiOptional
    };

    /** constructor
     *
     ** @param  param  parameter name
     *  @param  descr  parameter description
     *  @param  mode   parameter's cardinality mode
     */
    OFCmdParam(const char *param,
               const char *descr,
               const E_ParamMode mode)
      : ParamName(param),
        ParamDescription(descr),
        ParamMode(mode)
    {
    }

    /// parameter name
    const OFString ParamName;
    /// parameter description
    const OFString ParamDescription;
    /// parameter's cardinality mode
    const E_ParamMode ParamMode;
};


/** Internal structure to handle position of command line parameters.
 *  Parameters are all command line arguments which are no options (e.g. file names).
 */
struct OFCmdParamPos
{

    /** constructor
     *
     ** @param  parIter   iterator pointing to a specific parameter
     *  @param  optIter   iterator pointing to first option iterator in front of the parameter
     *  @param  optCount  number of options in front of the parameter
     */
    OFCmdParamPos(const OFListIterator(OFString) &parIter,
                  const OFListIterator(OFListIterator_OFString) &optIter,
                  const int optCount)
      : ParamIter(parIter),
        OptionIter(optIter),
        OptionCount(optCount)
    {
    }

    /// iterator pointing to a specific parameter
    const OFListIterator(OFString) ParamIter;
    /// iterator pointing to first option iterator in front of the parameter
    const OFListIterator(OFListIterator_OFString) OptionIter;
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
        PS_MissingValue,
        /// missing parameter
        PS_MissingParameter,
        /// too many parameters
        PS_TooManyParameters
    };


    /** status of converting string option value to value field
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


    /** status of converting string parameter to value field
     */
    enum E_ParamValueStatus
    {
        /// normal, no errors
        PVS_Normal,
        /// argument contains invalid char(s)
        PVS_Invalid,
        /// specified parameter doesn't exist
        PVS_CantFind,
        /// empty string argument
        PVS_Empty,
        /// converted value falls below minimum
        PVS_Underflow,
        /// converted value exceeds maximum
        PVS_Overflow
    };
    
    /** mode for findOption method
     */
    enum E_FindOptionMode
    {
        /// normal find
        FOM_Normal,
        /// find first option
        FOM_First,
        /// find next option
        FOM_Next 
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
    
    /** sets default width of option columns
     *
     *  @param  longCols   minimum width of the long option column
     *  @param  shortCols  minimum width of the short option column
     */
    void setOptionColumns(const int longCols,
                          const int shortCols);

    /** sets default width of parameter column
     *
     *  @param  column  minimum width of the long option column
     */
    void setParamColumn(const int column);

    /** adds an item to the list of valid options
     *  (full version)
     *
     ** @param  longOpt     long option name
     *  @param  shortOpt    short option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
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
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const char *shortOpt,
                     const char *optDescr);
 
    /** adds an item to the list of valid options
     *  (without short name)
     *
     ** @param  longOpt     long option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const int valueCount,
                     const char *valueDescr,
                     const char *optDescr);
 
    /** adds an item to the list of valid options
     *  (without short name and additional values)
     *
     ** @param  longOpt   long option name
     *  @param  optDescr  description of command line option
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const char *optDescr);
                   
    /** adds a new group (top-level).
     *  all following options belong to this group
     *
     ** @param  name       name of the group
     *  @param  longCols   minimum width of the long option column
     *  @param  shortCols  minimum width of the short option column
     */
    void addGroup(const char *name,
                  const int longCols = 0,
                  const int shortCols = 0);

    /** adds a new subgroup (beyond group-level).
     *  all following options belong to this subgroup
     *
     ** @param  name       name of the subgroup
     *  @param  longCols   minimum width of the long option column
     *  @param  shortCols  minimum width of the short option column
     */
    void addSubGroup(const char *name,
                     const int longCols = 0,
                     const int shortCols = 0);

    /** adds an item to the list of valid parameters
     *  (full version)
     *
     ** @param  param  parameter name
     *  @param  descr  parameter description
     *  @param  mode   parameter's cardinality (see above)
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool addParam(const char *param,
                    const char *descr,
                    const OFCmdParam::E_ParamMode mode = OFCmdParam::PM_Mandatory);

    /** adds an item to the list of valid parameters
     *  (without description)
     *
     ** @param  param  parameter name
     *  @param  mode   parameter's cardinality (see above)
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool addParam(const char *param,
                    const OFCmdParam::E_ParamMode mode = OFCmdParam::PM_Mandatory);


 // --- get information

    /** gets number of command line arguments.
     *  (options and parameters)
     *
     ** @return number of command line arguments
     */
    int getArgCount() const
    {
        return ArgumentList.size();
    }
    
    /** gets current command line argument as a C string
     *  This is the argument which is currently parsed.
     *
     ** @param  arg  reference to C string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getCurrentArg(const char *&arg);

    /** gets current command line argument as a C++ string.
     *  This is the argument which is currently parsed.
     *
     ** @param  arg  reference to C++ string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getCurrentArg(OFCmdString &arg);

    /** gets last command line argument as a C string
     *  This is the last entry in the list of all arguments.
     *
     ** @param  arg  reference to C string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getLastArg(const char *&arg);

    /** gets last command line argument as a C++ string
     *  This is the last entry in the list of all arguments.
     *
     ** @param  arg  reference to C++ string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getLastArg(OFCmdString &arg);

    /** gets number of parameters in the parsed command line.
     *  A parameter is an argument which is no option (e.g. a filename).
     *
     ** @return number of parameter
     */
    int getParamCount() const
    {
        return ParamPosList.size();
    }


 // --- find/get parameter

    /** checks whether specified parameter exists in the command line.
     *
     ** @param  pos  position of parameter to be checked (1..n)
     *
     ** @return OFTrue if parameter exists, false otherwise
     */
    OFBool findParam(const int pos);


    /** gets value of specified parameter as signed integer.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdSignedInt &value);

    /** get value of specified parameter as signed integer.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdSignedInt &value,
                                const OFCmdSignedInt low,
                                const OFBool incl = OFTrue);

    /** gets value of specified parameter as signed integer.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdSignedInt &value,
                                const OFCmdSignedInt low,
                                const OFCmdSignedInt high);

    /** gets value of specified parameter as unsigned integer.
     *  NB: If command line argument specifies a negative value the result depends on the
     *  signed->unsigned conversion implemented in sscanf() !!
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdUnsignedInt &value);

    /** gets value of specified parameter as unsigned integer.
     *  NB: If command line argument specifies a negative value the result depends on the
     *  signed->unsigned conversion implemented in sscanf() !!
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdUnsignedInt &value,
                                const OFCmdUnsignedInt low,
                                const OFBool incl = OFTrue);

    /** gets value of specified parameter as unsigned integer.
     *  NB: If command line argument specifies a negative value the result depends on the
     *  signed->unsigned conversion implemented in sscanf() !!
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdUnsignedInt &value,
                                const OFCmdUnsignedInt low,
                                const OFCmdUnsignedInt high);

    /** gets value of specified parameter as floating point.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdFloat &value);

    /** gets value of specified parameter as floating point.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdFloat &value,
                                const OFCmdFloat low,
                                const OFBool incl = OFTrue);

    /** gets value of specified parameter as floating point.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdFloat &value,
                                const OFCmdFloat low,
                                const OFCmdFloat high);

    /** gets value of specified parameter as C string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  param  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                const char *&param);

    /** gets value of specified parameter as C++ string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  param  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdString &param);


 // --- find/get option

    /** checks whether specified option exists in the command line.
     *  The search process starts from the last command line argument to the first one.
     *
     ** @param  longOpt  name of option (in long format) to be checked
     *  @param  pos      position of parameter (default: all parameters)
     *  @param  mode     ...
     *
     ** @return OFTrue if option exists, OFFalse otherwise
     */
    OFBool findOption(const char *longOpt,
                      const int pos = 0,
                      const E_FindOptionMode mode = FOM_Normal);

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


 // --- parsing command line

    E_ParseStatus parseLine(int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);


 // --- get usage/status strings

    void getSyntaxString(OFString &string) const;

    void getOptionString(OFString &string) const;

    void getParamString(OFString &string) const;

    void getStatusString(const E_ParseStatus status,
                         OFString &string);

    void getStatusString(const E_ParamValueStatus status,
                         OFString &string);

    void getStatusString(const E_ValueStatus status,
                         OFString &string);


    // flags

    /// parsing flag to expand wildcard under Windows (very similar to Unix)
    static const int ExpandWildcards;


 protected:
 
    OFBool checkOption(const char *string,
                       const OFBool mode = OFTrue) const;

    OFBool findParam(int pos,
                     OFListIterator(OFCmdParamPos *) &pos_iter);

    const OFCmdOption *findCmdOption(const char *option) const;
    
    void storeParameter(const char *param);

    int packColumnValues(int longCols,
                         int shortCols) const;

    void unpackColumnValues(const int value,
                            unsigned int &longCols,
                            unsigned int &shortCols) const;

#ifdef HAVE_WINDOWS_H
    void expandWildcards(const char *param);
#endif

    E_ParseStatus checkParamCount();

    OFBool getMissingParam(OFString &arg);
    

 private:

    OFList<OFCmdOption *> ValidOptionList;
    OFList<OFCmdParam *> ValidParamList;

    OFList<OFString> ArgumentList;
    OFListIterator(OFString) ArgumentIterator;
    
    OFList<OFCmdParamPos *> ParamPosList;
    OFList<OFListIterator_OFString> OptionPosList;
    OFListIterator(OFListIterator_OFString) OptionPosIterator;
    OFListIterator(OFListIterator_OFString) OptionBlockIterator;

    OFBool OptionBlockMode;
    OFString OptionChars;

    int LongColumn;
    int ShortColumn;
    int ParamColumn;
    
    int MinParamCount;
    int MaxParamCount;

    OFCmdParam::E_ParamMode LastParamMode;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofcmdln.h,v $
 * Revision 1.15  1999-04-29 15:21:45  joergr
 * Removed debug code.
 *
 * Revision 1.13  1999/04/28 13:13:16  joergr
 * Removed some '#ifdef DEBUG' statements from header files to avoid
 * problems with inconsistent compilations.
 *
 * Revision 1.12  1999/04/27 17:46:05  joergr
 * Added some comments (DOC++ style).
 *
 * Revision 1.10  1999/04/26 16:32:47  joergr
 * Added support to define minimum width of short and long option columns.
 * Removed bug: empty parameters have always been interpreted as options.
 * Enhanced support of wildcard expansion under Windows (now very similar
 * to Unix shells).
 *
 * Revision 1.9  1999/03/24 16:59:38  joergr
 * Added optional parameters to define minimum width of columns for short and
 * long options in syntax output.
 * Changed optional integer parameter in method findOption to enum type.
 *
 * Revision 1.8  1999/02/08 11:51:38  joergr
 * Removed '#include <iostream.h>' from ofcmdln.h.
 *
 * Revision 1.7  1999/02/05 14:07:07  joergr
 * Added automatic wildcard expansion for Windows compilers.
 *
 * Revision 1.6  1998/12/02 18:44:25  joergr
 * Introduced test whether added options are correct (starting with defined
 * option character followed by a character which is no number). Changed
 * parse routine to distinguish between options (normally starting mit - or
 * +) and signed numbers which can be valid parameters.
 *
 * Revision 1.5  1998/12/02 17:38:53  joergr
 * Introduced new enum type used to indicate the status when converting
 * parameter values (similar to option values). Changed return value of
 * getParam() methods to this type. Added corresponding getStatusString()
 * method to convert status code to strings.
 *
 * Revision 1.4  1998/12/02 15:19:49  joergr
 * Added methods to convert parameters to signed/unsigned integers and
 * floats. Changed return value of existing getParam() methods.
 *
 * Revision 1.3  1998/11/30 12:30:18  joergr
 * Use lists of pointers (!) to internal data structures to avoid errors with
 * MSVC5 (operator '==' was not defined to compare structures).
 *
 * Revision 1.2  1998/11/30 12:27:02  joergr
 * Introduced additional type definition to avoid errors with MSVC5 when
 * using ListIterators of ListIterators (syntax problems?).
 *
 * Revision 1.1  1998/11/27 12:34:47  joergr
 * Added class to handle command line arguments.
 *
 *
 */
