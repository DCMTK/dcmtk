/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *  Purpose: Handle command line arguments (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofchrenc.h"

#if defined(HAVE_WINDOWS_H) && !defined(__MINGW32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define COMMAND_FILE_PREFIX '@'

#ifdef DCMTK_USE_WCHAR_T
#define WIDE_COMMAND_FILE_PREFIX L'@'
#endif


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int OFCommandLine::PF_ExpandWildcards = 0x0001;  // not used anymore
const int OFCommandLine::PF_NoCommandFiles  = 0x0002;

const int OFCommandLine::AF_Exclusive       = 0x0001;
const int OFCommandLine::AF_Internal        = 0x0002;
const int OFCommandLine::AF_NoWarning       = 0x0004;

/*-----------------------*
 *  struct declarations  *
 *-----------------------*/

/*  Internal structure to store valid command line options.
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
     *  @param  flags       flags (see AF_xxx)
     */
    OFCmdOption(const char *longOpt,
                const char *shortOpt,
                const int valueCount,
                const char *valueDescr,
                const char *optDescr,
                const int flags)
      : LongOption(longOpt),
        ShortOption(shortOpt),
        ValueCount(valueCount),
        ValueDescription(valueDescr),
        OptionDescription(optDescr),
        Flags(flags),
        Checked(OFFalse)
    {
    }

    /** destructor
     */
    ~OFCmdOption()
    {
#ifdef DEBUG
        if (!Checked && !(Flags & (OFCommandLine::AF_Exclusive | OFCommandLine::AF_NoWarning)) && !LongOption.empty())
        {
            ofConsole.lockCerr() << "WARNING: option " << LongOption << " has possibly never been checked !" << OFendl;
            ofConsole.unlockCerr();
        }
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
    /// flags (see AF_xxx)
    const int Flags;
    /// OFTrue if findOption has been applied to this option
    OFBool Checked;

private:

    /// private undefined copy assignment operator
    OFCmdOption& operator=(const OFCmdOption& arg);
};


/*  Internal structure to handle position of command line parameters.
 *  Parameters are all command line arguments which are no options (e.g. file names).
 */
struct OFCmdParamPos
{

    /** constructor
     *
     ** @param  parIter    iterator pointing to a specific parameter
     *  @param  optIter    iterator pointing to first option iterator in front of the parameter
     *  @param  optCount   number of options in front of the parameter
     *  @param  directOpt  number of options which are direct predecessor in the argument list
     */
    OFCmdParamPos(const OFListIterator(OFString) &parIter,
                  const OFListIterator(OFListIterator_OFString) &optIter,
                  const int optCount,
                  const int directOpt)
      : ParamIter(parIter),
        OptionIter(optIter),
        OptionCount(optCount),
        DirectOption(directOpt)
    {
    }

    /// iterator pointing to a specific parameter
    const OFListIterator(OFString) ParamIter;
    /// iterator pointing to first option iterator in front of the parameter
    const OFListIterator(OFListIterator_OFString) OptionIter;
    /// number of options in front of the parameter
    const int OptionCount;
    /// number of options which are direct predecessor in the argument list
    const int DirectOption;

private:

    /// private undefined copy assignment operator
    OFCmdParamPos& operator=(const OFCmdParamPos& arg);
};


/*------------------*
 *  implementation  *
 *------------------*/

OFCommandLine::OFCommandLine()
  : ProgramName(),
    ValidOptionList(),
    ValidParamList(),
    ArgumentList(),
    ArgumentIterator(),
    ParamPosList(),
    ParamPosNumber(0),
    ParamPosIterator(),
    OptionPosList(),
    OptionPosIterator(),
    OptionBlockIterator(),
    OptionBlockMode(OFFalse),
    OptionChars("+-"),
    ExclusiveOption(OFFalse),
    WideCharMode(OFFalse),
    LongColumn(0),
    ShortColumn(0),
    ParamColumn(0),
    MinParamCount(0),
    MaxParamCount(0),
    LastParamMode(OFCmdParam::PM_Mandatory)
{
}


OFCommandLine::~OFCommandLine()
{
    OFListIterator(OFCmdOption *) first_o = ValidOptionList.begin();
    const OFListIterator(OFCmdOption *) last_o = ValidOptionList.end();
    while (first_o != last_o)
    {
        /* avoid wrong warning message */
        if (ExclusiveOption)
            (*first_o)->Checked = OFTrue;
        /* delete object and remove from list */
        delete (*first_o);
        first_o = ValidOptionList.erase(first_o);
    }
    OFListIterator(OFCmdParam *) first_p = ValidParamList.begin();
    const OFListIterator(OFCmdParam *) last_p = ValidParamList.end();
    while (first_p != last_p)
    {
        delete (*first_p);
        first_p = ValidParamList.erase(first_p);
    }
    OFListIterator(OFCmdParamPos *) first_pp = ParamPosList.begin();
    const OFListIterator(OFCmdParamPos *) last_pp = ParamPosList.end();
    while (first_pp != last_pp)
    {
        delete (*first_pp);
        first_pp = ParamPosList.erase(first_pp);
    }
}


void OFCommandLine::setOptionChars(const char *chars)
{
    OptionChars = chars;
}


void OFCommandLine::setOptionColumns(const int longCols,
                                     const int shortCols)
{
    LongColumn = longCols;
    ShortColumn = shortCols;
}


void OFCommandLine::setParamColumn(const int column)
{
    ParamColumn = column;
}


OFBool OFCommandLine::checkOption(const OFString &option,
                                  const OFBool mode) const
{
    OFBool result = mode;
    const size_t optionLen = option.length();
    if (optionLen > 0)                                                   // empty strings are allowed to support (sub)groups
    {
        result = OFFalse;
        if (optionLen >= 2)
        {
            if (OptionChars.find(option.at(0)) != OFString_npos)         // options have to start with one of the defined chars
            {
                if (((option.at(0) != '-') && (option.at(0) != '+')) ||  // but when starting with sign character ...
                    (option.at(1) < '0') || (option.at(1) > '9'))        // ... don't allow a number as the following character
                {
                    result = OFTrue;
                }
            }
        }
    }
    return result;
}

void OFCommandLine::addGeneralOptions(const int longCols,
                                      const int shortCols)
{
    addGroup("general options:", longCols, shortCols + 2);
        addOption("--help",    "-h", "print this help text and exit",      OFCommandLine::AF_Exclusive);
        addOption("--version",       "print version information and exit", OFCommandLine::AF_Exclusive);
}

OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *shortOpt,
                                const int valueCount,
                                const char *valueDescr,
                                const char *optDescr,
                                const int flags)
{
    if (checkOption(longOpt) && checkOption(shortOpt))
    {
#ifdef DEBUG
        if (strlen(longOpt) > 0)
        {
            OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
            const OFListIterator(OFCmdOption *) last = ValidOptionList.end();
            while (iter != last)
            {
                if ((*iter)->LongOption == longOpt)
                {
                    ofConsole.lockCerr() << "WARNING: long option " << longOpt << " already defined ... not added !" << OFendl;
                    ofConsole.unlockCerr();
                    return OFFalse;
                }
                if ((strlen(shortOpt) > 0) && ((*iter)->ShortOption == shortOpt))
                {
                    ofConsole.lockCerr() << "WARNING: short option " << shortOpt << " already defined for " << (*iter)->LongOption
                                         << " ..." << OFendl << "         option " << longOpt << " not added !" << OFendl;
                    ofConsole.unlockCerr();
                    return OFFalse;
                }
                ++iter;
            }
        }
#endif
        OFCmdOption *opt = new OFCmdOption(longOpt, shortOpt, valueCount, valueDescr, optDescr, flags);
        if (opt != NULL)
        {
            ValidOptionList.push_back(opt);
            return OFTrue;
        }
    }
#ifdef DEBUG
    ofConsole.lockCerr() << "WARNING: invalid option " << shortOpt << "/" <<longOpt << " ... not added !" << OFendl;
    ofConsole.unlockCerr();
#endif
    return OFFalse;
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *shortOpt,
                                const char *optDescr,
                                const int flags)
{
    return addOption(longOpt, shortOpt, 0, "", optDescr, flags);
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const int valueCount,
                                const char *valueDescr,
                                const char *optDescr,
                                const int flags)
{
    return addOption(longOpt, "", valueCount, valueDescr, optDescr, flags);
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *optDescr,
                                const int flags)
{
    return addOption(longOpt, "", 0, "", optDescr, flags);
}


