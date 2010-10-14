/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *  Generate a builtin data dictionary which can be compiled into
 *  the dcmdata library.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:11 $
 *  CVS/RCS Revision: $Revision: 1.34 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#define INCLUDE_LIBC
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>  /* this includes either winsock.h or winsock2.h */
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>  /* include winsock.h directly i.e. on MacOS */
#endif
#endif

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dcdicent.h"

#define PRIVATE_TAGS_IFNAME "WITH_PRIVATE_TAGS"

static const char*
rr2s(DcmDictRangeRestriction rr)
{
    const char* s;
    switch (rr) {
    case DcmDictRange_Unspecified:
        s = "DcmDictRange_Unspecified";
        break;
    case DcmDictRange_Odd:
        s = "DcmDictRange_Odd";
        break;
    case DcmDictRange_Even:
        s = "DcmDictRange_Even";
        break;
    default:
        s = "DcmDictRange_GENERATOR_ERROR";
        break;
    }
    return s;
}

static void
printSimpleEntry(FILE* fout, const DcmDictEntry* e, OFBool& isFirst, OFBool& isPrivate)
{

    const char *c = e->getPrivateCreator();

    if (c && !isPrivate)
    {
       fprintf(fout, "#ifdef %s\n", PRIVATE_TAGS_IFNAME);
       isPrivate = OFTrue;
    }
    else if (isPrivate && !c)
    {
       fprintf(fout, "#endif\n");
       isPrivate = OFFalse;
    }

    if (isFirst)
    {
      fprintf(fout, "    ");
      isFirst = OFFalse;
    } else fprintf(fout, "  , ");

    fprintf(fout, "{ 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n",
            e->getGroup(), e->getElement(),
            e->getUpperGroup(), e->getUpperElement());
    fprintf(fout, "      EVR_%s, \"%s\", %d, %d, \"%s\",\n",
            e->getVR().getVRName(),
            e->getTagName(),
            e->getVMMin(), e->getVMMax(),
            OFSTRING_GUARD(e->getStandardVersion()));
    fprintf(fout, "      %s, %s,\n", rr2s(e->getGroupRangeRestriction()),
            rr2s(e->getElementRangeRestriction()));

    if (c)
      fprintf(fout, "      \"%s\" }\n", c);
    else
      fprintf(fout, "      NULL }\n");
}

#ifdef HAVE_CUSERID
static char*
getUserName(char* userString, int /* maxLen */)
{
    return cuserid(userString); // thread safe, maxLen >= L_cuserid ?
}
#elif HAVE_GETLOGIN
static char*
getUserName(char* userString, int maxLen)
{
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
    // use getlogin_r instead of getlogin
    if (getlogin_r(userString, maxLen) != 0)
        strncpy(userString, "<no-utmp-entry>", maxLen);
    return userString;
#else
    char* s;
    s = getlogin(); // thread unsafe
    if (s == NULL) s = "<no-utmp-entry>";
    return strncpy(userString, s, maxLen);
#endif
}
#elif defined(_WIN32)

#include <lm.h>

static char*
getUserName(char* userString, int maxLen)
{
    WKSTA_USER_INFO_0 *userinfo;
    if (NetWkstaUserGetInfo(NULL, 0, (LPBYTE*)&userinfo) == NERR_Success)
    {
        // Convert the Unicode full name to ANSI.
        WideCharToMultiByte( CP_ACP, 0, (WCHAR*)userinfo->wkui0_username, -1,
            userString, maxLen, NULL, NULL );
    } else {
        strncpy(userString, "<no-user-information-available>", maxLen);
    }
    return userString;
}
#else
static char*
getUserName(char* userString, int maxLen)
{
    return strncpy(userString, "<unknown-user>", maxLen);
}
#endif

#ifdef HAVE_UNAME
static char*
getHostName(char* hostString, int maxLen)
{
    struct utsname n;
    uname(&n);
    strncpy(hostString, n.nodename, maxLen);
    return hostString;
}
#elif HAVE_GETHOSTNAME
static char*
getHostName(char* userString, int maxLen)
{
    gethostname(userString, maxLen);
    return userString;
}
#else
static char*
getHostName(char* hostString, int maxLen)
{
    return strncpy(hostString, "localhost", maxLen);
}
#endif

