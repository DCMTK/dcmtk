
#ifndef DCMDICT_H
#define DCMDICT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcdicent.h"
#include "dcentbst.h"


/*
 * The DICOM Data Dictionary
 */

#define DCM_MAXDICTLINESIZE	2048
#define DCM_MAXDICTFIELDS	6

#define DCM_DICT_ENVIRONMENT_VARIABLE	"DCMDICTPATH"

#ifndef DCM_DICT_DEFAULT_PATH
#if defined(unix)
#define DCM_DICT_DEFAULT_PATH	"/usr/local/dicom/lib/dicom.dic"
#elif defined(dos) || defined(win31)
#define DCM_DICT_DEFAULT_PATH	"\\dicom.dic"
#elif defined(macintosh)
#define DCM_DICT_DEFAULT_PATH	"dicom.dic"
#else
#error "Don't know how to define DCM_DICT_DEFAULT_PATH"
#endif
#endif /* !DCM_DICT_DEFAULT_PATH */

#ifndef ENVIRONMENT_PATH_SEPARATOR
#define ENVIRONMENT_PATH_SEPARATOR '\n'	/* at least define something unlikely */
#endif


class DcmDataDictionary {
private:
    DcmDictEntryPtrBSTSet dict;    /* dictionary of normal tags */
    DcmDictEntryPtrBSTSet repElementDict; /* dictionary of repeating element tags */
    DcmDictEntryPtrBSTSet repGroupDict; /* dictionary of repeating group tags */

protected:
    /* Load external dictionaries defined via environment variables */
    BOOL loadExternalDictionaries();

    /* Load a builtin (compiled) data dictionary.
    ** Depending on which code is in use, this function may not
    ** do anything.
    */
    void loadBuiltinDictionary();

    const DcmDictEntry* findEntry(const DcmDictEntry& entry);
    void deleteEntry(const DcmDictEntry& entry);

public:
    /* constructor/destructor */
    DcmDataDictionary(BOOL loadBuiltin=FALSE, BOOL loadExternal=FALSE);
    ~DcmDataDictionary();

    /* the number of entries in the complete dictionary */
    int numberOfEntries() { 
	return dict.length() + repElementDict.length() + 
	    repGroupDict.length(); }

    /*
     * Load a particular dictionary from file.
     * Returns FALSE if file cannot be opened (only if errorIfAbsent==TRUE)
     * Returns FALSE if formatting error encountered in file
     */
    BOOL loadDictionary(const char* fileName, BOOL errorIfAbsent=TRUE);
    
    /* 
     * Dictionary lookups are performed by looking for a key. 
     * First the normal tag dictionary is searched.  If not found
     * then the repeating element dictionary is searched, then the
     * repeating group dictionary.
     */
    const DcmDictEntry* findEntry(const DcmTagKey& key);

    void clear(); /* delete all entries */

    void balance(); /* force a rebalance of the dictionary trees */

    /*
     * Add an entry to the dictionary.  Must be allocated via new.
     * The entry becomes the property of the dictionary and will be
     * deallocated (via delete) upon clear() or dictionary destruction.
     * If an equivalent entry already exists it will be replaced by
     * the new entry and the old entry deallocated (via delete).
     */
    void addEntry(DcmDictEntry* entry);
    
    /* 
     * For stepping through the normal tags of dictionary D use:
     * for (Pix p = D.first(); p != NULL; D.next(p)) 
     *     WHATEVER(D.contents(p)); 
     */
    Pix first() { return dict.first(); }
    void next(Pix& i) { dict.next(i); }
    const DcmDictEntry* contents(Pix i) { return dict(i); }

    /* 
     * For stepping through the repeating element tags of dictionary D use:
     * for (Pix p = D.repElementFirst(); p != NULL; D.repElementNext(p)) 
     *     WHATEVER(D.repElementContents(p)); 
     */
    Pix repElementFirst() { return repElementDict.first(); }
    void repElementNext(Pix& i) { repElementDict.next(i); }
    const DcmDictEntry* repElementContents(Pix i) { return repElementDict(i); }

    /*
     * For stepping through the repeating group tags of dictionary D use:
     * for (Pix p = D.repGroupFirst(); p != NULL; D.repGroupNext(p)) 
     *     WHATEVER(D.repGroupContents(p)); 
     */
    Pix repGroupFirst() { return repGroupDict.first(); }
    void repGroupNext(Pix& i) { repGroupDict.next(i); }
    const DcmDictEntry* repGroupContents(Pix i) { return repGroupDict(i); }
};


/*
** THE Global DICOM Data Dictionary.
**
** Will be created before main() starts.  
**
** Tries to load a builtin data dictionary (if compiled in)
** Tries to load data dictionaries from files specified by
** the DCMDICTPATH environment variable.  If this environment
** variable does not exist then a default file is loaded (if
** it exists).  
**
** It is possible that no data dictionary gets loaded.  This
** is likely to cause unexpected behaviour in the dcmdata
** toolkit classes.
*/

extern DcmDataDictionary dcmDataDict;

#endif
