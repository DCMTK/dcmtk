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
** Update Date:		$Date: 1996-03-12 15:21:24 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/mkdictbi.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dcdict.h"


static void 
printSimpleEntry(FILE* fout, const DcmDictEntry* e, int lastEntry)
{
    fprintf(fout, "    { 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n",
	    e->getGroup(), e->getElement(), 
	    e->getUpperGroup(), e->getUpperElement());
    fprintf(fout, "      ((DcmEVR)(%d)) /*%s*/, \"%s\", %d, %d, \"%s\" }",
	    int(e->getVR().getEVR()), e->getVR().getVRName(),
	    e->getTagName(), 
	    e->getVMMin(), e->getVMMax(),
	    e->getStandardVersion());
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
getDateString(char* dateString)
{
    time_t systime = time(NULL);
    const char *ct = ctime(&systime);
    strcpy(dateString, ct);
    stripTrailing(dateString, '\n');
    return dateString;
}


int 
main(int argc, char* argv[])
{
    char* progname;
    FILE* fout = NULL;
    const DcmDictEntry* e = NULL;

    /* clear out any preloaded dictionary */
    dcmDataDict.clear();

    progname = argv[0];
    int i;
    for (i=1; i<argc; i++) { 
        dcmDataDict.loadDictionary(argv[i]);
    }
    
    fout = stdout;

    /* generate c++ code for static dictionary */

    char dateString[128];
    getDateString(dateString);

    fprintf(fout, "/*\n");
    fprintf(fout, "** DO NOT EDIT\n");
    fprintf(fout, "** This file is generated automatically by:\n");
    fprintf(fout, "** Prog: %s\n", progname);
    fputs("**\n", fout);
    fprintf(fout, "** Date: %s\n", dateString);
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
    fprintf(fout, "};\n");
    fprintf(fout, "\n");
    fprintf(fout, "static DBI_SimpleEntry simpleBuiltinDict[] = {\n");
    
    Pix p = NULL;
    Pix pp = NULL;
    int lastEntry = FALSE;
    for (p = dcmDataDict.first(); p != 0; dcmDataDict.next(p)) {
        e = dcmDataDict.contents(p);
        printSimpleEntry(fout, e, lastEntry);
    }

    for (p = dcmDataDict.repElementFirst(); p != 0; 
	 dcmDataDict.repElementNext(p)) {
        e = dcmDataDict.repElementContents(p);
        pp = p;
        dcmDataDict.repElementNext(pp);
        if (pp == 0) {
            lastEntry = TRUE;
        }
        printSimpleEntry(fout, e, lastEntry);
    }

    for (p = dcmDataDict.repGroupFirst(); p != 0; 
	 dcmDataDict.repGoupNext(p)) {
        e = dcmDataDict.repGroupContents(p);
        pp = p;
        dcmDataDict.repGroupNext(pp);
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
** Revision 1.2  1996-03-12 15:21:24  hewett
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