int
main(int argc, char* argv[])
{
    char* progname;
    FILE* fout = NULL;
    DcmDictEntry* e = NULL;

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    prepareCmdLineArgs(argc, argv, "mkdictbi");

    DcmDataDictionary& globalDataDict = dcmDataDict.wrlock();

    /* clear out any preloaded dictionary */
    globalDataDict.clear();

    progname = argv[0];
    int i;
    for (i=1; i<argc; i++) {
        globalDataDict.loadDictionary(argv[i]);
    }

    fout = stdout;

    OFString dateString;
    OFDateTime::getCurrentDateTime().getISOFormattedDateTime(dateString);

    /* generate c++ code for static dictionary */

    fprintf(fout, "/*\n");
    fprintf(fout, "** DO NOT EDIT THIS FILE !!!\n");
    fprintf(fout, "** It was generated automatically by:\n");
#ifndef SUPPRESS_CREATE_STAMP
    /*
     * Putting the date in the file will confuse CVS/RCS
     * if nothing has changed except the generation date.
     * This is only an issue if the header file is continually
     * generated new.
     */
    fputs("**\n", fout);
    char tmpString[512];
    getUserName(tmpString, 512);
    fprintf(fout, "**   User: %s\n", tmpString);
    getHostName(tmpString, 512);
    fprintf(fout, "**   Host: %s\n", tmpString);
    fprintf(fout, "**   Date: %s\n", dateString.c_str());
#endif
    fprintf(fout, "**   Prog: %s\n", progname);
    fputs("**\n", fout);
    if (argc > 1) {
        fprintf(fout, "**   From: %s\n", argv[1]);
        for (i=2; i<argc; i++) {
            fprintf(fout, "**         %s\n", argv[i]);
        }
    }
    fputs("**\n", fout);
    fprintf(fout, "*/\n");
    fprintf(fout, "\n");
    fprintf(fout, "#include \"dcmtk/dcmdata/dcdict.h\"\n");
    fprintf(fout, "#include \"dcmtk/dcmdata/dcdicent.h\"\n");
    fprintf(fout, "\n");
    fprintf(fout, "const char* dcmBuiltinDictBuildDate = \"%s\";\n", dateString.c_str());
    fprintf(fout, "\n");
    fprintf(fout, "struct DBI_SimpleEntry {\n");
    fprintf(fout, "    Uint16 group;\n");
    fprintf(fout, "    Uint16 element;\n");
    fprintf(fout, "    Uint16 upperGroup;\n");
    fprintf(fout, "    Uint16 upperElement;\n");
    fprintf(fout, "    DcmEVR evr;\n");
    fprintf(fout, "    const char* tagName;\n");
    fprintf(fout, "    int vmMin;\n");
    fprintf(fout, "    int vmMax;\n");
    fprintf(fout, "    const char* standardVersion;\n");
    fprintf(fout, "    DcmDictRangeRestriction groupRestriction;\n");
    fprintf(fout, "    DcmDictRangeRestriction elementRestriction;\n");
    fprintf(fout, "    const char* privateCreator;\n");
    fprintf(fout, "};\n");
    fprintf(fout, "\n");
    fprintf(fout, "static const DBI_SimpleEntry simpleBuiltinDict[] = {\n");

    OFBool isFirst = OFTrue;
    OFBool isPrivate = OFFalse;

    /*
    ** the hash table does not maintain ordering so we must put
    ** all the entries into a sorted list.
    */
    DcmDictEntryList list;
    DcmHashDictIterator iter(globalDataDict.normalBegin());
    DcmHashDictIterator last(globalDataDict.normalEnd());
    for (; iter != last; ++iter) {
        e = new DcmDictEntry(*(*iter));
        list.insertAndReplace(e);
    }
    /* output the list contents */

    /* non-repeating standard elements */
    DcmDictEntryListIterator listIter(list.begin());
    DcmDictEntryListIterator listLast(list.end());
    for (; listIter != listLast; ++listIter)
    {
        printSimpleEntry(fout, *listIter, isFirst, isPrivate);
    }

    /* repeating standard elements */
    DcmDictEntryListIterator repIter(globalDataDict.repeatingBegin());
    DcmDictEntryListIterator repLast(globalDataDict.repeatingEnd());
    for (; repIter != repLast; ++repIter)
    {
        printSimpleEntry(fout, *repIter, isFirst, isPrivate);
    }

    if (isPrivate)
    {
       fprintf(fout, "#endif\n");
    }

    fprintf(fout, "\n};\n");
    fprintf(fout, "\n");
    fprintf(fout, "static const int simpleBuiltinDict_count = \n");
    fprintf(fout, "    sizeof(simpleBuiltinDict)/sizeof(DBI_SimpleEntry);\n");
    fprintf(fout, "\n");

    fprintf(fout, "\n");
    fprintf(fout, "void\n");
    fprintf(fout, "DcmDataDictionary::loadBuiltinDictionary()\n");
    fprintf(fout, "{\n");
    fprintf(fout, "    DcmDictEntry* e = NULL;\n");
    fprintf(fout, "    const DBI_SimpleEntry *b = simpleBuiltinDict;\n");
    fprintf(fout, "    for (int i=0; i<simpleBuiltinDict_count; i++) {\n");
    fprintf(fout, "        b = simpleBuiltinDict + i;\n");
    fprintf(fout, "        e = new DcmDictEntry(b->group, b->element,\n");
    fprintf(fout, "            b->upperGroup, b->upperElement, b->evr,\n");
    fprintf(fout, "            b->tagName, b->vmMin, b->vmMax,\n");
    fprintf(fout, "            b->standardVersion, OFFalse, b->privateCreator);\n");
    fprintf(fout, "        e->setGroupRangeRestriction(b->groupRestriction);\n");
    fprintf(fout, "        e->setElementRangeRestriction(b->elementRestriction);\n");
    fprintf(fout, "        addEntry(e);\n");
    fprintf(fout, "    }\n");
    fprintf(fout, "}\n");
    fprintf(fout, "\n");
    fprintf(fout, "\n");

    dcmDataDict.unlock();
    return 0;
}

