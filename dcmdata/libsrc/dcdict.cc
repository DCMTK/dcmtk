/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcdict.cc
**
** Purpose:
** Loadable DICOM data dictionary
** 
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:34 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdict.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dcdict.h"
#include "dcdefine.h"

/*
** The separator character between fields in the data dictionary file(s)
*/
#define DCM_DICT_FIELD_SEPARATOR_CHAR '\t'

/*
** Comment character for the data dictionary file(s)
*/
#define DCM_DICT_COMMENT_CHAR '#'

/*
** A dictionary tree is rebalanced after 
** adding every DCM_DICT_BALANCE_RATE entries.
*/
#define DCM_DICT_BALANCE_RATE 5

/*
** THE Global DICOM Data Dictionary
*/
 
DcmDataDictionary dcmDataDict(TRUE, TRUE);


/*
** Member Functions
*/

DcmDataDictionary::DcmDataDictionary(BOOL loadBuiltin, BOOL loadExternal)
{
    if (loadBuiltin) {
	loadBuiltinDictionary();
    }
    if (loadExternal) {
	loadExternalDictionaries();
    }
}

DcmDataDictionary::~DcmDataDictionary()
{
    clear();
}


void DcmDataDictionary::clear()
{
   Pix p = NULL;
   DcmDictEntry* e = NULL;

   for (p = dict.first(); p != 0; dict.next(p)) {
       e = dict(p);
       delete e;
   }
   dict.clear();

   for (p = repeatingDict.first(); p != NULL; repeatingDict.next(p)) {
       e = repeatingDict(p);
       delete e;
   }
   repeatingDict.clear();
}


static void
stripWhitespace(char* s)
{
    char* p;

    if (s == NULL) return;
		
    p = s;
    while (*s != '\0') {
	if (isspace(*s) == FALSE) {
	    *p++ = *s;
	}
	s++;
    }
    *p = '\0';
}

static char*
stripTrailingWhitespace(char* s)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; i >= 0 && isspace(s[i]); i--)
        s[i] = '\0';
    return s;
}

static BOOL
parseVMField(char* vmField, int& vmMin, int& vmMax)
{
    BOOL ok = TRUE;
    char c;
    
    /* strip any whitespace */
    stripWhitespace(vmField);


    if (sscanf(vmField, "%d-%d", &vmMin, &vmMax) == 2) {
	/* range vm */
    } else if (sscanf(vmField, "%d-%c", &vmMin, &c) == 2) {
	if ((c == 'n') || (c == 'N')) {
	    vmMax = DcmVariableVM;
	} else {
       	    ok = FALSE;
	}
    } else if (sscanf(vmField, "%d", &vmMin) == 1) {
	vmMax = vmMin;
    } else {
	ok = FALSE;
    }
    return ok;
}

static int
splitFields(char* line, char* fields[], int maxFields, char splitChar)
{
    char* p;
    int foundFields = 0;
    int len;

    do {
	p = strchr(line, splitChar);
	if (p == NULL) {
	    len = strlen(line);
	} else {
	    len = p - line;
	}
	fields[foundFields] = (char*)malloc(len+1);
	strncpy(fields[foundFields], line, len);
	fields[foundFields][len] = '\0';
	
	foundFields++;
	line = p + 1;
    } while ((foundFields < maxFields) && (p != NULL));

    return foundFields;
}

static BOOL
parseWholeTagField(char* s, DcmTagKey& key, DcmTagKey& upperKey)
{
    BOOL ok = TRUE;
    unsigned int gl, gh, el, eh;
    
    stripWhitespace(s);

    if (sscanf(s, "(%x-%x,%x-%x)", &gl, &gh, &el, &eh) == 4) {
	/* repeating group and element */
	key.set(gl,el);
	upperKey.set(gh,eh);
    } else if (sscanf(s, "(%x-%x,%x)", &gl, &gh, &el) == 3) {
	/* only repeating group */
	key.set(gl,el);
	upperKey.set(gh, el);
    } else if (sscanf(s, "(%x,%x-%x)", &gl, &el, &eh) == 3) {
	/* only repeating element */
	key.set(gl,el);
	upperKey.set(gl,eh);
    } else if (sscanf(s, "(%x,%x)", &gl, &el) == 2) {
	/* normal element */
        key.set(gl, el);
	upperKey.set(gl, el);
    } else {
        ok = FALSE;
    }
    return ok;
}

static BOOL
onlyWhitespace(const char* s)
{
    int len = strlen(s);
    int charsFound = FALSE;

    for (int i=0; (!charsFound) && (i<len); i++) {
	charsFound = !isspace(s[i]);
    }
    return (!charsFound)?(TRUE):(FALSE);
}

