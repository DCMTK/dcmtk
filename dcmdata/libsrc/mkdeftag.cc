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
 *  Purpose: Generate a C++ header defining symbolic names for DICOM Tags.
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

static char*
convertToIdentifier(char* s)
{
    if (s == NULL) {
        return s;
    }
    int len = strlen(s);
    int i=0;
    unsigned char c;

    for (i=0; i<len; i++) {
        c = OFstatic_cast(unsigned char, s[i]);
        if (!(isalnum(c) || (c == '_'))) {
            /* replace char with '_' */
            s[i] = '_';
        }
    }
    return s;
}

static char
rangeRestriction2char(DcmDictRangeRestriction r)
{
    char c = ' ';
    if (r == DcmDictRange_Odd) {
        c = 'o';
    } else if (r == DcmDictRange_Even) {
        c = 'e';
    } else if (r == DcmDictRange_Unspecified) {
        c = 'u';
    }
    return c;
}

static void
printDefined(FILE* fout, const DcmDictEntry* e)
{
    int i;
    char buf[DCM_MAXDICTLINESIZE+1];
    const char* tagPrefix;

    if (e == NULL || e->getTagName() == NULL)
        return;

    strcpy(buf, e->getTagName());

    convertToIdentifier(buf);

    tagPrefix = "#define DCM_";
    fputs(tagPrefix, fout); /* write out prefix */
    fputs(buf, fout); /* write out rest of name */

    int n = 48 - (strlen(tagPrefix) + strlen(buf));
    for (i=0; i<n; i++) {
            putc(' ', fout);
    }

    fprintf(fout, " DcmTagKey(0x%04x, 0x%04x)",
            e->getGroup(), e->getElement());

    if (e->isRepeating()) {
        fprintf(fout, " /* (0x%04x", e->getGroup());
        if (e->isRepeatingGroup()) {
            if (e->getGroupRangeRestriction() != DcmDictRange_Even) {
                fprintf(fout, "-%c",
                        rangeRestriction2char(e->getGroupRangeRestriction()));
            }
            fprintf(fout, "-0x%04x", e->getUpperGroup());
        }
        fprintf(fout, ",0x%04x", e->getElement());
        if (e->isRepeatingElement()) {
            if (e->getElementRangeRestriction() != DcmDictRange_Even) {
                fprintf(fout, "-%c",
                        rangeRestriction2char(e->getElementRangeRestriction()));
            }
            fprintf(fout, "-0x%04x", e->getUpperElement());
        }
        fprintf(fout, ") */");
    }
    fputs("\n", fout);
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
    s = getlogin(); // thread unsafe?
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

int main(int argc, char* argv[])
{
    const char* progname = "mkdeftag";
    const char* filename = NULL;
    DcmDictEntry* e = NULL;
    int i = 0;
    FILE* fout = NULL;

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

    prepareCmdLineArgs(argc, argv, "mkdeftag");

    DcmDataDictionary& globalDataDict = dcmDataDict.wrlock();

    /* clear out global data dictionary */
    globalDataDict.clear();

    progname = argv[0];

    if (argc >= 3 && 0 == strcmp(argv[1], "-o")) {
        filename = argv[2];
        argv += 2;
        argc -= 2;
    }

    for (i=1; i<argc; i++) {
        globalDataDict.loadDictionary(argv[i]);
    }

    if (filename) {
        fout = fopen(filename, "w");
        if (!fout) {
            fprintf(stderr, "Failed to open file \"%s\", giving up\n", filename);
            return -1;
        }
    } else {
        fout = stdout;
    }

    OFString dateString;
    OFDateTime::getCurrentDateTime().getISOFormattedDateTime(dateString);

    fputs("/*\n", fout);
    fputs("** DO NOT EDIT THIS FILE !!!\n", fout);
    fputs("** It was generated automatically by:\n", fout);
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
    fputs("*/\n\n", fout);
    fputs("#ifndef DCDEFTAG_H\n", fout);
    fputs("#define DCDEFTAG_H\n\n", fout);
    fputs("#include \"dcmtk/dcmdata/dctagkey.h\"\n\n", fout);
#ifndef SUPPRESS_CREATE_STAMP
    fprintf(fout, "#define DCM_DICT_DEFTAG_BUILD_DATE \"%s\"\n", dateString.c_str());
    fputs("\n", fout);
#endif

    /*
    ** the hash table does not maintain ordering so we must put
    ** all the entries into a sorted list.
    */
    DcmDictEntryList list;
    DcmHashDictIterator iter(globalDataDict.normalBegin());
    DcmHashDictIterator end(globalDataDict.normalEnd());
    for (; iter != end; ++iter)
    {
        if ((*iter)->getPrivateCreator() == NULL) // exclude private tags
        {
          e = new DcmDictEntry(*(*iter));
          list.insertAndReplace(e);
        }
    }

    /* generate the entries themselves */
    fputs("\n/*\n", fout);
    fputs("** Fixed Tags in ascending (gggg,eeee) order.\n", fout);
    fprintf(fout, "** Number of entries: %lu\n", OFstatic_cast(unsigned long, list.size()));
    fputs("** Tags with a repeating component (repeating tags) are listed later.\n", fout);
    fputs("*/\n", fout);

    /* output the list contents */
    DcmDictEntryListIterator listIter(list.begin());
    DcmDictEntryListIterator listLast(list.end());
    for (; listIter != listLast; ++listIter)
    {
        printDefined(fout, *listIter);
    }

    // count non-private repeating tags in dictionary
    int repCount = 0;
    DcmDictEntryListIterator repIter(globalDataDict.repeatingBegin());
    DcmDictEntryListIterator repLast(globalDataDict.repeatingEnd());
    for (; repIter != repLast; ++repIter)
    {
        if ((*repIter)->getPrivateCreator() == NULL) ++repCount;
    }

    fputs("\n/*\n", fout);
    fputs("** Tags where the group/element can vary (repeating tags).\n",
          fout);
    fprintf(fout, "** Number of entries: %d\n", repCount);
    fputs("*/\n", fout);

    repIter = globalDataDict.repeatingBegin();
    for (; repIter != repLast; ++repIter)
    {
        if ((*repIter)->getPrivateCreator() == NULL) // exclude private tags
        {
            printDefined(fout, *repIter);
        }
    }
    fputs("\n#endif /* !DCDEFTAG_H */\n", fout);

    dcmDataDict.unlock();
    return 0;
}
