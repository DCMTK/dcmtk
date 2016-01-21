/*
 *
 *  Copyright (C) 1998-2016, OFFIS e.V.
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
 */


#ifndef OFCMDLN_H
#define OFCMDLN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/offile.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


/*--------------------*
 *  macro definition  *
 *--------------------*/

// Only use wchar_t on windows (not mingw) and don't use it on MSC6
#if defined(HAVE_WINDOWS_H) && !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER > 1200)
# define DCMTK_USE_WCHAR_T
#endif
#if defined(WIDE_CHAR_MAIN_FUNCTION) && defined(DCMTK_USE_WCHAR_T)
// Windows-specific version supporting wide character encoding (UTF-16)
# define DCMTK_MAIN_FUNCTION int wmain(int argc, wchar_t *argv[])
#else
// default version supporting various character encodings (incl. UTF-8)
# define DCMTK_MAIN_FUNCTION int main(int argc, char *argv[])
#endif


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
struct DCMTK_OFSTD_EXPORT OFCmdParam
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
class DCMTK_OFSTD_EXPORT OFCommandLine
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

    /** mode for findOption() method
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
     *  @param  optDescr    description of command line option (use '\\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if successfully added
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
     ** @param  longOpt   long option name
     *  @param  shortOpt  short option name
     *  @param  optDescr  description of command line option (use '\\n' for line break)
     *  @param  flags     optional flags (see AF_xxx below)
     *
     ** @return OFTrue if successfully added
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
     *  @param  optDescr    description of command line option (use '\\n' for line break)
     *  @param  flags       optional flags (see AF_xxx below)
     *
     ** @return OFTrue if successfully added
     */
    OFBool addOption(const char *longOpt,
                     const int valueCount,
                     const char *valueDescr,
                     const char *optDescr,
                     const int flags = 0);

    /** adds an item to the list of valid options
     *  (without short name and additional values)
     *
     ** @param  longOpt   long option name
     *  @param  optDescr  description of command line option (use '\\n' for line break)
     *  @param  flags     optional flags (see AF_xxx below)
     *
     ** @return OFTrue if successfully added
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
     *  @param  descr  parameter description (use '\\n' for line break)
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
     ** @return OFTrue if an exclusive option is used, OFFalse otherwise
     */
    OFBool hasExclusiveOption() const
    {
        return ExclusiveOption;
    }

    /** checks whether the wide character version of parseLine() has been used.
     *  Support for wide character encoding is Windows-specific (i.e. UTF-16) because other
     *  operating systems use UTF-8 for Unicode support.
     *
     ** @return OFTrue if wide char version of parseLine() has been used, OFFalse otherwise
     */
    OFBool getWideCharMode() const
    {
        return WideCharMode;
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
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
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ParamValueStatus getParamAndCheckMinMax(const int pos,
                                              OFCmdFloat &value,
                                              const OFCmdFloat low,
                                              const OFCmdFloat high);

    /** gets value of specified parameter as C string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                const char *&value);

    /** gets value of specified parameter as C++ string.
     *
     ** @param  pos    position of parameter (1..n)
     *  @param  value  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFCmdString &value);

    /** gets value of specified parameter as an instance of OFFilename.
     *  Please note that on Windows systems the returned filename might also contain a wide
     *  character version (UTF-16) as an alternative representation.  See getWideCharMode().
     *
     ** @param  pos       position of parameter (1..n)
     *  @param  filename  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, PVS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ParamValueStatus getParam(const int pos,
                                OFFilename &filename);


 // --- find/get option (option is an argument which starts with an option character, see above)

    /** checks whether specified option exists in the command line.
     *  The search process starts from the last command line argument (which is a direct
     *  predecessor of the optional defined reference parameter 'pos') and goes to the
     *  first one.
     *
     ** @param  longOpt  name of option (in long format) to be checked
     *  @param  pos      position of reference parameter
     *                   (default: all parameters; if value is negative option must be a
     *                    direct predecessor of the specified reference parameter '-pos',
     *                    no further search processes will be performed; if search direction
     *                    is left to right this parameter is ignored, also option blocks are
     *                    not supported in this case)
     *  @param  mode     find option mode (used to support option blocks and reverse search
     *                   direction)
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
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(OFCmdSignedInt &value);

    /** returns next argument as a signed integer value and checks for given boundary
     *
     ** @param  value  reference to signed integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdSignedInt &value,
                                      const OFCmdSignedInt low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as a signed integer value and checks for given boundaries
     *
     ** @param  value  reference to signed integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdSignedInt &value,
                                         const OFCmdSignedInt low,
                                         const OFCmdSignedInt high);

    /** returns next argument as an unsigned integer value
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(OFCmdUnsignedInt &value);

    /** returns next argument as an unsigned integer value and checks for given boundary
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdUnsignedInt &value,
                                      const OFCmdUnsignedInt low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as an unsigned integer value and checks for given boundaries
     *
     ** @param  value  reference to unsigned integer variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdUnsignedInt &value,
                                         const OFCmdUnsignedInt low,
                                         const OFCmdUnsignedInt high);

    /** returns next argument as a floating point value
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(OFCmdFloat &value);

    /** returns next argument as a floating point value and checks for given boundary
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check)
     *  @param  incl   if OFTrue 'low' value is valid (included), otherwise invalid
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMin(OFCmdFloat &value,
                                      const OFCmdFloat low,
                                      const OFBool incl = OFTrue);

    /** returns next argument as a floating point value and checks for given boundaries
     *
     ** @param  value  reference to floating point variable where the value should be stored
     *  @param  low    minimum boundary for value (used for range check, boundary included)
     *  @param  high   maximum boundary for value (ditto)
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValueAndCheckMinMax(OFCmdFloat &value,
                                         const OFCmdFloat low,
                                         const OFCmdFloat high);

    /** returns next argument as a C string
     *
     ** @param  value  reference to C string variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(const char *&value);

    /** returns next argument as a C++ string
     *
     ** @param  value  reference to C++ string variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(OFCmdString &value);

    /** returns next argument as an instance of OFFilename.
     *  Please note that on Windows systems the returned filename might also contain a wide
     *  character version (UTF-16) as an alternative representation.  See getWideCharMode().
     *
     ** @param  filename  reference to variable where the value should be stored
     *
     ** @return status of get/conversion, VS_Normal if successful
     *          (use getStatusString() for error string)
     */
    E_ValueStatus getValue(OFFilename &filename);


 // --- parsing command line

    /** parses specified command line arguments (argc, argv).  Sets 'WideCharMode' to OFFalse.
     *  Additionally, create internal structures for evaluation and return status indicating
     *  any errors that occurred during the parse process.
     *
     ** @param  argCount  number of command line arguments stored in argValue
     *  @param  argValue  array where the command line arguments are stored
     *  @param  flags     optional flags affecting the parse process (see PF_xxx below)
     *  @param  startPos  index of first argument which should be parsed (starting from 0,
     *                    default: 1)
     *
     ** @return status of parse process, PS_Normal if successful
     *          (use getStatusString() for error string).  If an exclusive option is used,
     *           the status code PS_ExclusiveOption is returned.
     */
    E_ParseStatus parseLine(int argCount,
                            char *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

#ifdef DCMTK_USE_WCHAR_T

    /** parses specified command line arguments (argc, argv).  Sets 'WideCharMode' to OFTrue.
     *  This is a Windows-specific version supporting the wide character encoding (UTF-16).
     *  Internally, all character strings are stored in UTF-8, because this Unicode encoding is
     *  supported by DICOM.  However, there are getValue() and getParam() methods that allow for
     *  accessing filenames with wide character encoding (UTF-16).
     *  Additionally, create internal structures for evaluation and return status indicating any
     *  errors that occurred during the parse process.
     *
     ** @param  argCount  number of command line arguments stored in argValue
     *  @param  argValue  array where the command line arguments are stored
     *  @param  flags     optional flags affecting the parse process (see PF_xxx below)
     *  @param  startPos  index of first argument which should be parsed (starting from 0,
     *                    default: 1)
     *
     ** @return status of parse process, PS_Normal if successful
     *          (use getStatusString() for error string).  If an exclusive option is used,
     *           the status code PS_ExclusiveOption is returned.
     */
    E_ParseStatus parseLine(int argCount,
                            wchar_t *argValue[],
                            const int flags = 0,
                            const int startPos = 1);

#endif  // DCMTK_USE_WCHAR_T


 // --- get usage/status strings

    /** returns command line syntax as a C++ string (single text line).
     *  Text consists of "[options]" (if defined by addOption()) and all parameters defined
     *  by addParam().
     *
     ** @param  syntaxStr  reference to C++ string where the text should be stored
     */
    void getSyntaxString(OFString &syntaxStr) const;

    /** returns description of all options as a C++ string (multiple text lines).
     *  Text consists of group headings, short and long option names, option descriptions and
     *  additional option values.
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

    /** returns status of parseLine() as a C++ string
     *
     ** @param  status     value returned by method parseLine()
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ParseStatus status,
                         OFString &statusStr);

    /** returns status of getParam() as a C++ string
     *
     ** @param  status     value returned by method getParam()
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ParamValueStatus status,
                         OFString &statusStr);

    /** returns status of getValue() as a C++ string
     *
     ** @param  status     value returned by method getValue()
     *  @param  statusStr  reference to C++string where the text should be stored
     */
    void getStatusString(const E_ValueStatus status,
                         OFString &statusStr);


 // --- flags (used for method parseLine())

    /// parsing flag to expand wildcard under Windows.
    /// (not used anymore, only available for reasons of backward compatibility;
    ///  link 'setargv.obj' to your program if you need wildcard expansion.)
    static const int PF_ExpandWildcards;
    /// disable support for command files ("@filename") containing additional arguments
    static const int PF_NoCommandFiles;

 // --- flags (used for method addOption())

    /// exclusive option that overrides any other option (e.g.\ "--help")
    static const int AF_Exclusive;
    /// internal option that is not shown in the syntax usage output
    static const int AF_Internal;
    /// do not output a warning message if option has never been checked.
    /// (useful if option is only checked depending on another option)
    static const int AF_NoWarning;


 protected:

    /** checks whether given option is valid (starting with an option char and not
     *  followed by a number)
     */
    OFBool checkOption(const OFString &option,
                       const OFBool mode = OFTrue) const;

    /** finds specified parameter and sets given iterator to this position
     */
    OFBool findParam(const int pos,
                     OFListIterator(OFCmdParamPos *) &pos_iter);

    /** finds specified option and returns reference to its describing structure
     */
    const OFCmdOption *findCmdOption(const OFString &option) const;

    /** stores the specified parameter in the argument/parameter list
     */
    void storeParameter(const OFString &param,
                        const int directOption = 0);

    /** parse the given argument list and check it for known options/parameters
     */
    E_ParseStatus parseArgumentList(OFList<OFString> &argList,
                                    const int flags);

    /** check whether 'argValue' points to command file and parse content if so
     */
    E_ParseStatus parseCommandFile(const char *argValue,
                                   OFList<OFString> &argList);

#ifdef DCMTK_USE_WCHAR_T

    /** check whether 'argValue' points to command file and parse content if so.
     *  This is a Windows-specific version supporting wide char encoding (UTF-16).
     */
    E_ParseStatus parseCommandFile(const wchar_t *argValue,
                                   const OFString &strValue,
                                   OFList<OFString> &argList);

#endif  // DCMTK_USE_WCHAR_T

    /** packs the two 16 bit values into one 32 bit value
     */
    int packColumnValues(int longCols,
                         int shortCols) const;

    /** unpacks two 16 bit values from one 32 bit value
     */
    void unpackColumnValues(const int value,
                            unsigned int &longCols,
                            unsigned int &shortCols) const;

    /** checks whether number of parameters in parsed command line is within the range
     *  of min/max (see below)
     */
    E_ParseStatus checkParamCount();

    /** returns last command line argument as a C++ string
     */
    OFBool getLastArg(OFString &arg);

    /** returns name of parameter which is missed in the parsed command line
     *  (used for error output)
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
    /// current position in param position list (numeric value)
    int ParamPosNumber;
    /// current position in param position list (iterator)
    OFListIterator(OFCmdParamPos *) ParamPosIterator;
    /// list of option positions within argument list
    OFList<OFListIterator_OFString> OptionPosList;
    /// current position in option position list
    OFListIterator(OFListIterator_OFString) OptionPosIterator;
    /// current position in option position list (used for option blocks)
    OFListIterator(OFListIterator_OFString) OptionBlockIterator;

    /// OFTrue if option block is active, OFFalse otherwise
    OFBool OptionBlockMode;
    /// C++ string consisting of all valid characters introducing an option
    OFString OptionChars;

    /// OFTrue if an "exclusive" option is used in the command line, OFFalse otherwise
    OFBool ExclusiveOption;
    /// OFTrue if wide character version of parseLine() has been used, OFFalse otherwise
    OFBool WideCharMode;

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