static char*
getLine(char* line, int maxLineLen, FILE* f)
{
    char* s;

    s = fgets(line, maxLineLen, f);
	
    /* strip any trailing white space */
    stripTrailingWhitespace(line);
		
    return s;
}

static BOOL
isaCommentLine(const char* s)
{
    BOOL isComment = FALSE; /* assumption */
    int len = strlen(s);
    int i = 0;
    for (i=0; i<len && isspace(s[i]); i++) /*loop*/;
    isComment = (s[i] == DCM_DICT_COMMENT_CHAR);
    return isComment;
}

BOOL 
DcmDataDictionary::loadDictionary(const char* fileName, BOOL errorIfAbsent)
{

    char lineBuf[DCM_MAXDICTLINESIZE+1];
    FILE* f;
    int lineNumber = 0;
    char* lineFields[DCM_MAXDICTFIELDS+1];
    int fieldsPresent;
    DcmDictEntry* e;
    int errorsEncountered = 0;
    BOOL errorOnThisLine = FALSE;
    int i;

    Uint16 groupNumber, elementNumber;
    Uint16 upperGroupNumber, upperElementNumber;
    DcmTagKey key, upperKey;
    DcmVR vr;
    char* vrName;
    char* tagName;
    int vmMin, vmMax = 1;    
    char* standardVersion;
    
    if ((f = fopen(fileName, "r")) == NULL) {
	if (errorIfAbsent) {
            cerr << "DcmDataDictionary: " << "cannot open: " 
		 << fileName << endl;
	}
        return FALSE;
    }

    while (getLine(lineBuf, DCM_MAXDICTLINESIZE, f)) {
	lineNumber++;

	if (onlyWhitespace(lineBuf)) {
	    continue; /* ignore this line */
	}
	if (isaCommentLine(lineBuf)) {
	    continue; /* ignore this line */
	}

	errorOnThisLine = FALSE;
    
        /* fields are tab separated */
	fieldsPresent = splitFields(lineBuf, lineFields, 
				    DCM_MAXDICTFIELDS, 
				    DCM_DICT_FIELD_SEPARATOR_CHAR);

	/* initialize dict entry fields */
	groupNumber = elementNumber = 0xFFFF;
	upperGroupNumber = upperElementNumber = 0xFFFF;
	vrName = NULL;
	tagName = NULL;
	vmMin = vmMax = 1;
	standardVersion = "DICOM3";

	switch (fieldsPresent) {
	case 0:
	case 1:
	case 2:
	    cerr << "DcmDataDictionary: "<< fileName << ": "
		 << "too few fields (line " 
		 << lineNumber << "): " << fileName << endl;
	    errorOnThisLine = TRUE;
	    break;
	default:
	    cerr << "DcmDataDictionary: " << fileName << ": "
		 << "too many fields (line " 
		 << lineNumber << "): " << endl;
	    errorOnThisLine = TRUE;
	    break;
	case 5:
	    standardVersion = lineFields[4];
	    /* drop through to next case label */
	case 4:
	    /* the VM field is present */
	    if (!parseVMField(lineFields[3], vmMin, vmMax)) {
		cerr << "DcmDataDictionary: " << fileName << ": "
		     << "bad VM field (line " 
		     << lineNumber << "): " << lineFields[3] << endl;
		errorOnThisLine = TRUE;
	    }
	    /* drop through to next case label */
	case 3:
	    if (!parseWholeTagField(lineFields[0], key, upperKey)) {
		cerr << "DcmDataDictionary: " << fileName << ": "
		     << "bad Tag field (line " 
		     << lineNumber << "): " << lineFields[0] << endl;
		errorOnThisLine = TRUE;
	    } else {
		/* all is OK */
		vrName = lineFields[1];
		tagName = lineFields[2];
	    }
	}

	if (!errorOnThisLine) {
	    /* check the VR Field */
	    vr.setVR(vrName);
	    if (vr.getEVR() == EVR_UNKNOWN) {
		cerr << "DcmDataDictionary: " << fileName << ": "
		     << "bad VR field (line " 
		     << lineNumber << "): " << vrName << endl;
		errorOnThisLine = TRUE;
	    }
	}
		
	if (!errorOnThisLine) {
	    e = new DcmDictEntry(key.getGroup(), key.getElement(), 
				 upperKey.getGroup(), upperKey.getElement(),
				 vr, tagName, vmMin, vmMax, standardVersion);

	    addEntry(e);
	}

	for (i=0; i<fieldsPresent; i++) {
	    free(lineFields[i]);
	    lineFields[i] = NULL;
	}
		
	if (errorOnThisLine) {
	    errorsEncountered++;
	}
    }
    
    fclose(f);
    
    if (errorsEncountered == 0) {
	/* perform a final rebalance */
	balance();
    }
    
    /* return false if errors were encountered */
    return (errorsEncountered == 0) ? (TRUE) : (FALSE);
}

