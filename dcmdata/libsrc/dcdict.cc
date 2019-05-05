/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Purpose: loadable DICOM data dictionary
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

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

GlobalDcmDataDictionary dcmDataDict;


/*
** Member Functions
*/

static DcmDictEntry*
makeSkelEntry(Uint16 group, Uint16 element,
              Uint16 upperGroup, Uint16 upperElement,
              DcmEVR evr, const char* tagName, int vmMin, int vmMax,
              const char* standardVersion,
              DcmDictRangeRestriction groupRestriction,
              DcmDictRangeRestriction elementRestriction,
              const char* privCreator)
{
    DcmDictEntry* e = NULL;
    e = new DcmDictEntry(group, element, upperGroup, upperElement, evr,
                         tagName, vmMin, vmMax, standardVersion, OFFalse, privCreator);
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
                      DcmDictRange_Unspecified, DcmDictRange_Unspecified, NULL);
    addEntry(e);

    /*
    ** We need to know about Items and Delimitation Items to parse
    ** (and construct) sequences.
    */
    e = makeSkelEntry(0xfffe, 0xe000, 0xfffe, 0xe000,
                      EVR_na, "Item", 1, 1, "DICOM",
                      DcmDictRange_Unspecified, DcmDictRange_Unspecified, NULL);
    addEntry(e);
    e = makeSkelEntry(0xfffe, 0xe00d, 0xfffe, 0xe00d,
                      EVR_na, "ItemDelimitationItem", 1, 1, "DICOM",
                      DcmDictRange_Unspecified, DcmDictRange_Unspecified, NULL);
    addEntry(e);
    e = makeSkelEntry(0xfffe, 0xe0dd, 0xfffe, 0xe0dd,
                      EVR_na, "SequenceDelimitationItem", 1, 1, "DICOM",
                      DcmDictRange_Unspecified, DcmDictRange_Unspecified, NULL);
    addEntry(e);

    skeletonCount = numberOfEntries();
    return OFTrue;
}


DcmDataDictionary::DcmDataDictionary(OFBool loadBuiltin, OFBool loadExternal)
  : hashDict(),
    repDict(),
    skeletonCount(0),
    dictionaryLoaded(OFFalse)
{
    /* Make sure any DCMDICTPATH dictionary is loaded even if loading
     * of external (default) dictionary is not enabled.
     */
    if (!loadExternal)
    {
        const char* env = getenv(DCM_DICT_ENVIRONMENT_VARIABLE);
        if ((env != NULL) && (strlen(env) != 0))
            loadExternal = OFTrue;
    }
    reloadDictionaries(loadBuiltin, loadExternal);
}

DcmDataDictionary::~DcmDataDictionary()
{
    clear();
}


void DcmDataDictionary::clear()
{
   hashDict.clear();
   repDict.clear();
   skeletonCount = 0;
   dictionaryLoaded = OFFalse;
}


static void
stripWhitespace(char* s)
{
  if (s)
  {
    unsigned char c;
    unsigned char *t;
    unsigned char *p;
    t=p=OFreinterpret_cast(unsigned char *, s);
    while ((c = *t++)) if (!isspace(c)) *p++ = c;
    *p = '\0';
  }
}

static char*
stripTrailingWhitespace(char* s)
{
    if (s == NULL) return s;
    for
    (
        char* it = s + strlen(s) - 1;
        it >= s && isspace(OFstatic_cast(unsigned char, *it));
        *it-- = '\0'
    );
    return s;
}

