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
 *  Update Date:      $Date: 1999-02-05 14:34:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofcmdln.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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
    ArgumentList(),
    ArgumentIterator(),
    ParamPosList(),
    OptionPosList(),
    OptionPosIterator(),
    OptionBlockIterator(),
    OptionBlockMode(OFFalse),
    OptionChars("+-")
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
  OFListIterator(OFCmdParam *) first_p = ParamPosList.begin();
  OFListIterator(OFCmdParam *) last_p = ParamPosList.end();
  while (first_p != last_p)
  {     
    delete (*first_p);
    first_p = ParamPosList.erase(first_p);
  }
}


void OFCommandLine::setOptionChars(const char *chars)
{
    OptionChars = chars;
}


OFBool OFCommandLine::checkOption(const char *string) const
{
    if ((string != NULL) && (strlen(string) > 0))                                      // empty strings are allowed to support (sub)groups
    {
        if ((strlen(string) < 2) || (OptionChars.find(string[0]) == OFString_npos) ||  // options have to start with one of the defined chars
            (((string[0] == '-') || (string[0] == '+')) &&                             // but when starting with sign character ...
            (string[1] >= '0') && (string[1] <= '9')))                                 // ... don't allow a number as the following character
               return OFFalse;
    }
    return OFTrue;
}


OFBool OFCommandLine::addOption(const char *longOpt,
                                const char *shortOpt,
                                const int valueCount,
                                const char *valueDescr,
                                const char *optDescr)
{
    if (checkOption(longOpt) && checkOption(shortOpt))
    {    
        OFCmdOption *opt = new OFCmdOption(longOpt, shortOpt, valueCount, valueDescr, optDescr);
        if (opt != NULL)
        {
            ValidOptionList.push_back(opt);
            return OFTrue;
        }
    }
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


void OFCommandLine::addGroup(const char *name)
{
    addOption("", "", 0, "", name);
}


void OFCommandLine::addSubGroup(const char *name)
{
    addOption("", "", 0, name, "");
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
    OFListIterator(OFCmdParam *) iter;
    return findParam(pos, iter);
}


OFBool OFCommandLine::findParam(int pos,
                                OFListIterator(OFCmdParam *) &pos_iter)
{
    if ((pos > 0) && (pos <= getParamCount()))
    {
        pos_iter = ParamPosList.begin();
        OFListIterator(OFCmdParam *) pos_last = ParamPosList.end();
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
                                 const int pos,
                                 const int next)
{
    OFListIterator(OFListIterator_OFString) pos_iter = (next > 1) ? OptionPosIterator : OptionPosList.end();
    OFListIterator(OFListIterator_OFString) pos_first = OptionPosList.begin();
    OFListIterator(OFCmdParam *) param_iter;
    if (findParam(pos, param_iter))
    {
        if ((*param_iter)->OptionCount == 0)                           // no options in front of specified parameter
            return OFFalse;
        pos_iter = (*param_iter)->OptionIter;                          // first option in front of parameter
    }
    while (pos_iter != pos_first)
    {
        ArgumentIterator = *(--pos_iter);
        if (OptionBlockMode && (pos_iter == OptionBlockIterator))      // new option is in front of alternative block option
            return OFFalse;
        else if (*ArgumentIterator == longOpt)                         // searched option
        {
            OptionPosIterator = pos_iter;                              // store option position
            if (next == 0)
                OptionBlockIterator = pos_iter;
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


void OFCommandLine::storeParameter(const char *param)
{
    ArgumentList.push_back((OFString)param);
    OFCmdParam *parm = new OFCmdParam(--ArgumentList.end(), OptionPosList.end(), OptionPosList.size());
    if (parm != NULL)
        ParamPosList.push_back(parm);
}

                    
OFCommandLine::E_ParseStatus OFCommandLine::parseLine(int argCount,
                                                      char *argValue[],
                                                      const int flags,
                                                      const int startPos)
{
    if (argCount > startPos)                                             // any command line arguments?
    {
        ArgumentList.clear();                                            // initialize lists
        ParamPosList.clear();
        OptionPosList.clear();
        for (int i = startPos; i < argCount; i++)                        // skip program name (argValue[0])
        {
            if (!checkOption(argValue[i]))                               // arg = parameter
            {
#ifdef HAVE_WINDOWS_H
                if (flags & ExpandWildcards)                             // expand wildcards
                {
                    WIN32_FIND_DATA data;
                    OFString str = "\\\\?\\";                            // necessary to prevent path limitation
                    str += argValue[i];
                    HANDLE handle = FindFirstFile(str.c_str(), &data);   // find first file matching the wildcards
                    if (handle != INVALID_HANDLE_VALUE)
                    {
                        do {
                            storeParameter(data.cFileName);
                        } while (FindNextFile(handle, &data));           // while further files exist ... add them
                        FindClose(handle);
                    } else
                        storeParameter(argValue[i]);                     // parameter contains no wildcards, just add it
                } else
#endif
                    storeParameter(argValue[i]);
            } else {                                                     // arg = option
                const OFCmdOption *opt = findCmdOption(argValue[i]);
                if (opt != NULL)
                {
                    ArgumentList.push_back((OFString)(opt->LongOption)); // convert argument to long format
                    OptionPosList.push_back(--ArgumentList.end());
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
        return PS_Normal;
    }
    return PS_NoArguments;
}


void OFCommandLine::getOptionString(OFString &string) const
{
    string = "";
    if (!ValidOptionList.empty())
    {
        OFListIterator(OFCmdOption *) iter = ValidOptionList.begin();
        OFListIterator(OFCmdOption *) last = ValidOptionList.end();
        OFString str;
        unsigned int shortSize = 0;
        unsigned int longSize = 0;
        unsigned int lineIndent = 0;
        const unsigned int groupIndent = 2;
        const unsigned int subGrpIndent = 4;
        const unsigned int columnSpace = 2;
        while (iter != last)
        {
            if ((shortSize == 0) && (longSize == 0))
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
            }
            if ((*iter)->LongOption.length() <= 0)
            {
                shortSize = 0;
                longSize = 0;
                if ((*iter)->OptionDescription.length() > 0)                  // new group
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
                    string.append(lineIndent + shortSize + longSize + 2, ' ');
                    if (shortSize > 0)
                        string.append(columnSpace, ' ');
                }
                str = (*iter)->OptionDescription;
                size_t pos = 0;
                while (((pos = (str.find('\n', pos))) != OFString_npos) && (pos < str.length()))
                    str.insert(++pos, OFString(lineIndent + shortSize + longSize + 4, ' '));
                string += str;
                string += "\n";
            }
            iter++;
        }
    }
}


void OFCommandLine::getStatusString(const E_ParseStatus status,
                                    OFString &string)
{
    OFString str;
    switch (status)
    {
        case PS_Normal:
            string = "";
            break;
        case PS_NoArguments:
            string = "";
            break;
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
    }
}


void OFCommandLine::getStatusString(const E_ParamValueStatus status,
                                    OFString &string)
{
    OFString str;
    switch (status)
    {
        case PVS_Normal:
            string = "";
            break;
        case PVS_Invalid:
            string = "Invalid parameter value ";
            if (getCurrentArg(str))
                string += str;
            break;
        case PVS_CantFind:
            string = "Can't find parameter";
            break;
        case PVS_Empty:
            string = "";
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
        case VS_NoMore:
            string = "";
            break;
        case VS_Empty:
            string = "";
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
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofcmdln.cc,v $
 * Revision 1.8  1999-02-05 14:34:19  joergr
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
