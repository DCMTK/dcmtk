/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface for loadable DICOM data dictionary
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:24:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdict.h,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMDICT_H
#define DCMDICT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dchashdi.h"

/*
 * The DICOM Data Dictionary
 */

#define DCM_MAXDICTLINESIZE     2048
#define DCM_MAXDICTFIELDS       6

#define DCM_DICT_ENVIRONMENT_VARIABLE   "DCMDICTPATH"

#ifndef DCM_DICT_DEFAULT_PATH
/* 
** The default dictionary path is system dependent.  It should
** be defined in a configuration file included from "osconfig.h"
*/
#error "DCM_DICT_DEFAULT_PATH is not defined via osconfig.h"
#endif /* !DCM_DICT_DEFAULT_PATH */

#ifndef ENVIRONMENT_PATH_SEPARATOR
#define ENVIRONMENT_PATH_SEPARATOR '\n' /* at least define something unlikely */
#endif


class DcmDataDictionary {
private:
    DcmHashDict hashDict;    /* dictionary of normal tags */
    DcmDictEntryList repDict; /* dictionary of repeating tags */
    int skeletonCount; /* the number of skeleton entries */
    OFBool dictionaryLoaded; /* is a dictionary loaded (more than skeleton) */

 // --- declarations to avoid compiler warnings
 
    DcmDataDictionary &operator=(const DcmDataDictionary &);
    DcmDataDictionary(const DcmDataDictionary &);

protected:
    /* Load external dictionaries defined via environment variables */
    OFBool loadExternalDictionaries();

    /* Load a builtin (compiled) data dictionary.
    ** Depending on which code is in use, this function may not
    ** do anything.
    */
    void loadBuiltinDictionary();

    /* Load skeleton dictionary (the bare minimum needed to run) */
    OFBool loadSkeletonDictionary();


    const DcmDictEntry* findEntry(const DcmDictEntry& entry);
    void deleteEntry(const DcmDictEntry& entry);

public:
    /* constructor/destructor */
    DcmDataDictionary(OFBool loadBuiltin=OFFalse, OFBool loadExternal=OFFalse);
    ~DcmDataDictionary();

    /* is a data dictionary loaded (excluding the skeleton dictionary) */
    OFBool isDictionaryLoaded() { return dictionaryLoaded; }

    /* the number of normal/repeating tag entries  */
    int numberOfNormalTagEntries() { return hashDict.size(); }
    int numberOfRepeatingTagEntries() { return repDict.size(); }

    /* total number of dictionary entries  */
    int numberOfEntries() 
        { return numberOfNormalTagEntries() 
              + numberOfRepeatingTagEntries() - skeletonCount; }

    /* the number of skeleton entries */
    int numberOfSkeletonEntries() { return skeletonCount; }

    /*
     * Load a particular dictionary from file.
     * Returns OFFalse if file cannot be opened (only if errorIfAbsent==OFTrue)
     * Returns OFFalse if formatting error encountered in file
     */
    OFBool loadDictionary(const char* fileName, OFBool errorIfAbsent=OFTrue);
    
    /* 
     * Dictionary lookups are performed by looking for a key. 
     * First the normal tag dictionary is searched.  If not found
     * then the repeating tag dictionary is searched.
     */
    const DcmDictEntry* findEntry(const DcmTagKey& key);
    const DcmDictEntry* findEntry(const char *name); /* by name also */

    void clear(); /* delete all entries */

    /*
     * Add an entry to the dictionary.  Must be allocated via new.
     * The entry becomes the property of the dictionary and will be
     * deallocated (via delete) upon clear() or dictionary destruction.
     * If an equivalent entry already exists it will be replaced by
     * the new entry and the old entry deallocated (via delete).
     */
    void addEntry(DcmDictEntry* entry);

    /* Iterators to access the normal and the repeating entries */ 
    DcmHashDictIterator normalBegin() { return hashDict.begin(); }
    DcmHashDictIterator normalEnd() { return hashDict.end(); }
    DcmDictEntryListIterator repeatingBegin() { return repDict.begin(); }
    DcmDictEntryListIterator repeatingEnd() { return repDict.end(); }

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
** Revision 1.13  1999-03-31 09:24:35  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1998/07/15 15:48:45  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.11  1997/08/26 14:02:56  hewett
** New data structures for data-dictionary.  The main part of the
** data-dictionary is now stored in an hash table using an optimized
** hash function.  This new data structure reduces data-dictionary
** load times by a factor of 4!  he data-dictionary specific linked-list
** has been replaced by a linked list derived from OFList class
** (see ofstd/include/oflist.h).
** The only interface modifications are related to iterating over the entire
** data dictionary which should not be needed by "normal" applications.
**
** Revision 1.10  1997/07/21 08:25:07  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/22 13:15:54  hewett
** Added method DcmDataDictionary::isDictionaryLoaded() to ask if a full
** data dictionary has been loaded.  This method should be used in tests
** rather that querying the number of entries (a sekelton dictionary is
** now always present).
**
** Revision 1.8  1997/05/13 13:58:41  hewett
** Added member function (loadSkeletomDictionary) to preload of a few
** essential attribute descriptions into the data dictionary (e.g. Item
** and ItemDelimitation tags).
**
** Revision 1.7  1996/09/18 16:37:10  hewett
** Added capability to search data dictionary by tag name.  The
** source code for these changes was contributed by Larry V. Streepy,
** Jr., Chief Technical Officer,  Healthcare Communications, Inc.,
** (mailto:streepy@healthcare.com).
**
** Revision 1.6  1996/03/22 13:09:12  hewett
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
*/