void OFCommandLine::addGroup(const char *name,
                             const int longCols,
                             const int shortCols)
{
    addOption("", "", packColumnValues(longCols, shortCols), "", name);
}


void OFCommandLine::addSubGroup(const char *name,
                                const int longCols,
                                const int shortCols)
{
    addOption("", "", packColumnValues(longCols, shortCols), name, "");
}


OFBool OFCommandLine::addParam(const char *param,
                               const char *descr,
                               const OFCmdParam::E_ParamMode mode)
{
    if (param != NULL)
    {
#ifdef DEBUG
        switch (LastParamMode)
        {
            case OFCmdParam::PM_Optional:
                if ((mode != OFCmdParam::PM_Optional) && (mode != OFCmdParam::PM_MultiOptional))
                {
                    ofConsole.lockCerr() << "WARNING: " << ValidParamList.size() << ". parameter is optional => hides "
                                         << param << " !" << OFendl;
                    ofConsole.unlockCerr();
                }
                break;
/*
            case OFCmdParam::PM_MultiMandatory:
                {
                    ofConsole.lockCerr() << "WARNING: " << ValidParamList.size() << ". parameter is multi-mandatory => hides "
                                         << param << " !" << OFendl;
                    ofConsole.unlockCerr();
                }
                break;
*/
            case OFCmdParam::PM_MultiOptional:
                {
                    ofConsole.lockCerr() << "WARNING: " << ValidParamList.size() << ". parameter is multi-optional => hides "
                                         << param << " !" << OFendl;
                    ofConsole.unlockCerr();
                }
                break;
            default:
                break;
        }
        LastParamMode = mode;
#endif
        OFCmdParam *par = new OFCmdParam(param, descr, mode);
        if (par != NULL)
        {
            ValidParamList.push_back(par);
            return OFTrue;
        }
    }
    return OFFalse;
}


OFBool OFCommandLine::addParam(const char *param,
                               const OFCmdParam::E_ParamMode mode)
{
    return addParam(param, "", mode);
}


OFBool OFCommandLine::gotoFirstArg()
{
    ArgumentIterator = ArgumentList.begin();
    return ArgumentIterator != ArgumentList.end();
}


OFBool OFCommandLine::gotoNextArg()
{
    if (ArgumentIterator != ArgumentList.end())
        return ++ArgumentIterator != ArgumentList.end();
    return OFFalse;
}


OFBool OFCommandLine::getCurrentArg(const char *&arg)
{
    if (ArgumentIterator != ArgumentList.end())
        return strlen(arg = (*ArgumentIterator).c_str()) > 0;
    return OFFalse;

}


