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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-08-26 14:03:17 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdict.cc,v $
** CVS/RCS Revision:	$Revision: 1.12 $
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
** THE Global DICOM Data Dictionary
*/
 
DcmDataDictionary dcmDataDict(OFTrue, OFTrue);


/*
** Member Functions
*/

static DcmDictEntry*
makeSkelEntry(Uint16 group, Uint16 element, 
	     Uint16 upperGroup, Uint16 upperElement,
	     DcmEVR evr, const char* tagName, int vmMin, int vmMax,
	     const char* standardVersion,
	     DcmDictRangeRestriction groupRestriction,
	     DcmDictRangeRestriction elementRestriction)
{
    DcmDictEntry* e = NULL;
    e = new DcmDictEntry(group, element, upperGroup, upperElement, evr,
			 tagName, vmMin, vmMax, standardVersion, OFFalse);
    if (e != NULL) {
	e->setGroupRangeRestriction(groupRestriction);
	e->setElementRangeRestriction(elementRestriction);
    }
    return e;
}
 

OFBool DcmDataDictionary::loadSkeletonDictionary()
{
    /*
    ** We need to know about Group Lengths to compute them
    */
    DcmDictEntry* e = NULL;
    e = makeSkelEntry(0x0000, 0x0000, 0xffff, 0x0000,
		      EVR_UL, "GenericGroupLength", 1, 1, "GENERIC",
		      DcmDictRange_Unspecified, DcmDictRange_Unspecified);
    addEntry(e);
    e = makeSkelEntry(0x0000, 0x0001, 0xffff, 0x0001,
		      EVR_UL, "GenericGroupLengthToEnd", 1, 1, "GENERIC",
		      DcmDictRange_Unspecified, DcmDictRange_Unspecified);
    addEntry(e);
    /*
    ** We need to know about Items and Delimitation Items to parse
    ** (and construct) sequences.
    */
    e = makeSkelEntry(0xfffe, 0xe000, 0xfffe, 0xe000,
		      EVR_na, "Item", 1, 1, "DICOM3",
		      DcmDictRange_Unspecified, DcmDictRange_Unspecified);
    addEntry(e);
    e = makeSkelEntry(0xfffe, 0xe00d, 0xfffe, 0xe00d,
		      EVR_na, "ItemDelimitationItem", 1, 1, "DICOM3",
		      DcmDictRange_Unspecified, DcmDictRange_Unspecified);
    addEntry(e);
    e = makeSkelEntry(0xfffe, 0xe0dd, 0xfffe, 0xe0dd,
		      EVR_na, "SequenceDelimitationItem", 1, 1, "DICOM3",
		      DcmDictRange_Unspecified, DcmDictRange_Unspecified);
    addEntry(e);

    skeletonCount = numberOfEntries();
    return OFTrue;
}

DcmDataDictionary::DcmDataDictionary(OFBool loadBuiltin, OFBool loadExternal)
{
    loadSkeletonDictionary();
    dictionaryLoaded = OFFalse;
    if (loadBuiltin) {
	loadBuiltinDictionary();
	dictionaryLoaded = (numberOfEntries() > skeletonCount);
    }
    if (loadExternal) {
	if (loadExternalDictionaries()) {
	    dictionaryLoaded = OFTrue;
	}
    }
}

DcmDataDictionary::~DcmDataDictionary()
{
    clear();
}


void DcmDataDictionary::clear()
{
   hashDict.clear();
   repDict.clear();
}


