/*
 *
 *  Copyright (C) 1999-2024, OFFIS e.V.
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
 *  Purpose: Handle console applications (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstring.h"     /* for OFString */

#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"     /* for OFCharacterEncoding */

#include <locale>
#endif // DCMTK_ENABLE_CHARSET_CONVERSION

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


/*------------------*
 *  implementation  *
 *------------------*/

OFConsoleApplication::OFConsoleApplication(const char *app,
                                           const char *desc,
                                           const char *rcsid)
 : Name(OFSTRING_GUARD(app)),
   Description(OFSTRING_GUARD(desc)),
   Identification(OFSTRING_GUARD(rcsid)),
   QuietMode(OFFalse),
   CmdLine(NULL)
{
}


OFConsoleApplication::~OFConsoleApplication()
{
}


OFBool OFConsoleApplication::checkParseStatus(const OFCommandLine::E_ParseStatus status)
{
    OFBool result = OFFalse;
    switch (status)
    {
        case OFCommandLine::PS_NoArguments:
            /* check whether to print the "usage text" or not */
            if ((CmdLine != NULL) && (CmdLine->getMinParamCount() > 0))
                printUsage();
            else
                result = OFTrue;
            break;
        case OFCommandLine::PS_ExclusiveOption:
            /* check whether to print the "usage text" or not */
            if ((CmdLine != NULL) && (CmdLine->findOption("--help")))
                printUsage();
            else
                result = OFTrue;
            break;
        case OFCommandLine::PS_Normal:
            result = OFTrue;
            break;
        default:
            /* an error occurred while parsing the command line */
            if (CmdLine != NULL)
            {
                OFString str;
                CmdLine->getStatusString(status, str);
                printError(str.c_str());
            }
            break;
    }
    return result;
}


OFBool OFConsoleApplication::parseCommandLine(OFCommandLine &cmd,
                                              int argCount,
                                              char *argValue[],
                                              const int flags,
                                              const int startPos)
{
    /* store reference to given command line object */
    CmdLine = &cmd;
    /* parse command line and check status */
    return checkParseStatus(cmd.parseLine(argCount, argValue, flags, startPos));
}


#ifdef DCMTK_USE_WCHAR_T

OFBool OFConsoleApplication::parseCommandLine(OFCommandLine &cmd,
                                              int argCount,
                                              wchar_t *argValue[],
                                              const int flags,
                                              const int startPos)
{
    /* store reference to given command line object */
    CmdLine = &cmd;
    /* parse command line and check status */
    return checkParseStatus(cmd.parseLine(argCount, argValue, flags, startPos));
}

#endif // DCMTK_USE_WCHAR_T


void OFConsoleApplication::printHeader(const OFBool hostInfo,
                                       const OFBool stdError)
{
    /* lock output stream */
    STD_NAMESPACE ostream *output = (stdError) ? &ofConsole.lockCerr() : &ofConsole.lockCout();
    if (!Identification.empty())
        (*output) << Identification << OFendl << OFendl;
    (*output) << Name;
    if (!Description.empty())
        (*output) << ": " << Description;
    (*output) << OFendl;
    /* print optional host information */
    if (hostInfo)
    {
        (*output) << OFendl << "Host type: " << CANONICAL_HOST_TYPE << OFendl;
#if defined(DCMTK_ENABLE_CHARSET_CONVERSION) && defined(HAVE_LOCALE_H)
        /* determine system's current locale */
        const char *currentLocale = setlocale(LC_CTYPE, NULL);
        if (setlocale(LC_CTYPE, "") != NULL)
        {
            OFString encoding = OFCharacterEncoding::getLocaleEncoding();
            (*output) << "Character encoding: ";
            if (!encoding.empty())
                (*output) << encoding << OFendl;
            else
                (*output) << "system default (unknown)" << OFendl;
            /* reset locale to the previous setting or to the default (7-bit ASCII) */
            if (currentLocale != NULL)
                setlocale(LC_CTYPE, currentLocale);
            else
                setlocale(LC_CTYPE, "C");
        }
#endif
#ifdef HAVE_WINDOWS_H
        if ((CmdLine != NULL) && CmdLine->getWideCharMode())
        {
            (*output) << "Character encoding: Unicode (UTF-16)" << OFendl;
        } else {
            /* determine system's current code pages */
            (*output) << "Code page: " << GetOEMCP() << " (OEM) / " << GetACP() << " (ANSI)" << OFendl;
        }
#endif
        /* output details on DCMTK's build options */
        (*output) << "Build options:";
#ifdef DEBUG
        /* indicate that debug code is present */
        (*output) << " debug";
#endif
#ifdef DCMTK_SHARED
        /* indicate that shared library support is enabled */
        (*output) << " shared";
#endif
#ifdef HAVE_CXX11
        /* indicate that C++11 is used */
        (*output) << " cxx11";
#endif
#if defined(HAVE_STL_ALGORITHM) && defined(HAVE_STL_LIMITS) && defined(HAVE_STL_LIST) && \
    defined(HAVE_STL_MAP) && defined(HAVE_STL_MEMORY) && defined(HAVE_STL_STACK) && \
    defined(HAVE_STL_STRING) && defined(HAVE_STL_SYSTEM_ERROR) && defined(HAVE_STL_TUPLE) && \
    defined(HAVE_STL_TYPE_TRAITS) && defined(HAVE_STL_VECTOR)
        /* indicate that the C++ STL is used */
        (*output) << " stl";
#elif defined(HAVE_STL_ALGORITHM) || defined(HAVE_STL_LIMITS) || defined(HAVE_STL_LIST) || \
    defined(HAVE_STL_MAP) || defined(HAVE_STL_MEMORY) || defined(HAVE_STL_STACK) || \
    defined(HAVE_STL_STRING) || defined(HAVE_STL_SYSTEM_ERROR) || defined(HAVE_STL_TUPLE) || \
    defined(HAVE_STL_TYPE_TRAITS) || defined(HAVE_STL_VECTOR)
        /* indicate that parts of the C++ STL are used */
        (*output) << " (stl)";
#endif
#ifdef WITH_THREADS
        /* indicate that MT support is enabled */
        (*output) << " threads";
#endif
#ifdef DCMTK_ENABLE_LFS
        /* indicate that LFS support is enabled */
        (*output) << " lfs";
#endif
#if DCM_DICT_DEFAULT == 1
        /* indicate that the builtin data dictionary is enabled */
        (*output) << " builtin-dict";
#elif DCM_DICT_DEFAULT == 2
        /* indicate that the external data dictionary is enabled */
        (*output) << " extern-dict";
#endif
#ifdef DCM_DICT_USE_DCMDICTPATH
        /* indicate that the DCMDICTPATH environment variable is checked */
        (*output) << " dcmdictpath";
#elif DCM_DICT_DEFAULT == 0
        /* indicate that no data dictionary is available */
        (*output) << " no-dict";
#endif
#ifdef ENABLE_PRIVATE_TAGS
        /* indicate that private tag dictionary is enabled */
        (*output) << " private-tags";
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
        /* indicate that character set conversion is enabled */
        (*output) << " char-conv";
#endif
        (*output) << OFendl;
    }
    /* release output stream */
    if (stdError)
        ofConsole.unlockCerr();
    else
        ofConsole.unlockCout();
}


