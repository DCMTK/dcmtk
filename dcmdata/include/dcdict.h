/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcdict.h
**
** Purpose:
** Interface for loadable DICOM data dictionary
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-22 13:09:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdict.h,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCMDICT_H
#define DCMDICT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcdicent.h"
#include "dcentbst.h"
#include "dcentlst.h"


/*
 * The DICOM Data Dictionary
 */

#define DCM_MAXDICTLINESIZE	2048
#define DCM_MAXDICTFIELDS	6

#define DCM_DICT_ENVIRONMENT_VARIABLE	"DCMDICTPATH"

#ifndef DCM_DICT_DEFAULT_PATH
/* 
** The default dictionary path is system dependent.  It should
** be defined in a configuration file included from "osconfig.h"
*/
#error "DCM_DICT_DEFAULT_PATH is not defined via osconfig.h"
#endif /* !DCM_DICT_DEFAULT_PATH */

#ifndef ENVIRONMENT_PATH_SEPARATOR
#define ENVIRONMENT_PATH_SEPARATOR '\n'	/* at least define something unlikely */
#endif


class DcmDataDictionary {
private:
    DcmDictEntryPtrBSTSet dict;    /* dictionary of normal tags */
    DcmDictEntryList repDict; /* dictionary of repeating tags */

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

    /* the number of normal/repeating tag entries  */
    int numberOfNormalTagEntries() { return dict.length(); }
    int numberOfRepeatingTagEntries() { return repDict.length(); }

    /* total number of dictionary entries  */
    int numberOfEntries() 
	{ return numberOfNormalTagEntries() + numberOfRepeatingTagEntries(); }

    /*
     * Load a particular dictionary from file.
     * Returns FALSE if file cannot be opened (only if errorIfAbsent==TRUE)
     * Returns FALSE if formatting error encountered in file
     */
    BOOL loadDictionary(const char* fileName, BOOL errorIfAbsent=TRUE);
    
    /* 
     * Dictionary lookups are performed by looking for a key. 
     * First the normal tag dictionary is searched.  If not found
     * then the repeating tag dictionary is searched.
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
    Pix normalFirst() { return dict.first(); }
    void normalNext(Pix& i) { dict.next(i); }
    const DcmDictEntry* normalContents(Pix i) { return dict(i); }

    /*
     * For stepping through the repeating tags of dictionary D use:
     * for (Pix p = D.repFirst(); p != NULL; D.repNext(p)) 
     *     WHATEVER(D.repContents(p)); 
     */
    Pix repeatingFirst() { return repDict.first(); }
    void repeatingNext(Pix& i) { repDict.next(i); }
    const DcmDictEntry* repeatingContents(Pix i) { return repDict.contents(i); }

};


/*
** THE Global DICOM Data Dictionary.
**
** Will be created before main() starts.  
**
** Tries to load a builtin data dictionary (if compiled in).
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


/*
** CVS/RCS Log:
** $Log: dcdict.h,v $
** Revision 1.6  1996-03-22 13:09:12  hewett
** Moved the definition of DCM_DICT_DEFAULT_PATH to the system
** dependent configuration files included via "osconfig.h".
**
** Revision 1.5  1996/03/21 09:50:38  hewett
** Added a  method numberOfEntries() to return the total number of
** dictionary entries.
**
** Revision 1.4  1996/03/20 16:43:49  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
**
*/

