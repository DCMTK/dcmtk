/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: Handle command line arguments (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.43 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCMDLN_H
#define OFCMDLN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofconsol.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


/*--------------------*
 *  type declaration  *
 *--------------------*/

/// signed integer value
typedef signed long OFCmdSignedInt;
/// unsigned integer value
typedef unsigned long OFCmdUnsignedInt;
/// floating point value
typedef double OFCmdFloat;
/// dynamic string value
typedef OFString OFCmdString;

// necessary for MSVC5 :-/
typedef OFListIterator(OFString) OFListIterator_OFString;


/*------------------------*
 *  forward declarations  *
 *------------------------*/

struct OFCmdOption;
struct OFCmdParamPos;


/*----------------------*
 *  struct declaration  *
 *----------------------*/

/** Internal structure to store valid command line parameters.
 *  Parameters are all command line arguments which are no options (e.g. file names).
 */
struct OFCmdParam
{

    /** mode specifying parameter's cardinality
     */
    enum E_ParamMode
    {
        /// parameter is required (# = 1), "option"
        PM_Mandatory,
        /// parameter is optional (# = 0..1), "[option]"
        PM_Optional,
        /// parameter is required, more than one value is allowed (# = 1..n), "option..."
        PM_MultiMandatory,
        /// parameter is optional, more than one value is allowed (# = 0..n), "[option...]"
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

private:

    /// private undefined copy assignment operator
    OFCmdParam& operator=(const OFCmdParam& arg);
};


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** handles command line arguments.
 *  This class is the interface to this module.
 *  All methods which can be used from outside are defined here.
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
        /// exclusive option used
        PS_ExclusiveOption,
        /// unknown option detected
        PS_UnknownOption,
        /// missing value(s) for an option
        PS_MissingValue,
        /// missing parameter
        PS_MissingParameter,
        /// too many parameters
        PS_TooManyParameters,
        /// cannot open command file
        PS_CannotOpenCommandFile
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
        FOM_Next,
        /// find first option (from left to right)
        FOM_FirstFromLeft,
        /// find next option (from left to right)
        FOM_NextFromLeft
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
     ** @param  chars  string containing all valid option characters (default: "+-")
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
     *  @param  optDescr    description of command line option (use '\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const char *shortOpt,
                     const int valueCount,
                     const char *valueDescr,
                     const char *optDescr,
                     const int flags = 0);

    /** adds an item to the list of valid options
     *  (without additional values)
     *
     ** @param  longOpt     long option name
     *  @param  shortOpt    short option name
     *  @param  optDescr    description of command line option (use '\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const char *shortOpt,
                     const char *optDescr,
                     const int flags = 0);

    /** adds an item to the list of valid options
     *  (without short name)
     *
     ** @param  longOpt     long option name
     *  @param  valueCount  number of additional values
     *  @param  valueDescr  description of optional values
     *  @param  optDescr    description of command line option (use '\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const int valueCount,
                     const char *valueDescr,
                     const char *optDescr,
                     const int flags = 0);

    /** adds an item to the list of valid options
     *  (without short name and additional values)
     *
     ** @param  longOpt    long option name
     *  @param  optDescr   description of command line option (use '\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if succesfully added
     */
    OFBool addOption(const char *longOpt,
                     const char *optDescr,
                     const int flags = 0);

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
     *  @param  descr  parameter description (use '\n' for line break)
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

    /** get the name of the program (i.e. the value of argv[0])
     *
     ** @return name of the program
     */
    const OFString &getProgramName() const
    {
        return ProgramName;
    }

    /** gets number of command line arguments.
     *  (options and parameters)
     *
     ** @return number of command line arguments
     */
    int getArgCount() const
    {
        return OFstatic_cast(int, ArgumentList.size());
    }

    /** sets first command line argument as the current one.
     *  Useful for iterating over all arguments.
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool gotoFirstArg();

    /** sets next command line argument as the current one.
     *  Useful for iterating over all arguments.
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool gotoNextArg();

    /** gets current command line argument as a C string
     *  This is the argument which is currently parsed or has been selected as
     *  the current one by gotoXXXArg().
     ** @param  arg  reference to C string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getCurrentArg(const char *&arg);

    /** gets current command line argument as a C++ string.
     *  This is the argument which is currently parsed or has been selected as
     *  the current one by gotoXXXArg().
     *
     ** @param  arg  reference to C++ string where argument should be stored
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getCurrentArg(OFCmdString &arg);

    /** gets number of parameters in the parsed command line.
     *  A parameter is an argument which is no option (e.g. a filename).
     *
     ** @return number of parameters
     */
    int getParamCount() const
    {
        return OFstatic_cast(int, ParamPosList.size());
    }

    /** gets minimum number of parameters which should be accepted.
     *
     ** @return number of parameters
     */
    int getMinParamCount() const
    {
        return MinParamCount;
    }

    /** gets maximum number of parameters which should be accepted.
     *
     ** @return number of parameters
     */
    int getMaxParamCount() const
    {
        return MaxParamCount;
    }

    /** checks whether the parsed command line contains any "exclusive" option
     *  which does not require any mandatory parameter.  Examples for typical
     *  exclusive options are "--help" and "--version".
     *
     ** @return OFTrue if an exclusive option is used
     */
    OFBool hasExclusiveOption() const
    {
        return ExclusiveOption;
    }


 // --- find/get parameter (parameter is an argument which is no option)

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
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdSignedInt &value);

    /** get value of specified parameter as signed integer and checks for given boundary
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMin(const int pos,
                                           OFCmdSignedInt &value,
                                           const OFCmdSignedInt low,
                                           const OFBool incl = OFTrue);

    /** gets value of specified parameter as signed integer and checks for given boundaries
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMinMax(const int pos,
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
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdUnsignedInt &value);

    /** gets value of specified parameter as unsigned integer and checks for given boundary.
     *  NB: If command line argument specifies a negative value the result depends on the
     *  signed->unsigned conversion implemented in sscanf() !!
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMin(const int pos,
                                           OFCmdUnsignedInt &value,
                                           const OFCmdUnsignedInt low,
                                           const OFBool incl = OFTrue);

    /** gets value of specified parameter as unsigned integer and checks for given boundaries.
     *  NB: If command line argument specifies a negative value the result depends on the
     *  signed->unsigned conversion implemented in sscanf() !!
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMinMax(const int pos,
                                              OFCmdUnsignedInt &value,
                                              const OFCmdUnsignedInt low,
                                              const OFCmdUnsignedInt high);

    /** gets value of specified parameter as floating point.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdFloat &value);

    /** gets value of specified parameter as floating point and checks for given boundary
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMin(const int pos,
                                           OFCmdFloat &value,
                                           const OFCmdFloat low,
                                           const OFBool incl = OFTrue);

    /** gets value of specified parameter as floating point and checks for given boundaries
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParamAndCheckMinMax(const int pos,
                                              OFCmdFloat &value,
                                              const OFCmdFloat low,
                                              const OFCmdFloat high);

    /** gets value of specified parameter as C string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  param  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                const char *&param);

    /** gets value of specified parameter as C++ string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  param  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful (use getStatusString for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdString &param);


 // --- find/get option (option is an argument which starts with an option character, see above)

    /** checks whether specified option exists in the command line.
     *  The search process starts from the last command line argument (which is a direct
     *  predecessor of the optional defined reference parameter 'pos') and goes to the
     *  first one.
     *
     ** @param  longOpt    name of option (in long format) to be checked
     *  @param  pos        position of reference parameter
     *                     (default: all parameters; if value is negative option must be a direct
     *                      predecessor of the specified reference parameter '-pos', no further
     *                      search processes will be performed; if search direction is left to right
     *                      this parameter is ignored, also option blocks are not supported in this
     *                      case)
     *  @param  mode       find option mode (used to support option blocks and reverse search direction)
     *
     ** @return OFTrue if option exists, OFFalse otherwise
     */
    OFBool findOption(const char *longOpt,
                      const signed int pos = 0,
                      const E_FindOptionMode mode = FOM_Normal);

    /** sets first command line option as the current one.
     *  Useful for iterating over all options.
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool gotoFirstOption();

    /** sets next command line option as the current one.
     *  Useful for iterating over all options.
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool gotoNextOption();

    /** returns current option as a C string.
     *  This is the option which has currently been parsed (e.g. used for error
     *  output) or which has been selected as the current one by gotoXXXOption().
     *
     ** @param  opt  reference to C string where option should be stored
     *
     ** @return OFTrue if there is a current option, OFFalse otherwise
     */
    OFBool getCurrentOption(const char *&opt);

    /** returns current option as a C++ string.
     *  This is the option which has currently been parsed (e.g. used for error
     *  output) or which has been selected as the current one by gotoXXXOption().
     *
     ** @param  opt  reference to C++ string where option should be stored
     *
     ** @return OFTrue if there is a current option, OFFalse otherwise
     */
    OFBool getCurrentOption(OFCmdString &opt);

    /** starts an option block which can be used to support mutually exclusive options.
     */
    void beginOptionBlock();

    /** ends an option block which can be used to support mutually exclusive options.
     */
    void endOptionBlock();


 // --- get value (used for option values)

    /** returns next argument as a signed integer value
     *
     ** @param  value  reference to signed integer variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValue(OFCmdSignedInt &value);

    /** returns next argument as a signed integer value and checks for given boundary
     *
     ** @param  value  reference to signed integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdSignedInt &value,
                                      const OFCmdSignedInt low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as a signed integer value and checks for given boundaries
     *
     ** @param  value  reference to signed integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdSignedInt &value,
                                         const OFCmdSignedInt low,
                                         const OFCmdSignedInt high);

    /** returns next argument as an unsigned integer value
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValue(OFCmdUnsignedInt &value);

    /** returns next argument as an unsigned integer value and checks for given boundary
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdUnsignedInt &value,
                                      const OFCmdUnsignedInt low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as an unsigned integer value and checks for given boundaries
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdUnsignedInt &value,
                                         const OFCmdUnsignedInt low,
                                         const OFCmdUnsignedInt high);

    /** returns next argument as a floating point value
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValue(OFCmdFloat &value);

    /** returns next argument as a floating point value and checks for given boundary
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdFloat &value,
                                      const OFCmdFloat low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as a floating point value and checks for given boundaries
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (dito)
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdFloat &value,
                                         const OFCmdFloat low,
                                         const OFCmdFloat high);

    /** returns next argument as a C string
     *
     ** @param  value  reference to C string variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValue(const char *&value);

    /** returns next argument as a C++ string
     *
     ** @param  value  reference to C++ string variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful (use getStatusString for error string)
     */
    E_ValueStatus getValue(OFCmdString &value);


 // --- parsing command line

    /** parses specified command line arguments (argc, argv).
     *  Additionally create internal structures for evaluation and return status indicating any errors
     *  occuring during the parse process.
     *
     ** @param  argCount  number of command line arguments stored in argValue
     *  @param  argValue  array where the command line arguments are stored
     *  @param  flags     optional flags affecting the parse process (see PF_xxx below)
     *  @param  startPos  index of first argument which should be parsed (starting from 0, default: 1)
     *
     ** @return status of parse process, PS_Normal if successful (use getStatusString for error string).
     *          If an exclusive option is used the status code PS_ExclusiveOption is returned.
     */
    E_ParseStatus parseLine(int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);


 // --- get usage/status strings

    /** returns command line syntax as a C++ string (single text line).
     *  Text consists of "[options]" (if defined by addOption) and all parameters defined by addParam.
     *
     ** @param  syntaxStr  reference to C++ string where the text should be stored
     */
    void getSyntaxString(OFString &syntaxStr) const;

    /** returns description of all options as a C++ string (multiple text lines).
     *  Text consists of group headings, short and long option names, option descriptions and additional
     *  option values.
     *
     ** @param  optionStr  reference to C++ string where the text should be stored
     */
    void getOptionString(OFString &optionStr) const;

    /** returns description of all parameters as a C++ string (multiple text lines).
     *  Text consists of heading "parameters:" and parameter names and descriptions.
     *
     ** @param  paramStr  reference to C++ string where the text should be stored
     */
    void getParamString(OFString &paramStr) const;

    /** returns status of parseLine as a C++ string
     *
     ** @param  status     value returned by method parseLine
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ParseStatus status,
                         OFString &statusStr);

    /** returns status of getParam as a C++ string
     *
     ** @param  status     value returned by method getParam
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ParamValueStatus status,
                         OFString &statusStr);

    /** returns status of getValue as a C++ string
     *
     ** @param  status     value returned by method getValue
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ValueStatus status,
                         OFString &statusStr);


 // --- flags (used for method parseLine)

    /// parsing flag to expand wildcard under Windows (very similar to Unix)
    static const int PF_ExpandWildcards;
    /// disable support for command files ("@filename") containing additional arguments
    static const int PF_NoCommandFiles;

 // --- flags (used for method addOption)

    /// exclusive option that overrides any other option (e.g. "--help")
    static const int AF_Exclusive;
    /// internal option that is not shown in the syntax usage output
    static const int AF_Internal;
    /// do not output a warning message if option has never been checked.
    /// (useful if option is only checked depending on another option)
    static const int AF_NoWarning;

 protected:

    /** checks whether given option is valid (starting with an option char and not followed by a number)
     */
    OFBool checkOption(const OFString &option,
                       const OFBool mode = OFTrue) const;

    /** finds specified parameter and sets given iterator to this position
     */
    OFBool findParam(int pos,
                     OFListIterator(OFCmdParamPos *) &pos_iter);

    /** finds specified option and returns reference to its describing structure
     */
    const OFCmdOption *findCmdOption(const OFString &option) const;

    /** stores the specified parameter in the argument/parameter list
     */
    void storeParameter(const OFString &param,
                        const int directOption = 0);

    /** check whether 'argValue' points to command file and parse content if so
     */
    E_ParseStatus parseCommandFile(const char *argValue,
                                   OFList<OFString> &argList);

    /** packs the two 16 bit values into one 32 bit value
     */
    int packColumnValues(int longCols,
                         int shortCols) const;

    /** unpacks two 16 bit values from one 32 bit value
     */
    void unpackColumnValues(const int value,
                            unsigned int &longCols,
                            unsigned int &shortCols) const;

#ifdef HAVE_WINDOWS_H
    /** expands wildcards in specified parameter.
     *  Very similar to Unix environments, stores each resulting parameter in the argument/parameter list
     */
    void expandWildcards(const OFString &param,
                         int directOption = 0);
#endif

    /** checks whether number of parameters in parsed command line is within the range of min/max (see below)
     */
    E_ParseStatus checkParamCount();

    /** returns last command line argument as a C++ string
     */
    OFBool getLastArg(OFString &arg);

    /** returns name of parameter which is missed in the parsed command line (used for error output)
     */
    OFBool getMissingParam(OFString &param);


 private:

    /// program name, i.e. the value of argv[0]
    OFString ProgramName;

    /// list of valid/defined command line options
    OFList<OFCmdOption *> ValidOptionList;
    /// list of valid/defined command line parameters
    OFList<OFCmdParam *> ValidParamList;

    /// list of command line arguments (after parsing)
    OFList<OFString> ArgumentList;
    /// current position in argument list
    OFListIterator(OFString) ArgumentIterator;

    /// list of parameter positions within argument list
    OFList<OFCmdParamPos *> ParamPosList;
    /// list of option positions within argument list
    OFList<OFListIterator_OFString> OptionPosList;
    /// current position in option position list
    OFListIterator(OFListIterator_OFString) OptionPosIterator;
    /// current position in option position list (used for option blocks)
    OFListIterator(OFListIterator_OFString) OptionBlockIterator;

    /// OFTrue if option block is active, OFFalse otherwise
    OFBool OptionBlockMode;
    /// C++ string conisting of all valid characters introducing an option
    OFString OptionChars;

    /// OFTrue if an "exclusive" option is used in the command line, OFFalse otherwise
    OFBool ExclusiveOption;

    /// width of column for long option names
    int LongColumn;
    /// width of column for short option names
    int ShortColumn;
    /// width of column for parameter names
    int ParamColumn;

    /// minimum number of parameters which should be accepted
    int MinParamCount;
    /// maximum number of parameter which should be accepted
    int MaxParamCount;

    /// mode of last added parameter (used for debug checking)
    OFCmdParam::E_ParamMode LastParamMode;


 private:

    /// private undefined assignment operator
    OFCommandLine &operator=(const OFCommandLine &obj);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofcmdln.h,v $
 * Revision 1.43  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.42  2010-04-28 13:23:18  joergr
 * Added type cast to integer variable in order to avoid compiler warnings
 * reported by VisualStudio 2005.
 *
 * Revision 1.41  2009-12-23 12:15:51  joergr
 * Added support for getting the name of the program, i.e. the value of argv[0].
 *
 * Revision 1.40  2009-06-04 09:53:00  joergr
 * Added new flag that can be used to avoid wrong warning messages (in debug
 * mode) that an option has possibly never been checked.
 *
 * Revision 1.39  2008-04-16 12:39:35  joergr
 * Added support for reverse search direction (left to right) to findOption().
 *
 * Revision 1.38  2006/07/27 13:16:11  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags".
 * Added addOption() flag for internal options that are not shown in the syntax
 * usage output. Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.37  2005/12/08 16:05:48  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.36  2003/12/05 13:59:33  joergr
 * Fixed problem with retrieving option values using the new iteration feature.
 *
 * Revision 1.35  2003/12/05 10:36:03  joergr
 * Added support for iterating over command line arguments and options.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.34  2003/06/12 13:19:58  joergr
 * Added support for so-called command files ("@filename") which can be used to
 * summarize command line options and parameter
 *
 * Revision 1.33  2003/05/20 08:42:39  joergr
 * Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 * Revision 1.32  2002/12/09 13:04:41  joergr
 * Replaced tab characters by spaces.
 *
 * Revision 1.31  2002/12/05 13:48:21  joergr
 * Make sure that no warning on "unchecked command line options" is reported in
 * debug mode when an exclusive option is used.
 *
 * Revision 1.30  2002/11/27 11:23:04  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.29  2002/09/19 08:30:20  joergr
 * Added general support for "exclusive" command line options besides "--help",
 * e.g. "--version".
 *
 * Revision 1.28  2001/11/09 15:46:42  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 *
 * Revision 1.27  2001/08/23 16:05:52  meichel
 * Added private undefined copy assignment operators to avoid gcc warnings
 *
 * Revision 1.26  2001/06/01 15:51:32  meichel
 * Updated copyright header
 *
 * Revision 1.25  2000/10/10 12:01:20  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.24  2000/04/14 15:17:11  meichel
 * Adapted all ofstd library classes to consistently use ofConsole for output.
 *
 * Revision 1.23  2000/03/08 16:36:01  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/07 15:38:49  joergr
 * Changed behaviour of class OFConsoleApplication to support automatic
 * evaluation of "--help" option for command line application with no
 * mandatory parameter.
 *
 * Revision 1.21  2000/03/03 14:02:46  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.20  2000/03/02 12:39:11  joergr
 * Fixed inconsistency: console applications with no or only optional
 * parameters could not be started without any command line argument
 * because this was always regarded identical with "--help" (print usage).
 *
 * Revision 1.19  1999/10/04 10:02:31  joergr
 * Fixed bug in wildcard expansion (concerning "direct option" feature).
 *
 * Revision 1.18  1999/09/13 16:36:54  joergr
 * Corrected bug in OFCommandLine::findOption() regarding the optional
 * parameter 'pos' specifying a reference command line parameter.
 *
 * Revision 1.17  1999/09/06 16:48:25  joergr
 * Added support to method 'findOption()' to detect options which are
 * 'direct' predecessors of an optionally specified reference parameter.
 *
 * Revision 1.16  1999/05/04 08:38:26  joergr
 * Added DOC++ comments to header file.
 *
 * Revision 1.15  1999/04/29 15:21:45  joergr
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
