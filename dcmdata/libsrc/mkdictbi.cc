/*
**
** Author: Andrew Hewett	Created: 4.11.95
**
** Program: mkdictbi.cc
**
** Purpose:
** Generate a builtin data dictionary which can be compiled into
** the dcmdata library.  
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-09-24 16:26:28 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/mkdictbi.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcdict.h"

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
printSimpleEntry(FILE* fout, const DcmDictEntry* e, int lastEntry)
{
    fprintf(fout, "    { 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n",
	    e->getGroup(), e->getElement(), 
	    e->getUpperGroup(), e->getUpperElement());
    fprintf(fout, "      ((DcmEVR)(%d)) /*%s*/, \"%s\", %d, %d, \"%s\",\n",
	    int(e->getVR().getEVR()), e->getVR().getVRName(),
	    e->getTagName(), 
	    e->getVMMin(), e->getVMMax(),
	    e->getStandardVersion());
    fprintf(fout, "      %s, %s }", rr2s(e->getGroupRangeRestriction()),
	    rr2s(e->getElementRangeRestriction()));
    if (!lastEntry) {
    	fprintf(fout, ",\n");
    } else {
    	fprintf(fout, "\n");
    } 
}

static char*
stripTrailing(char* s, char c)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; (i >= 0) && (s[i] == c); i--)
        s[i] = '\0';
    return s;
}

static char*
getDateString(char* dateString, int maxLen)
{
    time_t systime = time(NULL);
    const char *ct = ctime(&systime);
    strncpy(dateString, ct, maxLen);
    stripTrailing(dateString, '\n');
    return dateString;
}


#ifdef HAVE_CUSERID
static char*
getUserName(char* userString, int maxLen)
{
    char* s;
    s = cuserid(NULL);
    return strncpy(userString, s, maxLen);
}
#elif HAVE_GETLOGIN
static char*
getUserName(char* userString, int maxLen)
{
    char* s;
    s = getlogin();
    if (s == NULL) s = "<no-utmp-entry>";
    return strncpy(userString, s, maxLen);
}
#else
static char*
getUserName(char* userString, int maxLen)
{
    char* s = "<unknown-user>";
    return strncpy(userString, s, maxLen);
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
    const DcmDictEntry* e = NULL;

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    prepareCmdLineArgs(argc, argv, "mkdictbi");

    /* clear out any preloaded dictionary */
    dcmDataDict.clear();

    progname = argv[0];
    int i;
    for (i=1; i<argc; i++) { 
        dcmDataDict.loadDictionary(argv[i]);
    }
    
    fout = stdout;

    char dateString[512];
    getDateString(dateString, 512);

    /* generate c++ code for static dictionary */

    fprintf(fout, "/*\n");
    fprintf(fout, "** DO NOT EDIT\n");
    fprintf(fout, "** This file is generated automatically by:\n");
#ifndef SUPRESS_CREATE_STAMP
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
    fprintf(fout, "**   Date: %s\n", dateString);
#endif
    fprintf(fout, "**   Prog: %s\n", progname);
    fputs("**\n", fout);
    if (argc > 1) {
	fprintf(fout, "** From: %s\n", argv[1]);
	for (i=2; i<argc; i++) {
	    fprintf(fout, "**       %s\n", argv[i]);
	}
    }
    fputs("**\n", fout);
    fprintf(fout, "*/\n");
    fprintf(fout, "\n");
    fprintf(fout, "#include \"dcdict.h\"\n");
    fprintf(fout, "\n");
    fprintf(fout, "char* dcmBuiltinDictBuildDate = \"%s\";\n", dateString);
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
    fprintf(fout, "};\n");
    fprintf(fout, "\n");
    fprintf(fout, "static DBI_SimpleEntry simpleBuiltinDict[] = {\n");
    
    Pix p = NULL;
    Pix pp = NULL;
    int lastEntry = FALSE;
    for (p = dcmDataDict.normalFirst(); p != 0; dcmDataDict.normalNext(p)) {
        e = dcmDataDict.normalContents(p);
        printSimpleEntry(fout, e, lastEntry);
    }

    for (p = dcmDataDict.repeatingFirst(); p != 0; 
	 dcmDataDict.repeatingNext(p)) {
        e = dcmDataDict.repeatingContents(p);
        pp = p;
        dcmDataDict.repeatingNext(pp);
        if (pp == 0) {
            lastEntry = TRUE;
        }
        printSimpleEntry(fout, e, lastEntry);
    }

    fprintf(fout, "};\n");
    fprintf(fout, "\n");
    fprintf(fout, "static int simpleBuiltinDict_count = \n");
    fprintf(fout, "    sizeof(simpleBuiltinDict)/sizeof(DBI_SimpleEntry);\n");
    fprintf(fout, "\n");

    fprintf(fout, "\n");
    fprintf(fout, "void\n");
    fprintf(fout, "DcmDataDictionary::loadBuiltinDictionary()\n");
    fprintf(fout, "{\n");
    fprintf(fout, "    DcmDictEntry* e = NULL;\n");
    fprintf(fout, "    DBI_SimpleEntry* b = simpleBuiltinDict;\n");
    fprintf(fout, "    for (int i=0; i<simpleBuiltinDict_count; i++) {\n");
    fprintf(fout, "        b = simpleBuiltinDict + i;\n");
    fprintf(fout, "        e = new DcmDictEntry(b->group, b->element,\n");
    fprintf(fout, "            b->upperGroup, b->upperElement, b->evr,\n");
    fprintf(fout, "            b->tagName, b->vmMin, b->vmMax,\n");
    fprintf(fout, "            b->standardVersion, FALSE);\n");
    fprintf(fout, "        e->setGroupRangeRestriction(b->groupRestriction);\n");
    fprintf(fout, "        e->setElementRangeRestriction(b->elementRestriction);\n");
    fprintf(fout, "        addEntry(e);\n");
    fprintf(fout, "    }\n");
    fprintf(fout, "}\n");
    fprintf(fout, "\n");
    fprintf(fout, "\n");

    return 0;
}

/*
** CVS/RCS Log:
** $Log: mkdictbi.cc,v $
** Revision 1.4  1996-09-24 16:26:28  hewett
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
**
*/