OFBool OFCommandLine::getCurrentArg(OFCmdString &arg)
{
    if (ArgumentIterator != ArgumentList.end())
        return !(arg = *ArgumentIterator).empty();
    return OFFalse;

}


OFBool OFCommandLine::getLastArg(OFString &arg)
{
    if (!ArgumentList.empty())
        return !(arg = ArgumentList.back()).empty();
    return OFFalse;
}


OFBool OFCommandLine::findParam(const int pos)
{
    OFListIterator(OFCmdParamPos *) iter;
    return findParam(pos, iter);
}


OFBool OFCommandLine::findParam(const int pos,
                                OFListIterator(OFCmdParamPos *) &pos_iter)
{
    if ((pos > 0) && (pos <= getParamCount()))
    {
        int counter;
        if ((ParamPosNumber > 0) && (pos >= ParamPosNumber))     // can we start from previous position?
        {
            counter = pos - ParamPosNumber + 1;
            pos_iter = ParamPosIterator;
        } else {                                                 // if not, start from beginning
            counter = pos;
            pos_iter = ParamPosList.begin();
        }
        const OFListIterator(OFCmdParamPos *) pos_last = ParamPosList.end();
        while (pos_iter != pos_last)
        {
            ArgumentIterator = (*pos_iter)->ParamIter;
            if (--counter == 0)
            {
                ParamPosNumber = pos;                            // if found, store current position
                ParamPosIterator = pos_iter;
                return OFTrue;
            }
            ++pos_iter;
        }
    }
    return OFFalse;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFCmdSignedInt &value)
{
    if (findParam(pos))
    {
        if (sscanf((*ArgumentIterator).c_str(), "%li", &value) == 1)
            return PVS_Normal;
        return PVS_Invalid;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMin(const int pos,
                                                                     OFCmdSignedInt &value,
                                                                     const OFCmdSignedInt low,
                                                                     const OFExplicitBool incl)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return PVS_Underflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMinMax(const int pos,
                                                                        OFCmdSignedInt &value,
                                                                        const OFCmdSignedInt low,
                                                                        const OFCmdSignedInt high)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if (value < low)
            return PVS_Underflow;
        else if (value > high)
            return PVS_Overflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFCmdUnsignedInt &value)
{
    if (findParam(pos))
    {
        if (sscanf((*ArgumentIterator).c_str(), "%lu", &value) == 1)
            return PVS_Normal;
        return PVS_Invalid;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMin(const int pos,
                                                                     OFCmdUnsignedInt &value,
                                                                     const OFCmdUnsignedInt low,
                                                                     const OFExplicitBool incl)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return PVS_Underflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMinMax(const int pos,
                                                                        OFCmdUnsignedInt &value,
                                                                        const OFCmdUnsignedInt low,
                                                                        const OFCmdUnsignedInt high)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if (value < low)
            return PVS_Underflow;
        else if (value > high)
            return PVS_Overflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFCmdFloat &value)
{
    if (findParam(pos))
    {
        OFBool success = OFFalse;
        value = OFStandard::atof((*ArgumentIterator).c_str(), &success);
        if (success) return PVS_Normal;
        return PVS_Invalid;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMin(const int pos,
                                                                     OFCmdFloat &value,
                                                                     const OFCmdFloat low,
                                                                     const OFExplicitBool incl)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return PVS_Underflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParamAndCheckMinMax(const int pos,
                                                                        OFCmdFloat &value,
                                                                        const OFCmdFloat low,
                                                                        const OFCmdFloat high)
{
    E_ParamValueStatus status = getParam(pos, value);
    if (status == PVS_Normal)
    {
        if (value < low)
            return PVS_Underflow;
        else if (value > high)
            return PVS_Overflow;
    }
    return status;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          const char *&value)
{
    if (findParam(pos))
    {
        value = (*ArgumentIterator).c_str();
        if (strlen(value) > 0)
            return PVS_Normal;
        return PVS_Empty;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFCmdString &value)
{
    if (findParam(pos))
    {
        value = *ArgumentIterator;
        if (!value.empty())
            return PVS_Normal;
        return PVS_Empty;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFFilename &filename)
{
    OFString value;
    E_ParamValueStatus status = getParam(pos, value);
    if (status != PVS_CantFind)
    {
        if (status != PVS_Empty)
            filename.set(value, WideCharMode /* convert */);
        else
            filename.clear();
    }
    return status;
}


OFBool OFCommandLine::findOption(const char *longOpt,
                                 const signed int pos,
                                 const E_FindOptionMode mode)
{
#ifdef DEBUG
    OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
    const OFListIterator(OFCmdOption *) last = ValidOptionList.end();
    while (iter != last)
    {
        if ((*iter)->LongOption == longOpt)
        {
            (*iter)->Checked = OFTrue;
            break;
        }
        ++iter;
    }
    if (iter == last)
    {
        ofConsole.lockCerr() << "WARNING: unknown option " << longOpt << " in OFCommandLine::findOption() !" << OFendl;
        ofConsole.unlockCerr();
        return OFFalse;
    }
#endif
    // reverse direction (left to right)
    if ((mode == FOM_FirstFromLeft) || (mode == FOM_NextFromLeft))
    {
#ifdef DEBUG
        if (pos != 0)
        {
            ofConsole.lockCerr() << "WARNING: OFCommandLine::findOption() parameter 'pos' (" << pos << ") ignored !" << OFendl;
            ofConsole.unlockCerr();
        }
#endif
        OFListIterator(OFListIterator_OFString) pos_iter = OptionPosList.begin();
        const OFListIterator(OFListIterator_OFString) pos_end = OptionPosList.end();
        if (mode == FOM_NextFromLeft)
            pos_iter = (OptionPosIterator == pos_end) ? pos_end : ++OptionPosIterator;
        while (pos_iter != pos_end)
        {
            ArgumentIterator = *pos_iter;
            if (*ArgumentIterator == longOpt)                              // searched option
            {
                OptionPosIterator = pos_iter;                              // store option position
                return OFTrue;
            }
            pos_iter++;
        }
    } else {
        // normal direction (right to left)
        OFListIterator(OFListIterator_OFString) pos_iter = (mode == FOM_Next) ? OptionPosIterator : OptionPosList.end();
        const OFListIterator(OFListIterator_OFString) pos_first = OptionPosList.begin();
        OFListIterator(OFCmdParamPos *) param_iter;
        int diropt = 0;
        if (findParam(abs(pos), param_iter))                               // go to specified parameter position
        {
            diropt = (*param_iter)->DirectOption;                          // number of direct predecessors
            if (((*param_iter)->OptionCount == 0) ||                       // no options in front of specified parameter or
                ((pos < 0) && (diropt == 0)))                              // no 'direct' option ...
                    return OFFalse;
            pos_iter = (*param_iter)->OptionIter;                          // first option in front of parameter
            ++pos_iter;                                                    // goto next to facilitate loop condition
        }
        while (pos_iter != pos_first)
        {
            ArgumentIterator = *(--pos_iter);
            if (OptionBlockMode && (pos_iter == OptionBlockIterator))      // new option is in front of alternative block option
                return OFFalse;
            else if (*ArgumentIterator == longOpt)                         // searched option
            {
                OptionPosIterator = pos_iter;                              // store option position
                if (mode == FOM_Normal)
                    OptionBlockIterator = pos_iter;
                return OFTrue;
            }
            else if ((pos < 0) && (--diropt <= 0))                         // search only for the direct predecessor
                return OFFalse;
        }
    }
    return OFFalse;
}


OFBool OFCommandLine::gotoFirstOption()
{
    OptionPosIterator = OptionPosList.begin();
    if (OptionPosIterator != OptionPosList.end())
    {
        ArgumentIterator = *OptionPosIterator;
        return OFTrue;
    }
    return OFFalse;
}


OFBool OFCommandLine::gotoNextOption()
{
    if (OptionPosIterator != OptionPosList.end())
    {
        if (++OptionPosIterator != OptionPosList.end())
        {
            ArgumentIterator = *OptionPosIterator;
            return OFTrue;
        }
    }
    return OFFalse;
}


OFBool OFCommandLine::getCurrentOption(const char *&opt)
{
    if (OptionPosIterator != OptionPosList.end())
        return (opt = (**OptionPosIterator).c_str())[0] != '\0';
    return OFFalse;
}


OFBool OFCommandLine::getCurrentOption(OFCmdString &opt)
{
    if (OptionPosIterator != OptionPosList.end())
        return !(opt = **OptionPosIterator).empty();
    return OFFalse;
}


void OFCommandLine::beginOptionBlock()
{
    OptionBlockIterator = OptionPosList.end();
    OptionBlockMode = OFTrue;
}


void OFCommandLine::endOptionBlock()
{
    OptionBlockMode = OFFalse;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdSignedInt &value)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        if (sscanf((*ArgumentIterator).c_str(), "%li", &value) == 1)
            return VS_Normal;
        return VS_Invalid;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMin(OFCmdSignedInt &value,
                                                                const OFCmdSignedInt low,
                                                                const OFExplicitBool incl)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return VS_Underflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMinMax(OFCmdSignedInt &value,
                                                                   const OFCmdSignedInt low,
                                                                   const OFCmdSignedInt high)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if (value < low)
            return VS_Underflow;
        else if (value > high)
            return VS_Overflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdUnsignedInt &value)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        OFString &strVal = *ArgumentIterator;
        if (sscanf(strVal.c_str(), "%lu", &value) == 1)
        {
            // skip leading spaces
            size_t strPos = strVal.find_first_not_of(' ');
            // check for minus sign (negative number)
            if ((strPos != OFString_npos) && (strVal.at(strPos) != '-'))
                return VS_Normal;
        }
        return VS_Invalid;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMin(OFCmdUnsignedInt &value,
                                                                const OFCmdUnsignedInt low,
                                                                const OFExplicitBool incl)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return VS_Underflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMinMax(OFCmdUnsignedInt &value,
                                                                   const OFCmdUnsignedInt low,
                                                                   const OFCmdUnsignedInt high)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if (value < low)
            return VS_Underflow;
        else if (value > high)
            return VS_Overflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdFloat &value)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        OFBool success = OFFalse;
        value = OFStandard::atof((*ArgumentIterator).c_str(), &success);
        if (success) return VS_Normal;
        return VS_Invalid;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMin(OFCmdFloat &value,
                                                                const OFCmdFloat low,
                                                                const OFExplicitBool incl)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if ((value < low) || (!incl && (value == low)))
            return VS_Underflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMinMax(OFCmdFloat &value,
                                                                   const OFCmdFloat low,
                                                                   const OFCmdFloat high)
{
    E_ValueStatus status = getValue(value);
    if (status == VS_Normal)
    {
        if (value < low)
            return VS_Underflow;
        else if (value > high)
            return VS_Overflow;
    }
    return status;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(const char *&value)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        value = (*ArgumentIterator).c_str();
        if (strlen(value) > 0)
            return VS_Normal;
        return VS_Empty;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdString &value)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        value = *ArgumentIterator;
        if (!value.empty())
            return VS_Normal;
        return VS_Empty;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFFilename &filename)
{
    if (++ArgumentIterator != ArgumentList.end())
    {
        OFString strValue = *ArgumentIterator;
        if (!strValue.empty())
        {
            filename.set(strValue, WideCharMode /* convert */);
            return VS_Normal;
        } else
            filename.clear();
        return VS_Empty;
    }
    return VS_NoMore;
}


const OFCmdOption *OFCommandLine::findCmdOption(const OFString &option) const
{
    OFListConstIterator(OFCmdOption *) iter = ValidOptionList.begin();
    OFListConstIterator(OFCmdOption *) last = ValidOptionList.end();
    while (iter != last)
    {
        if (((*iter)->LongOption == option) || ((*iter)->ShortOption == option))
            return *iter;
        ++iter;
    }
    return NULL;
}


void OFCommandLine::storeParameter(const OFString &param,
                                   const int directOpt)
{
    ArgumentList.push_back(param);
    const OFListIterator(OFListIterator_OFString) iter = (OptionPosList.size() == 0) ? OptionPosList.end() : --OptionPosList.end();
    OFCmdParamPos *paramPos = new OFCmdParamPos(--ArgumentList.end(), iter, OFstatic_cast(int, OptionPosList.size()), directOpt);
    if (paramPos != NULL)
        ParamPosList.push_back(paramPos);
}


int OFCommandLine::packColumnValues(int longCols,
                                    int shortCols) const
{
    if (longCols < 0)
        longCols = 0;
    if (shortCols < 0)
        shortCols = 0;
    return ((longCols & 0xffff) << 16) | (shortCols & 0xffff);
}


void OFCommandLine::unpackColumnValues(const int value,
                                       unsigned int &longCols,
                                       unsigned int &shortCols) const
{
    longCols = (value == 0) ? LongColumn : (value >> 16) & 0xffff;
    shortCols = (value == 0) ? ShortColumn : (value & 0xffff);
}


OFCommandLine::E_ParseStatus OFCommandLine::checkParamCount()
{
    MinParamCount = 0;
    MaxParamCount = 0;
    OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
    const OFListIterator(OFCmdParam *) last = ValidParamList.end();
    while (iter != last)
    {
        if (!(*iter)->ParamName.empty())
        {
            switch ((*iter)->ParamMode)
            {
                case OFCmdParam::PM_Mandatory:
                    MinParamCount++;
                    if (MaxParamCount >= 0)
                        MaxParamCount++;
                    break;
                case OFCmdParam::PM_MultiMandatory:
                    MinParamCount++;
                    MaxParamCount = -1;
                    break;
                case OFCmdParam::PM_Optional:
                    if (MaxParamCount >= 0)
                        MaxParamCount++;
                    break;
                case OFCmdParam::PM_MultiOptional:
                    MaxParamCount = -1;
                    break;
            }
        }
        ++iter;
    }
    if (getArgCount() == 0)
        return PS_NoArguments;
    else if (hasExclusiveOption())
        return PS_ExclusiveOption;
    else if (getParamCount() < MinParamCount)
        return PS_MissingParameter;
    else if ((MaxParamCount >= 0) && (getParamCount() > MaxParamCount))
        return PS_TooManyParameters;
    return PS_Normal;
}


OFCommandLine::E_ParseStatus OFCommandLine::parseCommandFile(const char *argValue,
                                                             OFList<OFString> &argList)
{
    E_ParseStatus result = PS_NoArguments;
    /* check for command file parameter (syntax: "@filename") */
    if ((argValue != NULL) && (argValue[0] == COMMAND_FILE_PREFIX) && (argValue[1] != '\0'))
    {
        /* skip '@' symbol in filename */
        const char *filename = argValue + 1;
        /* open command file */
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
        STD_NAMESPACE ifstream cmdFile(filename, STD_NAMESPACE ios::in|STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream cmdFile(filename, STD_NAMESPACE ios::in);
#endif
        if (cmdFile)
        {
            char c, block = 0;
            OFString value;
            /* append command file content to the list of arguments */
            while (cmdFile.get(c))
            {
                /* double or single quote */
                if ((c == '"') || (c == '\''))
                {
                    if (block == c)
                    {
                        /* closing current quote block (also empty value) */
                        argList.push_back(value);
                        value.clear();
                        block = 0;
                    }
                    else if (block == 0)
                    {
                        /* starting new quote block */
                        block = c;
                    } else {
                        /* append character */
                        value += c;
                    }
                }
                /* space, tab, newline or return */
                else if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                {
                    /* outside block quote? */
                    if (block == 0)
                    {
                        if (!value.empty())
                        {
                            /* and add current value to the list of arguments */
                            argList.push_back(value);
                            value.clear();
                        }
                    } else
                        value += c;
                } else
                    value += c;
            }
            /* append remaining argument (if any) */
            if (!value.empty())
                argList.push_back(value);
#ifdef DEBUG
            if (block != 0)
            {
                ofConsole.lockCerr() << "WARNING: closing quotation mark (" << block << ") missing in command file " << filename << OFendl;
                ofConsole.unlockCerr();
            }
#endif
            result = PS_Normal;
        } else
            result = PS_CannotOpenCommandFile;
    }
    return result;
}


#ifdef DCMTK_USE_WCHAR_T

// Windows-specific version with wide character strings (UTF-16)
OFCommandLine::E_ParseStatus OFCommandLine::parseCommandFile(const wchar_t *argValue,
                                                             const OFString &strValue,
                                                             OFList<OFString> &argList)
{
    E_ParseStatus result = PS_NoArguments;
    /* check for command file parameter (syntax: "@filename") */
    if ((argValue != NULL) && (argValue[0] == WIDE_COMMAND_FILE_PREFIX) && (argValue[1] != L'\0'))
    {
        /* skip '@' symbol in filename */
        const wchar_t *filename = argValue + 1;
        /* open command file */
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
        STD_NAMESPACE wifstream cmdFile(filename, STD_NAMESPACE ios::in|STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE wifstream cmdFile(filename, STD_NAMESPACE ios::in);
#endif
        if (cmdFile)
        {
            wchar_t c, block = 0;
            STD_NAMESPACE wstring value;
            /* append command file content to the list of arguments */
            while (cmdFile.get(c))
            {
                /* double or single quote */
                if ((c == L'"') || (c == L'\''))
                {
                    if (block == c)
                    {
                        /* closing current quote block (also empty value) */
                        OFString tmpString;
                        /* convert current value to UTF-8 */
                        OFCharacterEncoding::convertFromWideCharString(value.c_str(), value.length(),
                            tmpString, OFCharacterEncoding::CPC_UTF8);
                        /* ... and add it to the list of arguments */
                        argList.push_back(tmpString);
                        value.clear();
                        block = 0;
                    }
                    else if (block == 0)
                    {
                        /* starting new quote block */
                        block = c;
                    } else {
                        /* append character */
                        value += c;
                    }
                }
                /* space, tab, newline or return */
                else if ((c == L' ') || (c == L'\t') || (c == L'\n') || (c == L'\r'))
                {
                    /* outside block quote? */
                    if (block == 0)
                    {
                        if (!value.empty())
                        {
                            OFString tmpString;
                            /* convert current value to UTF-8 */
                            OFCharacterEncoding::convertFromWideCharString(value.c_str(), value.length(),
                                tmpString, OFCharacterEncoding::CPC_UTF8);
                            /* ... and add it to the list of arguments */
                            argList.push_back(tmpString);
                            value.clear();
                        }
                    } else
                        value += c;
                } else
                    value += c;
            }
            /* append remaining argument (if any) */
            if (!value.empty())
            {
                OFString tmpString;
                /* convert current value to UTF-8 */
                OFCharacterEncoding::convertFromWideCharString(value.c_str(), value.length(), tmpString,
                    OFCharacterEncoding::CPC_UTF8);
                /* ... and add it to the list of arguments */
                argList.push_back(tmpString);
            }
#ifdef DEBUG
            if (block != 0)
            {
                ofConsole.lockCerr() << "WARNING: closing quotation mark (" << block << ") missing in command file " << strValue << OFendl;
                ofConsole.unlockCerr();
            }
#endif
            result = PS_Normal;
        } else
            result = PS_CannotOpenCommandFile;
    }
    return result;
}

#endif  // DCMTK_USE_WCHAR_T


OFCommandLine::E_ParseStatus OFCommandLine::parseArgumentList(OFList<OFString> &argList,
                                                              const int /*flags*/)
{
    ArgumentList.clear();                                                // initialize lists/positions
    ParamPosList.clear();
    OptionPosList.clear();
    ParamPosNumber = 0;
    ExclusiveOption = OFFalse;

    int directOption = 0;                                                // number of direct predecessor
    int i = OFstatic_cast(int, argList.size());
    OFListIterator(OFString) argIter = argList.begin();
    const OFListIterator(OFString) argEnd = argList.end();
    /* iterate over all command line arguments */
    while (argIter != argEnd)
    {
        if (!checkOption(*argIter, OFFalse))                             // arg = parameter
        {
            storeParameter(*argIter, directOption);
            directOption = 0;
        } else {                                                         // arg = option
            const OFCmdOption *opt = findCmdOption(*argIter);
            if (opt != NULL)
            {
                ArgumentList.push_back(OFstatic_cast(OFString, opt->LongOption)); // convert argument to long format
                OptionPosList.push_back(--ArgumentList.end());
                if (opt->Flags & AF_Exclusive)                           // check for an "exclusive" option
                    ExclusiveOption = OFTrue;
                directOption++;
                int j = opt->ValueCount;                                 // number of expected values
                if (j >= i)
                    return PS_MissingValue;                              // expecting more values than present
                while (j-- > 0)
                {
                    ArgumentList.push_back(*(++argIter));                // add values to argument list
                    i--;
                }
            } else {                                                     // unknown
                ArgumentList.push_back(*argIter);                        // store argument
                return PS_UnknownOption;
            }
        }
        ++argIter;
        i--;
    }
    return checkParamCount();
}


OFCommandLine::E_ParseStatus OFCommandLine::parseLine(int argCount,
                                                      char *argValue[],
                                                      const int flags,
                                                      const int startPos)
{
    OFList<OFString> argList;                                            // "expanded" list of arguments
    WideCharMode = OFFalse;
    if (argCount > 0)                                                    // determine program name
        ProgramName = argValue[0];
    else
        ProgramName.clear();
    if (argCount > startPos)                                             // any command line arguments?
    {
        /* expand command files (if any) */
        for (int i = startPos; i < argCount; i++)                        // skip program name (argValue[0])
        {
            if (flags & PF_NoCommandFiles)                               // do not try to detect command files
                argList.push_back(argValue[i]);
            else
            {
                /* parse command file content */
                E_ParseStatus status = parseCommandFile(argValue[i], argList);
                if (status == PS_NoArguments)
                    argList.push_back(argValue[i]);                      // store parameter as is
                else if (status != PS_Normal)
                {
                    ArgumentList.push_back(argValue[i] + 1);             // store filename for error message
                    return status;
                }
            }
        }
    }
    /* call the real function after the preparatory work is done */
    return parseArgumentList(argList, flags);
}


#ifdef DCMTK_USE_WCHAR_T

// Windows-specific version with wide character strings (UTF-16)
OFCommandLine::E_ParseStatus OFCommandLine::parseLine(int argCount,
                                                      wchar_t *argValue[],
                                                      const int flags,
                                                      const int startPos)
{
    OFList<OFString> argList;                                            // "expanded" list of arguments
    WideCharMode = OFTrue;
    if (argCount > 0)                                                    // determine program name (convert to UTF-8)
    {
        OFCharacterEncoding::convertFromWideCharString(argValue[0], wcslen(argValue[0]),
            ProgramName, OFCharacterEncoding::CPC_UTF8);
    } else
        ProgramName.clear();
    if (argCount > startPos)                                             // any command line arguments?
    {
        OFString strValue;
        /* expand command files (if any) */
        for (int i = startPos; i < argCount; i++)                        // skip program name (argValue[0])
        {
            OFCharacterEncoding::convertFromWideCharString(argValue[i], wcslen(argValue[i]),
                strValue, OFCharacterEncoding::CPC_UTF8, OFTrue /* clearMode */);
            if (flags & PF_NoCommandFiles)                               // do not try to detect command files
                argList.push_back(strValue);
            else
            {
                /* parse command file content */
                E_ParseStatus status = parseCommandFile(argValue[i], strValue, argList);
                if (status == PS_NoArguments)
                    argList.push_back(strValue);                         // store parameter as is
                else if (status != PS_Normal)
                {
                    ArgumentList.push_back(strValue.c_str() + 1);        // store filename for error message
                    return status;
                }
            }
        }
    }
    /* call the real function after the preparatory work is done */
    return parseArgumentList(argList, flags);
}

#endif  // DCMTK_USE_WCHAR_T


void OFCommandLine::getSyntaxString(OFString &syntaxStr) const
{
    syntaxStr.clear();
    if (!ValidOptionList.empty())
        syntaxStr += " [options]";
    if (!ValidParamList.empty())
    {
        OFListConstIterator(OFCmdParam *) iter = ValidParamList.begin();
        OFListConstIterator(OFCmdParam *) last = ValidParamList.end();
        while (iter != last)
        {
            if (!(*iter)->ParamName.empty())
            {
                switch ((*iter)->ParamMode)
                {
                    case OFCmdParam::PM_Mandatory:
                        syntaxStr += " ";
                        syntaxStr += (*iter)->ParamName;
                        break;
                    case OFCmdParam::PM_Optional:
                        syntaxStr += " [";
                        syntaxStr += (*iter)->ParamName;
                        syntaxStr += "]";
                        break;
                    case OFCmdParam::PM_MultiMandatory:
                        syntaxStr += " ";
                        syntaxStr += (*iter)->ParamName;
                        syntaxStr += "...";
                        break;
                    case OFCmdParam::PM_MultiOptional:
                        syntaxStr += " [";
                        syntaxStr += (*iter)->ParamName;
                        syntaxStr += "...]";
                        break;
                }
            }
            ++iter;
        }
    }
}


void OFCommandLine::getOptionString(OFString &optionStr) const
{
    optionStr.clear();
    if (!ValidOptionList.empty())
    {
        OFListConstIterator(OFCmdOption *) iter = ValidOptionList.begin();
        OFListConstIterator(OFCmdOption *) last = ValidOptionList.end();
        OFString str;
        int newGrp = 1;
        unsigned int shortSize = ShortColumn;
        unsigned int longSize = LongColumn;
        unsigned int lineIndent = 0;
        const unsigned int groupIndent = 2;
        const unsigned int subGrpIndent = 4;
        const unsigned int columnSpace = 2;
        while (iter != last)
        {
            /* skip internal options */
            if (!((*iter)->Flags & OFCommandLine::AF_Internal))
            {
                if (newGrp)
                {
                    /* determine column width per group */
                    OFListConstIterator(OFCmdOption *) i = iter;
                    while ((i != last) && !(*i)->LongOption.empty())
                    {
                        if (!((*i)->Flags & OFCommandLine::AF_Internal))
                        {
                            if ((*i)->ShortOption.length() > shortSize)
                                shortSize = OFstatic_cast(unsigned int, (*i)->ShortOption.length());
                            if ((*i)->LongOption.length() > longSize)
                                longSize = OFstatic_cast(unsigned int, (*i)->LongOption.length());
                        }
                        i++;
                    }
                    newGrp = 0;
                }
                if ((*iter)->LongOption.empty())
                {
                    /* group entry */
                    newGrp = 1;
                    unpackColumnValues((*iter)->ValueCount, longSize, shortSize);
                    if (!(*iter)->OptionDescription.empty())                     // new group
                    {
                        optionStr += (*iter)->OptionDescription;
                        lineIndent = groupIndent;
                    } else {                                                     // new sub group
                        optionStr.append(groupIndent, ' ');
                        optionStr += (*iter)->ValueDescription;
                        lineIndent = subGrpIndent;
                    }
                    optionStr += "\n";
                } else {
                    /* regular option */
                    optionStr.append(lineIndent, ' ');
                    if (shortSize > 0)
                    {
                        str = (*iter)->ShortOption;
                        str.resize(shortSize, ' ');
                        optionStr += str;
                        optionStr.append(columnSpace, ' ');
                    }
                    str = (*iter)->LongOption;
                    str.resize(longSize, ' ');
                    optionStr += str;
                    optionStr.append(columnSpace, ' ');
                    if (!(*iter)->ValueDescription.empty())
                    {
                        optionStr += (*iter)->ValueDescription;
                        optionStr += "\n";
                        optionStr.append(lineIndent + shortSize + longSize + columnSpace, ' ');
                        if (shortSize > 0)
                            optionStr.append(columnSpace, ' ');
                    }
                    str = (*iter)->OptionDescription;
                    size_t pos = 0;
                    while (((pos = (str.find('\n', pos))) != OFString_npos) && (pos < str.length()))
                        str.insert(++pos, OFString(lineIndent + shortSize + longSize + 2 * columnSpace, ' '));
                    optionStr += str;
                    optionStr += "\n";
                }
            }
            ++iter;
        }
    }
}


void OFCommandLine::getParamString(OFString &paramStr) const
{
    paramStr.clear();
    if (!ValidParamList.empty())
    {
        OFListConstIterator(OFCmdParam *) iter = ValidParamList.begin();
        OFListConstIterator(OFCmdParam *) last = ValidParamList.end();
        OFString str;
        unsigned int columnSize = ParamColumn;
        const unsigned int lineIndent = 2;
        const unsigned int columnSpace = 2;
        while ((iter != last) && !(*iter)->ParamDescription.empty())
        {
            if ((*iter)->ParamName.length() > columnSize)           // determine maximum column width
                columnSize = OFstatic_cast(unsigned int, (*iter)->ParamName.length());
            ++iter;
        }
        iter = ValidParamList.begin();                              // reset iterator
        while (iter != last)
        {
            if (!(*iter)->ParamDescription.empty())
            {
                if (paramStr.empty())
                    paramStr += "parameters:\n";
                paramStr.append(lineIndent, ' ');
                str = (*iter)->ParamName;
                str.resize(columnSize, ' ');
                paramStr += str;
                paramStr.append(columnSpace, ' ');
                str = (*iter)->ParamDescription;
                size_t pos = 0;
                while (((pos = (str.find('\n', pos))) != OFString_npos) && (pos < str.length()))
                    str.insert(++pos, OFString(lineIndent + columnSize + columnSpace, ' '));
                paramStr += str;
                paramStr += "\n";
            }
            ++iter;
        }
    }
}


OFBool OFCommandLine::getMissingParam(OFString &param)
{
    if (!ValidParamList.empty() && (getParamCount() < MinParamCount))
    {
        OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
        const OFListIterator(OFCmdParam *) last = ValidParamList.end();
        int i = getParamCount();
        while ((iter != last) && (i-- > 0))
            ++iter;
        if (iter != last)
        {
            param = (*iter)->ParamName;
            return OFTrue;
        }
    }
    return OFFalse;
}


void OFCommandLine::getStatusString(const E_ParseStatus status,
                                    OFString &statusStr)
{
    OFString str;
    switch (status)
    {
        case PS_UnknownOption:
            statusStr = "Unknown option ";
            if (getLastArg(str))
                statusStr += str;
            break;
        case PS_MissingValue:
            statusStr = "Missing value for option ";
            if (getLastArg(str))
                statusStr += str;
            break;
        case PS_MissingParameter:
            statusStr = "Missing parameter ";
            if (getMissingParam(str))
                statusStr += str;
            break;
        case PS_TooManyParameters:
            statusStr = "Too many parameters";
            break;
        case PS_CannotOpenCommandFile:
            statusStr = "Cannot open command file";
            if (getLastArg(str))
            {
                statusStr += " '";
                statusStr += str;
                statusStr += "'";
            }
            break;
        default:
            statusStr.clear();
            break;
    }
}


void OFCommandLine::getStatusString(const E_ParamValueStatus status,
                                    OFString &statusStr)
{
    OFString str;
    switch (status)
    {
        case PVS_Invalid:
            statusStr = "Invalid parameter value ";
            if (getCurrentArg(str))
                statusStr += str;
            break;
        case PVS_CantFind:
            statusStr = "Can't find parameter";
            break;
        case PVS_Underflow:
            statusStr = "Invalid parameter value ";
            if (getCurrentArg(str))
            {
                statusStr += str;
                statusStr += " (underflow)";
            }
            break;
        case PVS_Overflow:
            statusStr = "Invalid parameter value ";
            if (getCurrentArg(str))
            {
                statusStr += str;
                statusStr += " (overflow)";
            }
            break;
        default:
            statusStr.clear();
            break;
    }
}


void OFCommandLine::getStatusString(const E_ValueStatus status,
                                    OFString &statusStr)
{
    OFString str;
    switch (status)
    {
        case VS_Normal:
            statusStr.clear();
            break;
        case VS_Invalid:
            statusStr = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                statusStr += str;
                if (getCurrentArg(str))
                {
                    statusStr += " (";
                    statusStr += str;
                    statusStr += ")";
                }
            }
            break;
        case VS_Underflow:
            statusStr = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                statusStr += str;
                if (getCurrentArg(str))
                {
                    statusStr += " (underflow: ";
                    statusStr += str;
                    statusStr += ")";
                }
            }
            break;
        case VS_Overflow:
            statusStr = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                statusStr += str;
                if (getCurrentArg(str))
                {
                    statusStr += " (overflow: ";
                    statusStr += str;
                    statusStr += ")";
                }
            }
            break;
        default:
            statusStr.clear();
            break;
    }
}
