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
 *  Purpose: Template class for command line arguments (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-02 10:56:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofcmdln.cc,v $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofcmdln.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int OFCommandLine::ExpandWildcards = 0x0001;


/*------------------*
 *  implementation  *
 *------------------*/


OFCommandLine::OFCommandLine()
  : ValidOptionList(),
    ValidParamList(),
    ArgumentList(),
    ArgumentIterator(),
    ParamPosList(),
    OptionPosList(),
    OptionPosIterator(),
    OptionBlockIterator(),
    OptionBlockMode(OFFalse),
    OptionChars("+-"),
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
    OFListIterator(OFCmdOption *) last_o = ValidOptionList.end();
    while (first_o != last_o)
    {     
        delete (*first_o);
        first_o = ValidOptionList.erase(first_o);
    }
    OFListIterator(OFCmdParam *) first_p = ValidParamList.begin();
    OFListIterator(OFCmdParam *) last_p = ValidParamList.end();
    while (first_p != last_p)
    {     
        delete (*first_p);
        first_p = ValidParamList.erase(first_p);
    }
    OFListIterator(OFCmdParamPos *) first_pp = ParamPosList.begin();
    OFListIterator(OFCmdParamPos *) last_pp = ParamPosList.end();
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


OFBool OFCommandLine::checkOption(const char *string,
                                  const OFBool mode) const
{
    if (string != NULL)
    {
        if (strlen(string) > 0)                                                            // empty strings are allowed to support (sub)groups
        {
            if ((strlen(string) < 2) || (OptionChars.find(string[0]) == OFString_npos) ||  // options have to start with one of the defined chars
                (((string[0] == '-') || (string[0] == '+')) &&                             // but when starting with sign character ...
                (string[1] >= '0') && (string[1] <= '9')))                                 // ... don't allow a number as the following character
                   return OFFalse;
            return OFTrue;
        }
        return mode;
    }
    return OFFalse;
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *shortOpt,
                                const int valueCount,
                                const char *valueDescr,
                                const char *optDescr)
{
    if (checkOption(longOpt) && checkOption(shortOpt))
    {    
#ifdef DEBUG
        if (strlen(longOpt) > 0)
        {
            OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
            OFListIterator(OFCmdOption *) last = ValidOptionList.end();
            while (iter != last)
            {
                if ((*iter)->LongOption == longOpt)
                {
                    cerr << "WARNING: long option " << longOpt << " already defined ... not added !" << endl;
                    return OFFalse;
                }
                if ((strlen(shortOpt) > 0) && ((*iter)->ShortOption == shortOpt))
                {
                    cerr << "WARNING: short option " << shortOpt << " already defined for " << (*iter)->LongOption << " ..." << endl;
                    cerr << "         option " << longOpt << " not added !" << endl;
                    return OFFalse;
                }
                iter++;
            }
        }
#endif
        OFCmdOption *opt = new OFCmdOption(longOpt, shortOpt, valueCount, valueDescr, optDescr);
        if (opt != NULL)
        {
            ValidOptionList.push_back(opt);
            return OFTrue;
        }
    }
#ifdef DEBUG
    cerr << "WARNING: invalid option " << shortOpt << "/" <<longOpt << " ... not added !" << endl;
#endif
    return OFFalse;
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *shortOpt,
                                const char *optDescr)
{
    return addOption(longOpt, shortOpt, 0, "", optDescr);
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const int valueCount,
                                const char *valueDescr,
                                const char *optDescr)
{
    return addOption(longOpt, "", valueCount, valueDescr, optDescr);
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *optDescr)
{
    return addOption(longOpt, "", 0, "", optDescr);
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
                if (mode != OFCmdParam::PM_Optional)
                    cerr << "WARNING: " << ValidParamList.size() << ". parameter is optional => hides " << param << " !" << endl;
                break;
            case OFCmdParam::PM_MultiMandatory:
                cerr << "WARNING: " << ValidParamList.size() << ". parameter is multi_mandatory => hides " << param << " !" << endl;
                break;
            case OFCmdParam::PM_MultiOptional:
                cerr << "WARNING: " << ValidParamList.size() << ". parameter is multi_optional => hides " << param << " !" << endl;
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


OFBool OFCommandLine::getLastArg(const char *&arg)
{
    if (!ArgumentList.empty())
        return strlen(arg = (ArgumentList.back()).c_str()) > 0;
    return OFFalse;
}


OFBool OFCommandLine::getLastArg(OFCmdString &arg)
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
        OFListIterator(OFCmdParamPos *) pos_last = ParamPosList.end();
        while (pos_iter != pos_last)
        {
            ArgumentIterator = (*pos_iter)->ParamIter;
            if (--pos == 0)
                return OFTrue;
            pos_iter++;
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


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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
        if (sscanf((*ArgumentIterator).c_str(), "%lf", &value) == 1)
            return PVS_Normal;
        return PVS_Invalid;
    }
    return PVS_CantFind;
}


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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


OFCommandLine::E_ParamValueStatus OFCommandLine::getParam(const int pos,
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
    OFListIterator(OFCmdOption *) last = ValidOptionList.end();
    while (iter != last)
    {
        if ((*iter)->LongOption == longOpt)
        {
            (*iter)->Checked = OFTrue;
            break;
        }
        iter++;
    }
    if (iter == last)
    {
        cerr << "WARNING: unknown option " << longOpt << " in 'OFCommandLine::findOption()' !" << endl;
        return OFFalse;
    }    
#endif
    OFListIterator(OFListIterator_OFString) pos_iter = (mode == FOM_Next) ? OptionPosIterator : OptionPosList.end();
    OFListIterator(OFListIterator_OFString) pos_first = OptionPosList.begin();
    OFListIterator(OFCmdParamPos *) param_iter;
    int diropt = 0;
    if (findParam(abs(pos), param_iter))                               // go to specified parameter position
    {
        diropt = (*param_iter)->DirectOption;                          // number of direct predecessors
        if (((*param_iter)->OptionCount == 0) ||                       // no options in front of specified parameter or
            ((pos < 0) && (diropt == 0)))                              // no 'direct' option ...
                return OFFalse;
        pos_iter = (*param_iter)->OptionIter;                          // first option in front of parameter
        pos_iter++;                                                    // goto next to facilitate loop condition
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


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdSignedInt &value,
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


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdSignedInt &value,
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


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdUnsignedInt &value,
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


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdUnsignedInt &value,
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
        if (sscanf((*ArgumentIterator).c_str(), "%lf", &value) == 1)
            return VS_Normal;
        return VS_Invalid;
    }
    return VS_NoMore;
}


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdFloat &value,
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


OFCommandLine::E_ValueStatus OFCommandLine::getValue(OFCmdFloat &value,
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


const OFCmdOption *OFCommandLine::findCmdOption(const char *option) const
{
    OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
    OFListIterator(OFCmdOption *) last = ValidOptionList.end();
    while (iter != last)
    {
        if (((*iter)->LongOption == option) || ((*iter)->ShortOption == option))
            return *iter;
        iter++;
    }
    return NULL;
}


void OFCommandLine::storeParameter(const char *param,
                                   const int directOpt)
{
    ArgumentList.push_back((OFString)param);
    const OFListIterator(OFListIterator_OFString) iter = (OptionPosList.size() == 0) ? OptionPosList.end() : --OptionPosList.end();
    OFCmdParamPos *parm = new OFCmdParamPos(--ArgumentList.end(), iter, OptionPosList.size(), directOpt);
    if (parm != NULL)
        ParamPosList.push_back(parm);
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
void OFCommandLine::expandWildcards(const char *param,
                                    int directOpt)
{
    if (param != NULL)
    {
        OFString str(param);                                         // create OFString copy
        if ((str.length() >= 2) && (str[0] == '"') && (str[str.length() - 1] == '"'))
            storeParameter(str.substr(1, str.length() - 2).c_str(), directOpt);  // remove quotations
        else
        {
            size_t pos1 = str.find_first_of("*?");                   // search for wildcards
            if (pos1 != OFString_npos)
            {
                OFString name;
                WIN32_FIND_DATA data;
                size_t pos2 = str.find_first_of(":\\", pos1);        // search for next separator (":" or "\")
                HANDLE handle = FindFirstFile(str.substr(0, pos2).c_str(), &data);
                if (handle != INVALID_HANDLE_VALUE)                  // find first file/dir matching the wildcards
                {
                    do {
                        if ((strcmp(data.cFileName, ".") != 0) && (strcmp(data.cFileName, "..") != 0))
                        {                                            // skip "." and ".."
                            size_t pos3 = str.find_last_of(":\\", pos1);
                            if (pos3 != OFString_npos)
                                name = str.substr(0, pos3 + 1) + data.cFileName;
                            else
                                name = data.cFileName;               // no path specified
                            if (pos2 != OFString_npos)
                                name += str.substr(pos2);
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
}
#endif
                    
                    
OFCommandLine::E_ParseStatus OFCommandLine::checkParamCount()
{
    if ((getArgCount() == 1) && findOption("--help"))
        return PS_NoArguments;
    MinParamCount = 0;
    MaxParamCount = 0;
    OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
    OFListIterator(OFCmdParam *) last = ValidParamList.end();
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
        iter++;
    }
    if (getParamCount() < MinParamCount)
        return PS_MissingParameter;
    else if ((MaxParamCount >= 0) && (getParamCount() > MaxParamCount))
        return PS_TooManyParameters;
    return PS_Normal;
}


OFCommandLine::E_ParseStatus OFCommandLine::parseLine(int argCount,
                                                      char *argValue[],
#ifdef HAVE_WINDOWS_H
                                                      const int flags,
#else
                                                      const int /*flags*/,
#endif
                                                      const int startPos)
{
    if (argCount > startPos)                                             // any command line arguments?
    {
        ArgumentList.clear();                                            // initialize lists
        ParamPosList.clear();
        OptionPosList.clear();
        int directOption = 0;                                            // number of direct predecessor
        for (int i = startPos; i < argCount; i++)                        // skip program name (argValue[0])
        {
            if (!checkOption(argValue[i], OFFalse))                      // arg = parameter
            {
#ifdef HAVE_WINDOWS_H
                if (flags & ExpandWildcards)                             // expand wildcards
                    expandWildcards(argValue[i], directOption);
                else
#endif
                    storeParameter(argValue[i], directOption);
                directOption = 0;
            } else {                                                     // arg = option
                const OFCmdOption *opt = findCmdOption(argValue[i]);
                if (opt != NULL)
                {
                    ArgumentList.push_back((OFString)(opt->LongOption)); // convert argument to long format
                    OptionPosList.push_back(--ArgumentList.end());
                    directOption++;
                    int j = opt->ValueCount;
                    if (i + j >= argCount)                               // expecting more values than present
                        return PS_MissingValue;
                    while (j-- > 0)                                      // skip option values
                        ArgumentList.push_back((OFString)argValue[++i]); // add values to argument list
                } else {                                                 // unknown
                    ArgumentList.push_back(argValue[i]);                 // store argument
                    return PS_UnknownOption;
                }
            }
        }
        return checkParamCount();
    }
    return PS_NoArguments;
}


void OFCommandLine::getSyntaxString(OFString &string) const
{
    string = "";
    if (!ValidOptionList.empty())
        string += " [options]";
    if (!ValidParamList.empty())
    {
        OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
        OFListIterator(OFCmdParam *) last = ValidParamList.end();
        while (iter != last)
        {
            if ((*iter)->ParamName.length() > 0)
            {
                switch ((*iter)->ParamMode)
                {
                    case OFCmdParam::PM_Mandatory:
                        string += " ";
                        string += (*iter)->ParamName;
                        break;
                    case OFCmdParam::PM_Optional:
                        string += " [";
                        string += (*iter)->ParamName;
                        string += "]";
                        break;
                    case OFCmdParam::PM_MultiMandatory:
                        string += " ";
                        string += (*iter)->ParamName;
                        string += "...";
                        break;
                    case OFCmdParam::PM_MultiOptional:
                        string += " [";
                        string += (*iter)->ParamName;
                        string += "...]";
                        break;
                }
            }
            iter++;
        }
    }
}


void OFCommandLine::getOptionString(OFString &string) const
{
    string = "";
    if (!ValidOptionList.empty())
    {
        OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
        OFListIterator(OFCmdOption *) last = ValidOptionList.end();
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
            if (newGrp)
            {
                OFListIterator(OFCmdOption *) i = iter;
                while ((i != last) && ((*i)->LongOption.length() > 0))
                {
                    if ((*i)->ShortOption.length() > shortSize)
                        shortSize = (*i)->ShortOption.length();
                    if ((*i)->LongOption.length() > longSize)
                        longSize = (*i)->LongOption.length();
                    i++;
                }
                newGrp = 0;
            }
            if ((*iter)->LongOption.length() <= 0)
            {
                newGrp = 1;
                unpackColumnValues((*iter)->ValueCount, longSize, shortSize);
                if ((*iter)->OptionDescription.length() > 0)                 // new group
                {
                    string += (*iter)->OptionDescription;
                    lineIndent = groupIndent;
                } else {                                                     // new sub group
                    string.append(groupIndent, ' ');
                    string += (*iter)->ValueDescription;
                    lineIndent = subGrpIndent;
                }
                string += "\n";
            } else {
                string.append(lineIndent, ' ');
                if (shortSize > 0)
                {
                    str = (*iter)->ShortOption;
                    str.resize(shortSize, ' ');
                    string += str;
                    string.append(columnSpace, ' ');
                }                    
                str = (*iter)->LongOption;
                str.resize(longSize, ' ');
                string += str;
                string.append(columnSpace, ' ');
                if ((*iter)->ValueDescription.length() > 0)
                {
                    string += (*iter)->ValueDescription;
                    string += "\n";
                    string.append(lineIndent + shortSize + longSize + columnSpace, ' ');
                    if (shortSize > 0)
                        string.append(columnSpace, ' ');
                }
                str = (*iter)->OptionDescription;
                size_t pos = 0;
                while (((pos = (str.find('\n', pos))) != OFString_npos) && (pos < str.length()))
                    str.insert(++pos, OFString(lineIndent + shortSize + longSize + 2 * columnSpace, ' '));
                string += str;
                string += "\n";
            }
            iter++;
        }
    }
}


void OFCommandLine::getParamString(OFString &string) const
{
    string = "";
    if (!ValidParamList.empty())
    {
        OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
        OFListIterator(OFCmdParam *) last = ValidParamList.end();
        OFString str;
        unsigned int columnSize = ParamColumn;
        const unsigned int lineIndent = 2;
        const unsigned int columnSpace = 2;
        while ((iter != last) && ((*iter)->ParamDescription.length() > 0))
        {
            if ((*iter)->ParamName.length() > columnSize)           // determine maximum column width
                columnSize = (*iter)->ParamName.length();
            iter++;
        }
        iter = ValidParamList.begin();                              // reset iterator
        while (iter != last)
        {
            if ((*iter)->ParamDescription.length() > 0)
            {
                if (string.length() == 0)
                    string += "parameters:\n";
                string.append(lineIndent, ' ');
                str = (*iter)->ParamName;
                str.resize(columnSize, ' ');
                string += str;
                string.append(columnSpace, ' ');
                str = (*iter)->ParamDescription;
                size_t pos = 0;
                while (((pos = (str.find('\n', pos))) != OFString_npos) && (pos < str.length()))
                    str.insert(++pos, OFString(lineIndent + columnSize + columnSpace, ' '));
                string += str;
                string += "\n";
            }
            iter++;
        }
    }
}


OFBool OFCommandLine::getMissingParam(OFString &string)
{
    if (!ValidParamList.empty() && (getParamCount() < MinParamCount))
    {
        OFListIterator(OFCmdParam *) iter = ValidParamList.begin();
        OFListIterator(OFCmdParam *) last = ValidParamList.end();
        int i = getParamCount();
        while ((iter != last) && (i-- > 0))
            iter++;
        if (iter != last)
        {
            string = (*iter)->ParamName;
            return OFTrue;
        }
    }
    return OFFalse;
}


void OFCommandLine::getStatusString(const E_ParseStatus status,
                                    OFString &string)
{
    OFString str;
    switch (status)
    {
        case PS_UnknownOption:
            string = "Unknown option ";
            if (getLastArg(str))
                string += str;
            break;
        case PS_MissingValue:
            string = "Missing value for option ";
            if (getLastArg(str))
                string += str;
            break;
        case PS_MissingParameter:
            string = "Missing parameter ";
            if (getMissingParam(str))
                string += str;
            break;
        case PS_TooManyParameters:
            string = "Too many parameters";
            break;
        default:
            string = "";
            break;
    }
}


void OFCommandLine::getStatusString(const E_ParamValueStatus status,
                                    OFString &string)
{
    OFString str;
    switch (status)
    {
        case PVS_Invalid:
            string = "Invalid parameter value ";
            if (getCurrentArg(str))
                string += str;
            break;
        case PVS_CantFind:
            string = "Can't find parameter";
            break;
        case PVS_Underflow:
            string = "Invalid parameter value ";
            if (getCurrentArg(str))
            {
                string += str;
                string += " (underflow)";
            }
            break;
        case PVS_Overflow:
            string = "Invalid parameter value ";
            if (getCurrentArg(str))
            {
                string += str;
                string += " (overflow)";
            }
            break;
        default:
            string = "";
            break;        
    }
}


void OFCommandLine::getStatusString(const E_ValueStatus status,
                                    OFString &string)
{
    OFString str;
    switch (status)
    {
        case VS_Normal:
            string = "";
            break;
        case VS_Invalid:
            string = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                string += str;
                if (getCurrentArg(str))
                {
                    string += " (";
                    string += str;
                    string += ")";
                }
            }
            break;
        case VS_Underflow:
            string = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                string += str;
                if (getCurrentArg(str))
                {
                    string += " (underflow: ";
                    string += str;
                    string += ")";
                }
            }
            break;
        case VS_Overflow:
            string = "Invalid value for option ";
            if (getCurrentOption(str))
            {
                string += str;
                if (getCurrentArg(str))
                {
                    string += " (overflow: ";
                    string += str;
                    string += ")";
                }
            }
            break;
        default:
            string = "";
            break;
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofcmdln.cc,v $
 * Revision 1.20  2000-02-02 10:56:43  joergr
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
