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
 *  Purpose: Template class for command line arguments (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:52 $
 *  CVS/RCS Revision: $Revision: 1.50 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofcast.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define COMMAND_FILE_PREFIX '@'


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int OFCommandLine::PF_ExpandWildcards = 0x0001;
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
        if (!Checked && !(Flags & (OFCommandLine::AF_Exclusive | OFCommandLine::AF_NoWarning)) && (LongOption.length() > 0))
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
    OptionPosList(),
    OptionPosIterator(),
    OptionBlockIterator(),
    OptionBlockMode(OFFalse),
    OptionChars("+-"),
    ExclusiveOption(OFFalse),
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
        return (arg = *ArgumentIterator).length() > 0;
    return OFFalse;

}


OFBool OFCommandLine::getLastArg(OFString &arg)
{
    if (!ArgumentList.empty())
        return (arg = ArgumentList.back()).length() > 0;
    return OFFalse;
}


OFBool OFCommandLine::findParam(const int pos)
{
    OFListIterator(OFCmdParamPos *) iter;
    return findParam(pos, iter);
}


OFBool OFCommandLine::findParam(int pos,
                                OFListIterator(OFCmdParamPos *) &pos_iter)
{
    if ((pos > 0) && (pos <= getParamCount()))
    {
        pos_iter = ParamPosList.begin();
        const OFListIterator(OFCmdParamPos *) pos_last = ParamPosList.end();
        while (pos_iter != pos_last)
        {
            ArgumentIterator = (*pos_iter)->ParamIter;
            if (--pos == 0)
                return OFTrue;
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
                                                                     const OFBool incl)
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
                                                                     const OFBool incl)
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
                                                                     const OFBool incl)
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
                                                          const char *&param)
{
    if (findParam(pos))
    {
        param = (*ArgumentIterator).c_str();
        if (strlen(param) > 0)
            return PVS_Normal;
        return PVS_Empty;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
                                                          OFCmdString &param)
{
    if (findParam(pos))
    {
        param = *ArgumentIterator;
        if (param.length() > 0)
            return PVS_Normal;
        return PVS_Empty;
    }
    return PVS_CantFind;
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
        return strlen(opt = (**OptionPosIterator).c_str()) > 0;
    return OFFalse;
}


OFBool OFCommandLine::getCurrentOption(OFCmdString &opt)
{
    if (OptionPosIterator != OptionPosList.end())
        return (opt = **OptionPosIterator).length() > 0;
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
                                                                const OFBool incl)
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
        if (sscanf((*ArgumentIterator).c_str(), "%lu", &value) == 1)
            return VS_Normal;
        return VS_Invalid;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValueAndCheckMin(OFCmdUnsignedInt &value,
                                                                const OFCmdUnsignedInt low,
                                                                const OFBool incl)
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
                                                                const OFBool incl)
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
        if (value.length() > 0)
            return VS_Normal;
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
    OFCmdParamPos *paramPos = new OFCmdParamPos(--ArgumentList.end(), iter, OptionPosList.size(), directOpt);
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


#ifdef HAVE_WINDOWS_H
void OFCommandLine::expandWildcards(const OFString &param,
                                    int directOpt)
{
    const size_t paramLen = param.length();
    if ((paramLen >= 2) && (param.at(0) == '"') && (param.at(paramLen - 1) == '"'))
        storeParameter(param.substr(1, paramLen - 2).c_str(), directOpt);  // remove quotations
    else
    {
        size_t pos1 = param.find_first_of("*?");                 // search for wildcards
        if (pos1 != OFString_npos)
        {
            OFString name;
            WIN32_FIND_DATA data;
            size_t pos2 = param.find_first_of(":\\", pos1);      // search for next separator (":" or "\")
            HANDLE handle = FindFirstFile(param.substr(0, pos2).c_str(), &data);
            if (handle != INVALID_HANDLE_VALUE)                  // find first file/dir matching the wildcards
            {
                do {
                    if ((strcmp(data.cFileName, ".") != 0) && (strcmp(data.cFileName, "..") != 0))
                    {                                            // skip "." and ".."
                        size_t pos3 = param.find_last_of(":\\", pos1);
                        if (pos3 != OFString_npos)
                            name = param.substr(0, pos3 + 1) + data.cFileName;
                        else
                            name = data.cFileName;               // no path specified
                        if (pos2 != OFString_npos)
                            name += param.substr(pos2);
                        if (GetFileAttributes(name.c_str()) != 0xFFFFFFFF)
                        {
                            storeParameter(name.c_str(), directOpt);   // file/dir does exist
                            directOpt = 0;                             // only valid for first expanded parameter (tbt!)
                        } else
                            expandWildcards(name.c_str(), directOpt);  // recursively expand further wildcards
                    }
                } while (FindNextFile(handle, &data));           // while further files/dirs exist ... add them
                FindClose(handle);
            }
        } else
            storeParameter(param, directOpt);                    // parameter contains no wildcards, just add it
    }
}
#endif


