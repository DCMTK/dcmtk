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

int
main(int argc, char* argv[])
{
    char* progname;
    const char* filename = NULL;
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

    if (argc >= 3 && 0 == strcmp(argv[1], "-o")) {
        filename = argv[2];
        argv += 2;
        argc -= 2;
    }

    int i;
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
    if (filename)
    {
        fclose(fout);
    }
    return 0;
}