static void
stripWhitespace(char* s)
{
  if (s)
  {
    register char c;
    register char *t;
    register char *p;
    t=p=s;
    /* while (((c = *t))&&(isspace(c))) t++; */
    while ((c = *t++)) if (!isspace(c)) *p++ = c;
    *p = '\0';
  }
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

static OFBool
parseVMField(char* vmField, int& vmMin, int& vmMax)
{
    OFBool ok = OFTrue;
    char c = 0;
    int dummy = 0;
    
    /* strip any whitespace */
    stripWhitespace(vmField);

    if (sscanf(vmField, "%d-%d%c", &vmMin, &dummy, &c) == 3) {
	/* treat "2-2n" like "2-n" for the moment */
	if ((c == 'n') || (c == 'N')) {
	    vmMax = DcmVariableVM;
	} else {
       	    ok = OFFalse;
	}
    } else if (sscanf(vmField, "%d-%d", &vmMin, &vmMax) == 2) {
	/* range VM (e.g. "2-6") */
    } else if (sscanf(vmField, "%d-%c", &vmMin, &c) == 2) {
	if ((c == 'n') || (c == 'N')) {
	    vmMax = DcmVariableVM;
	} else {
       	    ok = OFFalse;
	}
    } else if (sscanf(vmField, "%d%c", &vmMin, &c) == 2) {
	/* treat "2n" like "2-n" for the moment */
	if ((c == 'n') || (c == 'N')) {
	    vmMax = DcmVariableVM;
	} else {
       	    ok = OFFalse;
	}
    } else if (sscanf(vmField, "%d", &vmMin) == 1) {
	/* fixed VM */
	vmMax = vmMin;
    } else if (sscanf(vmField, "%c", &c) == 1) {
	/* treat "n" like "1-n" */
	if ((c == 'n') || (c == 'N')) {
	    vmMin = 1;
	    vmMax = DcmVariableVM;
	} else {
       	    ok = OFFalse;
	}
    } else {
	ok = OFFalse;
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
	stripWhitespace(fields[foundFields]);
	
	foundFields++;
	line = p + 1;
    } while ((foundFields < maxFields) && (p != NULL));

    return foundFields;
}

static OFBool
parseTagPart(char *s, unsigned int& l, unsigned int& h,
	     DcmDictRangeRestriction& r)
{
    OFBool ok = OFTrue;
    char restrictor = ' ';
    
    r = DcmDictRange_Unspecified; /* by default */

    if (sscanf(s, "%x-%c-%x", &l, &restrictor, &h) == 3) {
	switch (restrictor) {
	case 'o':
	case 'O':
	    r = DcmDictRange_Odd;
	    break;
	case 'e':
	case 'E':
	    r = DcmDictRange_Even;
	    break;
	case 'u':
	case 'U':
	    r = DcmDictRange_Unspecified;
	    break;
	default:
	    cerr << "unknown range restrictor: " << restrictor << endl;
	    ok = OFFalse;
	    break;
	}
    } else if (sscanf(s, "%x-%x", &l, &h) == 2) {
	r = DcmDictRange_Even; /* by default */
    } else if (sscanf(s, "%x", &l) == 1) {
	h = l;
    } else {
	ok = OFFalse;
    }
    return ok;
}

static OFBool
parseWholeTagField(char* s, DcmTagKey& key, 
		   DcmTagKey& upperKey,
		   DcmDictRangeRestriction& groupRestriction,
		   DcmDictRangeRestriction& elementRestriction)
{
    unsigned int gl, gh, el, eh;
    groupRestriction = DcmDictRange_Unspecified;
    elementRestriction = DcmDictRange_Unspecified;

    stripWhitespace(s);
    char gs[64];
    char es[64];
    int slen = strlen(s);

    if (s[0] != '(') return OFFalse;
    if (s[slen-1] != ')') return OFFalse;
    if (strchr(s, ',') == NULL) return OFFalse;

    /* separate the group and element parts */
    int i = 1; /* after the '(' */
    int gi = 0;
    for (; s[i] != ',' && s[i] != '\0'; i++) {
	gs[gi] = s[i];
	gi++;
    }
    gs[gi] = '\0';
    
    if (s[i] == '\0') return OFFalse; /* element part missing */
    
    i++; /* after the ',' */
    int ei = 0;
    for (; s[i] != ')' && s[i] != '\0'; i++) {
	es[ei] = s[i];
	ei++;
    }
    es[ei] = '\0';

    /* parse the tag parts into their components */
    if (parseTagPart(gs, gl, gh, groupRestriction) == OFFalse)
	return OFFalse;

    if (parseTagPart(es, el, eh, elementRestriction) == OFFalse)
	return OFFalse;

    key.set(gl,el);
    upperKey.set(gh,eh);

    return OFTrue;
}

static OFBool
onlyWhitespace(const char* s)
{
    int len = strlen(s);
    int charsFound = OFFalse;

    for (int i=0; (!charsFound) && (i<len); i++) {
	charsFound = !isspace(s[i]);
    }
    return (!charsFound)?(OFTrue):(OFFalse);
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

static OFBool
isaCommentLine(const char* s)
{
    OFBool isComment = OFFalse; /* assumption */
    int len = strlen(s);
    int i = 0;
    for (i=0; i<len && isspace(s[i]); i++) /*loop*/;
    isComment = (s[i] == DCM_DICT_COMMENT_CHAR);
    return isComment;
}

OFBool 
DcmDataDictionary::loadDictionary(const char* fileName, OFBool errorIfAbsent)
{

    char lineBuf[DCM_MAXDICTLINESIZE+1];
    FILE* f;
    int lineNumber = 0;
    char* lineFields[DCM_MAXDICTFIELDS+1];
    int fieldsPresent;
    DcmDictEntry* e;
    int errorsEncountered = 0;
    OFBool errorOnThisLine = OFFalse;
    int i;

    Uint16 groupNumber, elementNumber;
    Uint16 upperGroupNumber, upperElementNumber;
    DcmTagKey key, upperKey;
    DcmDictRangeRestriction groupRestriction;
    DcmDictRangeRestriction elementRestriction;
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
        return OFFalse;
    }

    while (getLine(lineBuf, DCM_MAXDICTLINESIZE, f)) {
	lineNumber++;

	if (onlyWhitespace(lineBuf)) {
	    continue; /* ignore this line */
	}
	if (isaCommentLine(lineBuf)) {
	    continue; /* ignore this line */
	}

	errorOnThisLine = OFFalse;
    
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
	standardVersion = "DICOM";

	switch (fieldsPresent) {
	case 0:
	case 1:
	case 2:
	    cerr << "DcmDataDictionary: "<< fileName << ": "
		 << "too few fields (line " 
		 << lineNumber << "): " << fileName << endl;
	    errorOnThisLine = OFTrue;
	    break;
	default:
	    cerr << "DcmDataDictionary: " << fileName << ": "
		 << "too many fields (line " 
		 << lineNumber << "): " << endl;
	    errorOnThisLine = OFTrue;
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
		errorOnThisLine = OFTrue;
	    }
	    /* drop through to next case label */
	case 3:
	    if (!parseWholeTagField(lineFields[0], key, upperKey, 
				    groupRestriction, elementRestriction)) {
		cerr << "DcmDataDictionary: " << fileName << ": "
		     << "bad Tag field (line " 
		     << lineNumber << "): " << lineFields[0] << endl;
		errorOnThisLine = OFTrue;
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
		errorOnThisLine = OFTrue;
	    }
	}
		
	if (!errorOnThisLine) {
	    e = new DcmDictEntry(key.getGroup(), key.getElement(), 
				 upperKey.getGroup(), upperKey.getElement(),
				 vr, tagName, vmMin, vmMax, standardVersion);
	    e->setGroupRangeRestriction(groupRestriction);
	    e->setElementRangeRestriction(elementRestriction);
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
    
    /* return OFFalse if errors were encountered */
    return (errorsEncountered == 0) ? (OFTrue) : (OFFalse);
}

#ifndef HAVE_GETENV

static
char* getenv() {
    return NULL;
}

#endif /* !HAVE_GETENV */



OFBool 
DcmDataDictionary::loadExternalDictionaries()
{
    char* env = NULL;
    int len;
    int sepCnt = 0;
    OFBool msgIfDictAbsent = OFTrue;
    OFBool loadFailed = OFFalse;

    env = getenv(DCM_DICT_ENVIRONMENT_VARIABLE);
    if ((env == NULL) || (strlen(env) == 0)) {
        env = DCM_DICT_DEFAULT_PATH;
	msgIfDictAbsent = OFFalse;
    }

    if ((env != NULL) && (strlen(env) != 0)) {
	len = strlen(env);
	for (int i=0; i<len; i++) {
	    if (env[i] == ENVIRONMENT_PATH_SEPARATOR) {
		sepCnt++;
	    }
	}
	
	if (sepCnt == 0) {
	    if (!loadDictionary(env, msgIfDictAbsent)) {
		return OFFalse;
	    }
	} else {
	    char** dictArray;

	    dictArray = (char**) malloc((sepCnt + 1) * sizeof(char*));
			
	    int ndicts = splitFields(env, dictArray, sepCnt+1, 
				     ENVIRONMENT_PATH_SEPARATOR);
	    
	    for (int i=0; i<ndicts; i++) {
		if ((dictArray[i] != NULL) && (strlen(dictArray[i]) > 0)) {
		    if (!loadDictionary(dictArray[i], msgIfDictAbsent)) {
			loadFailed = OFTrue;
		    }
		}
		free(dictArray[i]);
	    }
	    free(dictArray);        
	}
    }

    return (loadFailed) ? (OFFalse) : (OFTrue);
}

    
void 
DcmDataDictionary::addEntry(DcmDictEntry* e)
{
    if (e->isRepeating()) {
	/* 
	 * Find the best position in repeating tag list 
	 * Existing entries are replaced if the ranges and repetition
	 * constraints are the same.
	 * If a range represents a subset of an existing range then it 
	 * will be placed before it in the list.  This ensures that a 
	 * search will find the subset rather than the superset.
	 * Otherwise entries are appended to the end of the list.
	 */
	OFBool inserted = OFFalse;

	DcmDictEntryListIterator iter(repDict.begin());
	DcmDictEntryListIterator last(repDict.end());
	for (; !inserted && iter != last; ++iter) {
	    if (e->setEQ(**iter)) {
		/* replace the old entry with the new */
		DcmDictEntry *old = *iter;
		*iter = e;
		delete old;
		inserted = OFTrue;
	    } else if (e->subset(**iter)) {
		/* e is a subset of the current list position, insert before */
		repDict.insert(iter, e);
		inserted = OFTrue;
	    }
	}
	if (!inserted) {
	    /* insert at end */
	    repDict.push_back(e);
	    inserted = OFTrue;
	}
    } else {
	hashDict.put(e);
    }
}

void 
DcmDataDictionary::deleteEntry(const DcmDictEntry& entry)
{
    DcmDictEntry* e = NULL;
    e = (DcmDictEntry*)findEntry(entry);
    if (e != NULL) {
	if (e->isRepeating()) {
	    repDict.remove(e);
	    delete e;
        } else {
	    hashDict.del(entry.getKey());
        }
    }
}

const DcmDictEntry* 
DcmDataDictionary::findEntry(const DcmDictEntry& entry)
{
    const DcmDictEntry* e = NULL;

    if (entry.isRepeating()) {
	OFBool found = OFFalse;
	DcmDictEntryListIterator iter(repDict.begin());
	DcmDictEntryListIterator last(repDict.end());
	for (; !found && iter != last; ++iter) {
	    if (entry.setEQ(**iter)) {
		found = OFTrue;
		e = *iter;
	    }
	}
    } else {
	e = hashDict.get(entry);
    }
    return e;
}

const DcmDictEntry* 
DcmDataDictionary::findEntry(const DcmTagKey& key)
{
    /* search first in the normal tags dictionary and if not found
     * then search in the repeating tags list.
     */
    const DcmDictEntry* e = NULL;

    e = hashDict.get(key);
    if (e == NULL) {
	/* search in the repeating tags dictionary */
	OFBool found = OFFalse;
	DcmDictEntryListIterator iter(repDict.begin());
	DcmDictEntryListIterator last(repDict.end());
	for (; !found && iter != last; ++iter) {
	    if ((*iter)->contains(key)) {
		found = OFTrue;
		e = *iter;
	    }
	}
    }
    return e;
}

const DcmDictEntry* 
DcmDataDictionary::findEntry(const char *name)
{
    const DcmDictEntry* e = NULL;

    /* search first in the normal tags dictionary and if not found
     * then search in the repeating tags list.
     */
    DcmHashDictIterator iter;
    for (iter=hashDict.begin(); (e==NULL) && (iter!=hashDict.end()); ++iter) {
	if ((*iter)->contains(name)) {
	    e = *iter;
	}
    }
    if (e == NULL) {
	/* search in the repeating tags dictionary */
	OFBool found = OFFalse;
	DcmDictEntryListIterator iter(repDict.begin());
	DcmDictEntryListIterator last(repDict.end());
	for (; !found && iter != last; ++iter) {
	    if ((*iter)->contains(name)) {
		found = OFTrue;
		e = *iter;
	    }
	}
    }
    return e;
}

/*
** CVS/RCS Log:
** $Log: dcdict.cc,v $
** Revision 1.12  1997-08-26 14:03:17  hewett
** New data structures for data-dictionary.  The main part of the
** data-dictionary is now stored in an hash table using an optimized
** hash function.  This new data structure reduces data-dictionary
** load times by a factor of 4!  he data-dictionary specific linked-list
** has been replaced by a linked list derived from OFList class
** (see ofstd/include/oflist.h).
** The only interface modifications are related to iterating over the entire
** data dictionary which should not be needed by "normal" applications.
**
** Revision 1.11  1997/07/31 15:55:11  meichel
** New routine stripWhitespace() in dcdict.cc, much faster.
**
** Revision 1.10  1997/07/21 08:25:25  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/22 13:16:04  hewett
** Added method DcmDataDictionary::isDictionaryLoaded() to ask if a full
** data dictionary has been loaded.  This method should be used in tests
** rather that querying the number of entries (a sekelton dictionary is
** now always present).
**
** Revision 1.8  1997/05/13 13:49:37  hewett
** Modified the data dictionary parse code so that it can handle VM
** descriptions of the form "2-2n" (as used in some supplements).
** Currently, a VM of "2-2n" will be represented internally as "2-n".
** Also added preload of a few essential attribute descriptions into
** the data dictionary (e.g. Item and ItemDelimitation tags).
**
** Revision 1.7  1996/09/18 16:37:26  hewett
** Added capability to search data dictionary by tag name.  The
** source code for these changes was contributed by Larry V. Streepy,
** Jr., Chief Technical Officer,  Healthcare Communications, Inc.,
** (mailto:streepy@healthcare.com).
**
** Revision 1.6  1996/04/18 09:51:00  hewett
** White space is now being stripped from data dictionary fields.  Previously
** a tag name could retain trailing whitespace which caused silly results
** when generating dcdeftag.h (e.g. tag names wil trailing underscores).
**
** Revision 1.5  1996/03/20 16:44:04  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
** Revision 1.4  1996/03/12 15:21:22  hewett
** The repeating sub-dictionary has been split into a repeatingElement and
** a repeatingGroups dictionary.  This is a temporary measure to reduce the
** problem of overlapping dictionary entries.  A full solution will require
** more radical changes to the data dictionary insertion and search
** mechanims.
**
** Revision 1.3  1996/01/09 11:06:44  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.2  1996/01/05 13:27:34  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.1  1995/11/23 17:02:39  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/