OFCommandLine::E_ParseStatus OFCommandLine::checkParamCount()
{
    MinParamCount = 0;
    MaxParamCount = 0;
    OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
    const OFListIterator(OFCmdParam *) last = ValidParamList.end();
    while (iter != last)
    {
        if ((*iter)->ParamName.length() > 0)
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
    if ((argValue != NULL) && (strlen(argValue) > 1) && (argValue[0] == COMMAND_FILE_PREFIX))
    {
        /* skip '@' symbol in filename */
        const char *filename = argValue + 1;
        /* open command file */
#ifdef HAVE_IOS_NOCREATE
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


OFCommandLine::E_ParseStatus OFCommandLine::parseLine(int argCount,
                                                      char *argValue[],
                                                      const int flags,
                                                      const int startPos)
{
    ArgumentList.clear();                                                // initialize lists
    ParamPosList.clear();
    OptionPosList.clear();
    ExclusiveOption = OFFalse;
    if (argCount > 0)                                                    // determine program name
        ProgramName = argValue[0];
    else
        ProgramName.clear();
    if (argCount > startPos)                                             // any command line arguments?
    {
        int i;
        int directOption = 0;                                            // number of direct predecessor
        OFList<OFString> argList;                                        // "expanded" list of arguments
        /* expand command files (if any) */
        for (i = startPos; i < argCount; i++)                            // skip program name (argValue[0])
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
        i = argList.size();
        OFListIterator(OFString) argIter = argList.begin();
        const OFListIterator(OFString) argEnd = argList.end();
        /* iterate over all command line arguments */
        while (argIter != argEnd)
        {
            if (!checkOption(*argIter, OFFalse))                         // arg = parameter
            {
#ifdef HAVE_WINDOWS_H
                if (flags & PF_ExpandWildcards)                          // expand wildcards
                    expandWildcards(*argIter, directOption);
                else
#endif
                    storeParameter(*argIter, directOption);
                directOption = 0;
            } else {                                                     // arg = option
                const OFCmdOption *opt = findCmdOption(*argIter);
                if (opt != NULL)
                {
                    ArgumentList.push_back(OFstatic_cast(OFString, opt->LongOption)); // convert argument to long format
                    OptionPosList.push_back(--ArgumentList.end());
                    if (opt->Flags & AF_Exclusive)                       // check for an "exclusive" option
                        ExclusiveOption = OFTrue;
                    directOption++;
                    int j = opt->ValueCount;                             // number of expected values
                    if (j >= i)
                        return PS_MissingValue;                          // expecting more values than present
                    while (j-- > 0)
                    {
                        ArgumentList.push_back(*(++argIter));            // add values to argument list
                        i--;
                    }
                } else {                                                 // unknown
                    ArgumentList.push_back(*argIter);                    // store argument
                    return PS_UnknownOption;
                }
            }
            ++argIter;
            i--;
        }
    }
    return checkParamCount();
}


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
            if ((*iter)->ParamName.length() > 0)
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
                    while ((i != last) && ((*i)->LongOption.length() > 0))
                    {
                        if (!((*i)->Flags & OFCommandLine::AF_Internal))
                        {
                            if ((*i)->ShortOption.length() > shortSize)
                                shortSize = (*i)->ShortOption.length();
                            if ((*i)->LongOption.length() > longSize)
                                longSize = (*i)->LongOption.length();
                        }
                        i++;
                    }
                    newGrp = 0;
                }
                if ((*iter)->LongOption.length() <= 0)
                {
                    /* group entry */
                    newGrp = 1;
                    unpackColumnValues((*iter)->ValueCount, longSize, shortSize);
                    if ((*iter)->OptionDescription.length() > 0)                 // new group
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
                    if ((*iter)->ValueDescription.length() > 0)
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
        while ((iter != last) && ((*iter)->ParamDescription.length() > 0))
        {
            if ((*iter)->ParamName.length() > columnSize)           // determine maximum column width
                columnSize = (*iter)->ParamName.length();
            ++iter;
        }
        iter = ValidParamList.begin();                              // reset iterator
        while (iter != last)
        {
            if ((*iter)->ParamDescription.length() > 0)
            {
                if (paramStr.length() == 0)
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


/*
 *
 * CVS/RCS Log:
 * $Log: ofcmdln.cc,v $
 * Revision 1.50  2010-10-14 13:14:52  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.49  2010-06-21 16:32:51  joergr
 * Removed wrong warning message on an optional parameter hiding another one.
 *
 * Revision 1.48  2009-12-23 12:19:02  joergr
 * Added support for getting the name of the program, i.e. the value of argv[0].
 *
 * Revision 1.47  2009-06-04 09:52:31  joergr
 * Added new flag that can be used to avoid wrong warning messages (in debug
 * mode) that an option has possibly never been checked.
 *
 * Revision 1.46  2008-09-24 13:04:58  joergr
 * Fixed typo in output of getStatusString().
 *
 * Revision 1.45  2008-04-16 12:39:40  joergr
 * Added support for reverse search direction (left to right) to findOption().
 *
 * Revision 1.44  2008-03-17 10:08:41  onken
 * Corrected "or" to "||".
 *
 * Revision 1.43  2008-03-11 16:18:56  joergr
 * Moved warning message on missing quotation mark when reading command files.
 *
 * Revision 1.42  2008-03-03 13:16:39  joergr
 * Enhanced support for quotation marks in command files.
 *
 * Revision 1.41  2007/03/16 11:20:44  joergr
 * Fixed wrong warning message about unchecked options in debug mode.
 *
 * Revision 1.40  2006/08/21 12:41:52  meichel
 * Updated code to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.39  2006/08/14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.38  2006/07/27 15:49:08  joergr
 * Fixed typo which caused compilation problems under Windows.
 *
 * Revision 1.37  2006/07/27 13:21:28  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags".
 * Added new addOption() flag for internal options that are not shown in the
 * syntax usage output. Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 * Slightly changed behaviour of "exclusive" options like "--help" or
 * "--version". Method parseLine() now returns PS_ExclusiveOption.
 *
 * Revision 1.36  2005/12/08 15:48:49  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.35  2003/12/05 13:58:28  joergr
 * Fixed problem with retrieving option values using the new iteration feature.
 *
 * Revision 1.34  2003/12/05 10:35:24  joergr
 * Added support for iterating over command line arguments and options.
 *
 * Revision 1.33  2003/07/09 13:58:04  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.32  2003/06/12 13:25:57  joergr
 * Added support for so-called command files ("@filename") which can be used to
 * summarize command line options and parameters.
 * Introduced macro OFListConstIterator() to support STL const_iterators.
 *
 * Revision 1.31  2003/05/20 08:47:50  joergr
 * Renamed parameters/variables "string" to avoid name clash with STL class.
 * Enhanced use of OFString routines.
 *
 * Revision 1.30  2002/09/19 08:30:32  joergr
 * Added general support for "exclusive" command line options besides "--help",
 * e.g. "--version".
 *
 * Revision 1.29  2002/06/20 12:04:36  meichel
 * Changed toolkit to use OFStandard::atof instead of atof, strtod or
 *   sscanf for all string to double conversions that are supposed to
 *   be locale independent
 *
 * Revision 1.28  2001/11/09 15:47:03  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 *
 * Revision 1.27  2001/06/01 15:51:37  meichel
 * Updated copyright header
 *
 * Revision 1.26  2000/06/02 12:39:56  joergr
 * Removed unnecessary warning message/condition (only in debug mode).
 *
 * Revision 1.25  2000/05/30 12:55:01  joergr
 * Adapted error output (avoid compiler warnings reported by gcc with
 * additional flags).
 *
 * Revision 1.24  2000/04/14 15:17:15  meichel
 * Adapted all ofstd library classes to consistently use ofConsole for output.
 *
 * Revision 1.23  2000/03/08 16:36:05  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/03 14:02:50  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.21  2000/03/02 12:40:39  joergr
 * Fixed inconsistency: console applications with no or only optional
 * parameters could not be started without any command line argument
 * because this was always regarded identical with "--help" (print usage).
 *
 * Revision 1.20  2000/02/02 10:56:43  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.19  1999/10/04 10:02:37  joergr
 * Fixed bug in wildcard expansion (concerning "direct option" feature).
 *
 * Revision 1.18  1999/09/13 17:06:56  joergr
 * Removed another (new) bug in findOption().
 *
 * Revision 1.17  1999/09/13 16:38:18  joergr
 * Corrected bug in OFCommandLine::findOption() regarding the optional
 * parameter 'pos' specifying a reference command line parameter.
 *
 * Revision 1.16  1999/09/06 16:48:32  joergr
 * Added support to method 'findOption()' to detect options which are
 * 'direct' predecessors of an optionally specified reference parameter.
 *
 * Revision 1.15  1999/04/29 13:46:36  joergr
 * Added checks whether an option which should be added already exists.
 *
 * Revision 1.14  1999/04/28 13:15:22  joergr
 * Removed some '#ifdef DEBUG' statements from header files to avoid
 * problems with inconsistent compilations.
 * Removed warning when adding optional parameter direct after another
 * optional parameter.
 *
 * Revision 1.13  1999/04/27 17:46:50  joergr
 * Corrected bug: option '--help' could not be used when mandatory parameters
 * were missing.
 *
 * Revision 1.11  1999/04/26 16:36:00  joergr
 * Added support to define minimum width of short and long option columns.
 * Removed bug: empty parameters have always been interpreted as options.
 * Enhanced support of wildcard expansion under Windows (now very similar
 * to Unix shells).
 *
 * Revision 1.10  1999/03/24 17:01:47  joergr
 * Added optional parameters to define minimum width of columns for short and
 * long options in syntax output.
 * Changed optional integer parameter in method findOption to enum type.
 *
 * Revision 1.9  1999/02/08 11:58:24  joergr
 * Removed name of parameter 'flags' in method parseLine() depending on
 * compilation platform (parameter is currently only used on Windows
 * systems - where windows.h exist) to avoid compiler warnings.
 *
 * Revision 1.8  1999/02/05 14:34:19  joergr
 * Corrected bug in wildcard expansion for Windows.
 *
 * Revision 1.7  1999/02/05 14:07:58  joergr
 * Introduced new preprocessor definition HAVE_WINDOWS_H.
 * Added automatic wildcard expansion for Windows compilers.
 *
 * Revision 1.6  1998/12/02 18:44:39  joergr
 * Introduced test whether added options are correct (starting with defined
 * option character followed by a character which is no number). Changed
 * parse routine to distinguish between options (normally starting mit - or
 * +) and signed numbers which can be valid parameters.
 *
 * Revision 1.5  1998/12/02 17:39:10  joergr
 * Introduced new enum type used to indicate the status when converting
 * parameter values (similar to option values). Changed return value of
 * getParam() methods to this type. Added corresponding getStatusString()
 * method to convert status code to strings.
 *
 * Revision 1.4  1998/12/02 15:20:05  joergr
 * Added methods to convert parameters to signed/unsigned integers and
 * floats. Changed return value of existing getParam() methods.
 *
 * Revision 1.3  1998/11/30 12:31:16  joergr
 * Use lists of pointers (!) to internal data structures to avoid errors with
 * MSVC5 (operator '==' was not defined to compare structures).
 *
 * Revision 1.2  1998/11/30 12:27:21  joergr
 * Introduced additional type definition to avoid errors with MSVC5 when
 * using ListIterators of ListIterators (syntax problems?).
 *
 * Revision 1.1  1998/11/27 12:34:23  joergr
 * Added class to handle command line arguments.
 *
 *
 */