/*
** CVS/RCS Log:
** $Log: mkdictbi.cc,v $
** Revision 1.34  2010-10-14 13:14:11  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.33  2010-08-10 11:59:31  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.32  2010-07-02 07:12:29  joergr
** Fixed typo: Changed "SUPRESS_CREATE_STAMP" to "SUPPRESS_CREATE_STAMP".
**
** Revision 1.31  2010-03-03 08:50:30  joergr
** Fixed compilation issue with call to getlogin_r().
**
** Revision 1.30  2009-11-04 09:58:11  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.29  2009-01-15 14:30:26  joergr
** Changed formatting of comment header.
**
** Revision 1.28  2006/08/02 15:20:42  meichel
** Fixed bugs that prevented compiling the affected tools under MinGW
**
** Revision 1.27  2005/12/09 15:04:37  meichel
** Updated build system for dcdeftag/dcdictzz to reflect new directory structure
**
** Revision 1.26  2005/12/08 15:42:13  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.25  2004/08/03 11:41:10  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.24  2002/11/27 12:07:02  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.23  2002/08/02 09:47:33  meichel
** Fixed bug in mkdictbi which would cause the builtin dictionary to fail
**   to compile if the last dictionary entry was a private tag.
**
** Revision 1.22  2002/07/23 14:21:35  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.21  2002/04/11 12:31:04  joergr
** Added support for MT-safe system routines (cuserid, getlogin, etc.).
** Replaced direct call of system routines by new standard date and time
** functions.
**
** Revision 1.20  2001/06/01 15:49:25  meichel
** Updated copyright header
**
** Revision 1.19  2000/05/03 14:19:10  meichel
** Added new class GlobalDcmDataDictionary which implements read/write lock
**   semantics for safe access to the DICOM dictionary from multiple threads
**   in parallel. The global dcmDataDict now uses this class.
**
** Revision 1.18  2000/04/14 16:17:21  meichel
** Minor changes for thread safety.
**
** Revision 1.17  2000/03/08 16:26:55  meichel
** Updated copyright header.
**
** Revision 1.16  2000/02/23 15:12:09  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.15  2000/02/01 10:12:12  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.14  1999/04/21 13:02:34  meichel
** Now always including <windows.h> instead of <winsock.h> on Win32 platforms.
**   This makes sure that <winsock2.h> is used if available.
**
** Revision 1.13  1999/03/31 09:26:09  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1999/03/22 15:45:48  meichel
** Implemented getUserName() on Win32 using the NetWkstaUserGetInfo() API.
**
** Revision 1.11  1998/07/15 14:09:52  joergr
** Including of <libc.h> to support getlogin() on NeXTSTEP.
** Replaced tabs by spaces.
**
** Revision 1.10  1997/08/26 14:03:20  hewett
** New data structures for data-dictionary.  The main part of the
** data-dictionary is now stored in an hash table using an optimized
** hash function.  This new data structure reduces data-dictionary
** load times by a factor of 4!  he data-dictionary specific linked-list
** has been replaced by a linked list derived from OFList class
** (see ofstd/include/oflist.h).
** The only interface modifications are related to iterating over the entire
** data dictionary which should not be needed by "normal" applications.
**
** Revision 1.9  1997/07/21 08:25:37  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/06/26 12:59:16  andreas
** - Include Additional headers (winsock.h, io.h) for Windows NT/95
**
** Revision 1.7  1997/05/13 13:44:30  hewett
** Fixed spelling in generated comments.
**
** Revision 1.6  1997/03/26 17:14:04  hewett
** Now uses proper enumerated type names for VR instead of numbers.
**
** Revision 1.5  1996/09/24 16:37:43  hewett
** Now correctly includes cmdlnarg.h
**
** Revision 1.4  1996/09/24 16:26:28  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.3  1996/03/20 16:44:07  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
** Revision 1.2  1996/03/12 15:21:24  hewett
** The repeating sub-dictionary has been split into a repeatingElement and
** a repeatingGroups dictionary.  This is a temporary measure to reduce the
** problem of overlapping dictionary entries.  A full solution will require
** more radical changes to the data dictionary insertion and search
** mechanims.
**
** Revision 1.1  1995/11/23 17:03:16  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
*/