static void
stripLeadingWhitespace(char* s)
{
  if (s)
  {
    unsigned char c;
    unsigned char *t;
    unsigned char *p;
    t=p=OFreinterpret_cast(unsigned char *, s);
    while (isspace(*t)) t++;
    while ((c = *t++)) *p++ = c;
    *p = '\0';
  }
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
splitFields(const char* line, char* fields[], int maxFields, char splitChar)
{
    const char *p;
    int foundFields = 0;
    size_t len;

    do {
#ifdef __BORLANDC__
        // Borland Builder expects a non-const argument
        p = strchr(OFconst_cast(char *, line), splitChar);
#else
        p = strchr(line, splitChar);
#endif
        if (p == NULL) {
            len = strlen(line);
        } else {
            len = p - line;
        }
        fields[foundFields] = OFstatic_cast(char *, malloc(len + 1));
        OFStandard::strlcpy(fields[foundFields], line, len+1);
        fields[foundFields][len] = '\0';
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
            DCMDATA_ERROR("DcmDataDictionary: Unknown range restrictor: " << restrictor);
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
                   DcmDictRangeRestriction& elementRestriction,
                   char *&privCreator)
{
    unsigned int gl, gh, el, eh;
    groupRestriction = DcmDictRange_Unspecified;
    elementRestriction = DcmDictRange_Unspecified;

    stripLeadingWhitespace(s);
    stripTrailingWhitespace(s);

    char gs[64];
    char es[64];
    char pc[64];
    size_t slen = strlen(s);

    if (s[0] != '(') return OFFalse;
    if (s[slen - 1] != ')') return OFFalse;
    if (strchr(s, ',') == NULL) return OFFalse;

    /* separate the group and element parts */
    int i = 1; /* after the '(' */
    int gi = 0;
    for (; s[i] != ',' && s[i] != '\0'; i++)
    {
        gs[gi] = s[i];
        gi++;
    }
    gs[gi] = '\0';

    if (s[i] == '\0') return OFFalse; /* element part missing */
    i++; /* after the ',' */

    stripLeadingWhitespace(s + i);

    int pi = 0;
    if (s[i] == '\"') /* private creator */
    {
        i++;  // skip opening quotation mark
        for (; s[i] != '\"' && s[i] != '\0'; i++) pc[pi++] = s[i];
        pc[pi] = '\0';
        if (s[i] == '\0') return OFFalse; /* closing quotation mark missing */
        i++;
        stripLeadingWhitespace(s + i);
        if (s[i] != ',') return OFFalse; /* element part missing */
        i++; /* after the ',' */
    }

    int ei = 0;
    for (; s[i] != ')' && s[i] != '\0'; i++) {
        es[ei] = s[i];
        ei++;
    }
    es[ei] = '\0';

    /* parse the tag parts into their components */
    stripWhitespace(gs);
    if (parseTagPart(gs, gl, gh, groupRestriction) == OFFalse)
        return OFFalse;

    stripWhitespace(es);
    if (parseTagPart(es, el, eh, elementRestriction) == OFFalse)
        return OFFalse;

    if (pi > 0)
    {
      // copy private creator name
      size_t buflen = strlen(pc) + 1;
      privCreator = new char[buflen]; // deleted by caller
      if (privCreator) OFStandard::strlcpy(privCreator, pc, buflen);
    }

    key.set(OFstatic_cast(unsigned short, gl), OFstatic_cast(unsigned short, el));
    upperKey.set(OFstatic_cast(unsigned short, gh), OFstatic_cast(unsigned short, eh));

    return OFTrue;
}

static OFBool
onlyWhitespace(const char* s)
{
    size_t len = strlen(s);
    int charsFound = OFFalse;

    for (size_t i = 0; (!charsFound) && (i < len); ++i) {
        charsFound = !isspace(OFstatic_cast(unsigned char, s[i]));
    }
    return (!charsFound)? (OFTrue) : (OFFalse);
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
    size_t len = strlen(s);
    size_t i = 0;
    for (i = 0; i < len && isspace(OFstatic_cast(unsigned char, s[i])); ++i) /*loop*/;
    isComment = (s[i] == DCM_DICT_COMMENT_CHAR);
    return isComment;
}

OFBool
DcmDataDictionary::reloadDictionaries(OFBool loadBuiltin, OFBool loadExternal)
{
    OFBool result = OFTrue;
    clear();
    loadSkeletonDictionary();
    if (loadBuiltin) {
        loadBuiltinDictionary();
        dictionaryLoaded = (numberOfEntries() > skeletonCount);
        if (!dictionaryLoaded) result = OFFalse;
    }
    if (loadExternal) {
        if (loadExternalDictionaries())
            dictionaryLoaded = OFTrue;
        else
            result = OFFalse;
    }
    return result;
}

OFBool
DcmDataDictionary::loadDictionary(const char* fileName, OFBool errorIfAbsent)
{

    char lineBuf[DCM_MAXDICTLINESIZE + 1];
    FILE* f = NULL;
    int lineNumber = 0;
    char* lineFields[DCM_MAXDICTFIELDS + 1];
    int fieldsPresent;
    DcmDictEntry* e;
    int errorsEncountered = 0;
    OFBool errorOnThisLine = OFFalse;
    int i;

    DcmTagKey key, upperKey;
    DcmDictRangeRestriction groupRestriction = DcmDictRange_Unspecified;
    DcmDictRangeRestriction elementRestriction = DcmDictRange_Unspecified;
    DcmVR vr;
    char* vrName;
    char* tagName;
    char* privCreator;
    int vmMin, vmMax = 1;
    const char* standardVersion;

    /* first, check whether 'fileName' really points to a file (and not to a directory or the like) */
    if (!OFStandard::fileExists(fileName) || (f = fopen(fileName, "r")) == NULL) {
        if (errorIfAbsent) {
            DCMDATA_ERROR("DcmDataDictionary: Cannot open file: " << fileName);
        }
        return OFFalse;
    }

    DCMDATA_DEBUG("DcmDataDictionary: Loading file: " << fileName);

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
        vrName = NULL;
        tagName = NULL;
        privCreator = NULL;
        vmMin = vmMax = 1;
        standardVersion = "DICOM";

        switch (fieldsPresent) {
        case 0:
        case 1:
        case 2:
            DCMDATA_ERROR("DcmDataDictionary: "<< fileName << ": "
                 << "too few fields (line " << lineNumber << ")");
            errorOnThisLine = OFTrue;
            break;
        default:
            DCMDATA_ERROR("DcmDataDictionary: " << fileName << ": "
                 << "too many fields (line " << lineNumber << "): ");
            errorOnThisLine = OFTrue;
            break;
        case 5:
            stripWhitespace(lineFields[4]);
            standardVersion = lineFields[4];
            /* drop through to next case label */
        case 4:
            /* the VM field is present */
            if (!parseVMField(lineFields[3], vmMin, vmMax)) {
                DCMDATA_ERROR("DcmDataDictionary: " << fileName << ": "
                     << "bad VM field (line " << lineNumber << "): " << lineFields[3]);
                errorOnThisLine = OFTrue;
            }
            /* drop through to next case label */
        case 3:
            if (!parseWholeTagField(lineFields[0], key, upperKey,
                 groupRestriction, elementRestriction, privCreator))
            {
                DCMDATA_ERROR("DcmDataDictionary: " << fileName << ": "
                     << "bad Tag field (line " << lineNumber << "): " << lineFields[0]);
                errorOnThisLine = OFTrue;
            } else {
                /* all is OK */
                vrName = lineFields[1];
                stripWhitespace(vrName);

                tagName = lineFields[2];
                stripWhitespace(tagName);
            }
        }

        if (!errorOnThisLine) {
            /* check the VR Field */
            vr.setVR(vrName);
            if (vr.getEVR() == EVR_UNKNOWN) {
                DCMDATA_ERROR("DcmDataDictionary: " << fileName << ": "
                     << "bad VR field (line " << lineNumber << "): " << vrName);
                errorOnThisLine = OFTrue;
            }
        }

        if (!errorOnThisLine) {
            e = new DcmDictEntry(
                key.getGroup(), key.getElement(),
                upperKey.getGroup(), upperKey.getElement(),
                vr, tagName, vmMin, vmMax, standardVersion, OFTrue,
                privCreator);

            e->setGroupRangeRestriction(groupRestriction);
            e->setElementRangeRestriction(elementRestriction);
            addEntry(e);
        }

        for (i = 0; i < fieldsPresent; i++) {
            free(lineFields[i]);
            lineFields[i] = NULL;
        }

        delete[] privCreator;

        if (errorOnThisLine) {
            errorsEncountered++;
        }
    }

    fclose(f);

    /* return OFFalse in case of errors and set internal state accordingly */
    if (errorsEncountered == 0) {
        dictionaryLoaded = OFTrue;
        return OFTrue;
    }
    else {
        dictionaryLoaded = OFFalse;
        return OFFalse;
    }
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
    const char* env = NULL;
    size_t len;
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
        for (size_t i = 0; i < len; ++i) {
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

            dictArray = OFstatic_cast(char **, malloc((sepCnt + 1) * sizeof(char*)));

            int ndicts = splitFields(env, dictArray, sepCnt + 1,
                                     ENVIRONMENT_PATH_SEPARATOR);

            for (int ii = 0; ii < ndicts; ii++) {
                if ((dictArray[ii] != NULL) && (strlen(dictArray[ii]) > 0)) {
                    if (!loadDictionary(dictArray[ii], msgIfDictAbsent)) {
                        loadFailed = OFTrue;
                    }
                }
                free(dictArray[ii]);
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
#ifdef PRINT_REPLACED_DICTIONARY_ENTRIES
                DCMDATA_WARN("replacing " << *old);
#endif
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
    e = OFconst_cast(DcmDictEntry *, findEntry(entry));
    if (e != NULL) {
        if (e->isRepeating()) {
            repDict.remove(e);
            delete e;
        } else {
            hashDict.del(entry.getKey(), entry.getPrivateCreator());
        }
    }
}

const DcmDictEntry*
DcmDataDictionary::findEntry(const DcmDictEntry& entry) const
{
    const DcmDictEntry* e = NULL;

    if (entry.isRepeating()) {
        OFBool found = OFFalse;
        DcmDictEntryListConstIterator iter(repDict.begin());
        DcmDictEntryListConstIterator last(repDict.end());
        for (; !found && iter != last; ++iter) {
            if (entry.setEQ(**iter)) {
                found = OFTrue;
                e = *iter;
            }
        }
    } else {
        e = hashDict.get(entry, entry.getPrivateCreator());
    }
    return e;
}

const DcmDictEntry*
DcmDataDictionary::findEntry(const DcmTagKey& key, const char *privCreator) const
{
    /* search first in the normal tags dictionary and if not found
     * then search in the repeating tags list.
     */
    const DcmDictEntry* e = NULL;

    e = hashDict.get(key, privCreator);
    if (e == NULL) {
        /* search in the repeating tags dictionary */
        OFBool found = OFFalse;
        DcmDictEntryListConstIterator iter(repDict.begin());
        DcmDictEntryListConstIterator last(repDict.end());
        for (; !found && iter != last; ++iter) {
            if ((*iter)->contains(key, privCreator)) {
                found = OFTrue;
                e = *iter;
            }
        }
    }
    return e;
}

const DcmDictEntry*
DcmDataDictionary::findEntry(const char *name) const
{
    const DcmDictEntry* e = NULL;
    const DcmDictEntry* ePrivate = NULL;

    /* search first in the normal tags dictionary and if not found
     * then search in the repeating tags list.
     */
    DcmHashDictIterator iter;
    for (iter = hashDict.begin(); (e == NULL) && (iter != hashDict.end()); ++iter) {
        if ((*iter)->contains(name)) {
            e = *iter;
            if (e->getGroup() % 2)
            {
                /* tag is a private tag - continue search to be sure to find non-private keys first */
                if (!ePrivate) ePrivate = e;
                e = NULL;
            }
        }
    }

    if (e == NULL) {
        /* search in the repeating tags dictionary */
        OFBool found = OFFalse;
        DcmDictEntryListConstIterator iter2(repDict.begin());
        DcmDictEntryListConstIterator last(repDict.end());
        for (; !found && iter2 != last; ++iter2) {
            if ((*iter2)->contains(name)) {
                found = OFTrue;
                e = *iter2;
            }
        }
    }

    if (e == NULL && ePrivate != NULL) {
        /* no standard key found - use the first private key found */
        e = ePrivate;
    }

    return e;
}


/* ================================================================== */


GlobalDcmDataDictionary::GlobalDcmDataDictionary()
  : dataDict(NULL)
#ifdef WITH_THREADS
  , dataDictLock()
#endif
{
}

GlobalDcmDataDictionary::~GlobalDcmDataDictionary()
{
  /* No threads may be active any more, so no locking needed */
  delete dataDict;
}

void GlobalDcmDataDictionary::createDataDict()
{
  /* Make sure only one thread tries to initialize the dictionary */
#ifdef WITH_THREADS
  dataDictLock.wrlock();
#endif

#ifdef DONT_LOAD_EXTERNAL_DICTIONARIES
    #error "The macro DONT_LOAD_EXTERNAL_DICTIONARIES has been defined in older versions of DCMTK. Undefine ENABLE_EXTERNAL_DICTIONARY instead."
#endif

#ifdef ENABLE_EXTERNAL_DICTIONARY
  const OFBool loadExternal = OFTrue;
#else
  const OFBool loadExternal = OFFalse;
#endif
  /* Make sure no other thread managed to create the dictionary
   * before we got our write lock. */
  if (!dataDict)
    dataDict = new DcmDataDictionary(OFTrue /*loadBuiltin*/, loadExternal);
#ifdef WITH_THREADS
  dataDictLock.wrunlock();
#endif
}

const DcmDataDictionary& GlobalDcmDataDictionary::rdlock()
{
#ifdef WITH_THREADS
  dataDictLock.rdlock();
#endif
  if (!dataDict)
  {
    /* dataDictLock must not be locked during createDataDict() */
#ifdef WITH_THREADS
    dataDictLock.rdunlock();
#endif
    createDataDict();
#ifdef WITH_THREADS
    dataDictLock.rdlock();
#endif
  }
  return *dataDict;
}

DcmDataDictionary& GlobalDcmDataDictionary::wrlock()
{
#ifdef WITH_THREADS
  dataDictLock.wrlock();
#endif
  if (!dataDict)
  {
    /* dataDictLock must not be locked during createDataDict() */
#ifdef WITH_THREADS
    dataDictLock.wrunlock();
#endif
    createDataDict();
#ifdef WITH_THREADS
    dataDictLock.wrlock();
#endif
  }
  return *dataDict;
}

void GlobalDcmDataDictionary::rdunlock()
{
#ifdef WITH_THREADS
  dataDictLock.rdunlock();
#endif
}

void GlobalDcmDataDictionary::wrunlock()
{
#ifdef WITH_THREADS
  dataDictLock.wrunlock();
#endif
}

OFBool GlobalDcmDataDictionary::isDictionaryLoaded()
{
  OFBool result = rdlock().isDictionaryLoaded();
  rdunlock();
  return result;
}

void GlobalDcmDataDictionary::clear()
{
  wrlock().clear();
  wrunlock();
}