void OFConsoleApplication::printUsage(const OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    printHeader();
    STD_NAMESPACE ostream &output = ofConsole.lockCout();
    output << "usage: " << Name;
    if (cmd != NULL)
    {
        OFString str;
        cmd->getSyntaxString(str);
        output << str << OFendl;
        cmd->getParamString(str);
        if (!str.empty())
            output << OFendl << str;
        cmd->getOptionString(str);
        if (!str.empty())
            output << OFendl << str;
    }
    output << OFendl;
    ofConsole.unlockCout();
    /* exit code: no error */
    exit(EXITCODE_NO_ERROR);
}


void OFConsoleApplication::printArguments(OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    STD_NAMESPACE ostream &output = ofConsole.lockCerr();
    if (CmdLine != NULL)
    {
        output << "expanded command line to " << CmdLine->getArgCount() << " arguments:" << OFendl;
        const char *arg;
        /* iterate over all command line arguments */
        if (CmdLine->gotoFirstArg())
        {
            do {
                if (CmdLine->getCurrentArg(arg))
                    output << "'" << arg << "' ";
            } while (CmdLine->gotoNextArg());
        }
        output << OFendl << OFendl;
    } else
        output << "warning: cannot print expanded command line arguments" << OFendl << OFendl;
    ofConsole.unlockCerr();
}


void OFConsoleApplication::printIdentifier()
{
    if (!Identification.empty())
    {
        ofConsole.lockCerr() << Identification << OFendl << OFendl;
        ofConsole.unlockCerr();
    }
}


void OFConsoleApplication::printError(const char *str,
                                      const int code)
{
    if (!QuietMode)
    {
        printHeader(OFFalse /*hostInfo*/, OFTrue /*stdError*/);
        ofConsole.lockCerr() << "error: " << str << OFendl;
        ofConsole.unlockCerr();
    }
    exit(code);
}


void OFConsoleApplication::printWarning(const char *str,
                                        const char *prefix)
{
    if (!QuietMode)
    {
        ofConsole.lockCerr() << Name << ": ";
        if ((prefix != NULL) && (prefix[0] != '\0'))
            ofConsole.getCerr() << prefix << ": ";
        ofConsole.getCerr() << str << OFendl;
        ofConsole.unlockCerr();
    }
}


void OFConsoleApplication::printMessage(const char *str)
{
    if (!QuietMode)
    {
        ofConsole.lockCerr() << str << OFendl;
        ofConsole.unlockCerr();
    }
}


OFBool OFConsoleApplication::quietMode() const
{
    return QuietMode;
}


void OFConsoleApplication::setQuietMode(const OFBool mode)
{
    QuietMode = mode;
}


void OFConsoleApplication::checkValue(const OFCommandLine::E_ValueStatus status,
                                      OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    if (status != OFCommandLine::VS_Normal)
    {
        OFString str;
        if (cmd != NULL)
            cmd->getStatusString(status, str);
        if (!str.empty())
            printError(str.c_str());
    }
}


void OFConsoleApplication::checkParam(const OFCommandLine::E_ParamValueStatus status,
                                      OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    if (status != OFCommandLine::PVS_Normal)
    {
        OFString str;
        if (cmd != NULL)
            cmd->getStatusString(status, str);
        if (!str.empty())
            printError(str.c_str());
    }
}


void OFConsoleApplication::checkDependence(const char *subOpt,
                                           const char *baseOpt,
                                           OFBool condition)
{
    if (!condition)
    {
        OFString str = subOpt;
        str += " only allowed with ";
        str += baseOpt;
        printError(str.c_str());
    }
}


void OFConsoleApplication::checkConflict(const char *firstOpt,
                                         const char *secondOpt,
                                         OFBool condition)
{
    if (condition)
    {
        OFString str = firstOpt;
        str += " not allowed with ";
        str += secondOpt;
        printError(str.c_str());
    }
}