#ifndef HAVE_GETENV

static
char* getenv() {
    return NULL;
}

#endif /* !HAVE_GETENV */



BOOL 
DcmDataDictionary::loadExternalDictionaries()
{
    char* env = NULL;
    int i, len;
    int sepCnt = 0;
    BOOL msgIfDictAbsent = TRUE;
    BOOL loadFailed = FALSE;

    env = getenv(DCM_DICT_ENVIRONMENT_VARIABLE);
    if ((env == NULL) || (strlen(env) == 0)) {
        env = DCM_DICT_DEFAULT_PATH;
	msgIfDictAbsent = FALSE;
    }

    if ((env != NULL) && (strlen(env) != 0)) {
	len = strlen(env);
	for (i=0; i<len; i++) {
	    if (env[i] == ENVIRONMENT_PATH_SEPARATOR) {
		sepCnt++;
	    }
	}
	
	if (sepCnt == 0) {
	    if (!loadDictionary(env, msgIfDictAbsent)) {
		return FALSE;
	    }
	} else {
	    char** dictArray;

	    dictArray = (char**) malloc((sepCnt + 1) * sizeof(char*));
			
	    int ndicts = splitFields(env, dictArray, sepCnt+1, 
				     ENVIRONMENT_PATH_SEPARATOR);
	    
	    for (int i=0; i<ndicts; i++) {
		if ((dictArray[i] != NULL) && (strlen(dictArray[i]) > 0)) {
		    if (!loadDictionary(dictArray[i], msgIfDictAbsent)) {
			loadFailed = TRUE;
		    }
		}
		free(dictArray[i]);
	    }
	    free(dictArray);        
	}
    }

    return (loadFailed) ? (FALSE) : (TRUE);
}

void
DcmDataDictionary::balance()
{
    dict.balance();
    repeatingDict.balance();
}
    
void 
DcmDataDictionary::addEntry(DcmDictEntry* e)
{
    /*
     * The dictionaries are implemented as binary trees.
     * Since the entries are often added in assending
     * order, we would get a pathalogically unbalanced
     * tree.  It is thus worthwhile to perform a
     * rebalance every once in a while.  Every DCM_DICT_BALANCE_RATE
     * entries seems reasonable.
     */

    if (e->isRepeating()) {
        repeatingDict.add(e, TRUE);
        if ((repeatingDict.length() % DCM_DICT_BALANCE_RATE) == 0) {
            repeatingDict.balance();
        }
    } else {
        dict.add(e, TRUE);
        if ((dict.length() % DCM_DICT_BALANCE_RATE) == 0) {
            dict.balance();
        }
    }
}

void 
DcmDataDictionary::deleteEntry(const DcmDictEntry& entry)
{
    DcmDictEntry* e = NULL;

    e = (DcmDictEntry*)findEntry(entry);
    if (e != NULL) {
        if (e->isRepeating()) {
	    repeatingDict.del(e);
            delete e;
        } else {
	    dict.del(e);
	    delete e;
        }
    }
}

const DcmDictEntry* 
DcmDataDictionary::findEntry(const DcmDictEntry& entry)
{
    const DcmDictEntry* e = NULL;
    Pix p = NULL;

    if (entry.isRepeating()) {
	p = repeatingDict.seek((DcmDictEntry*)&entry);
	if (p != NULL) {
	    e = repeatingDict(p);
        }
    } else {
        p = dict.seek((DcmDictEntry*)&entry);
        if (p != NULL) {
            e = dict(p);
        }
    }
    return e;
}

const DcmDictEntry* 
DcmDataDictionary::findEntry(const DcmTagKey& key)
{
    DcmDictEntry keyEntry(key);
    const DcmDictEntry* e = NULL;
    Pix p = NULL;

    /* search first in the normal tags dictionary, then
     * in the repeating tags dictionary 
     */
    p = dict.seek(&keyEntry);
    if (p != NULL) {
        e = dict(p);
    } else {
	p = repeatingDict.seek(&keyEntry);
	if (p != NULL) {
	    e = repeatingDict(p);
        }
    }
    return e;
}

/*
** CVS/RCS Log:
** $Log: dcdict.cc,v $
** Revision 1.2  1996-01-05 13:27:34  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.1  1995/11/23 17:02:39  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/

