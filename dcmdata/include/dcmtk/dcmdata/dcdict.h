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
 *  Purpose: Interface for loadable DICOM data dictionary
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:40 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCMDICT_H
#define DCMDICT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/dcmdata/dchashdi.h"

/// maximum length of a line in the loadable DICOM dictionary
#define DCM_MAXDICTLINESIZE     2048

/// maximum number of fields per entry in the loadable DICOM dictionary
#define DCM_MAXDICTFIELDS       6

/// environment variable pointing to the data dictionary file
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


/** this class implements a loadable DICOM Data Dictionary
 */
class DcmDataDictionary
{
public:

    /** constructor
     *  @param loadBuiltin flag indicating if a built-in data dictionary
     *    (if any) should be loaded.
     *  @param loadExternal flag indicating if an external data dictionary
     *    should be read from file.
     */
    DcmDataDictionary(OFBool loadBuiltin, OFBool loadExternal);

    /// destructor
    ~DcmDataDictionary();

    /** checks if a data dictionary is loaded (excluding the skeleton dictionary)
     *  @return true if loaded, false if no dictionary is present
     */
    OFBool isDictionaryLoaded() const { return dictionaryLoaded; }

    /// returns the number of normal (non-repeating) tag entries
    int numberOfNormalTagEntries() const { return hashDict.size(); }

    /// returns the number of repeating tag entries
    int numberOfRepeatingTagEntries() const { return OFstatic_cast(int, repDict.size()); }

    /** returns the number of dictionary entries that were loaded
     *  either from file or from a built-in dictionary or both.
     */
    int numberOfEntries() const
        { return numberOfNormalTagEntries()
              + numberOfRepeatingTagEntries() - skeletonCount; }

    /** returns the number of skeleton entries. The skeleton is a collection
     *  of dictionary entries which are always present, even if neither internal
     *  nor external dictionary have been loaded. It contains very basic
     *  things like item delimitation and sequence delimitation.
     */
    int numberOfSkeletonEntries() const { return skeletonCount; }

    /** reload data dictionaries. First, all dictionary entries are deleted.
     *  @param loadBuiltin flag indicating if a built-in data dictionary
     *    (if any) should be loaded.
     *  @param loadExternal flag indicating if an external data dictionary
     *    should be read from file.
     *  @return true if reload was successful, false if an error occurred
     */
    OFBool reloadDictionaries(OFBool loadBuiltin, OFBool loadExternal);

    /** load a particular dictionary from file.
     *  @param fileName filename
     *  @param errorIfAbsent causes the method to return false
     *     if the file cannot be opened
     *  @return false if the file contains a parse error or if the file could
     *     not be opened and errorIfAbsent was set, true otherwise.
     */
    OFBool loadDictionary(const char* fileName, OFBool errorIfAbsent = OFTrue);

    /** dictionary lookup for the given tag key and private creator name.
     *  First the normal tag dictionary is searched.  If not found
     *  then the repeating tag dictionary is searched.
     *  @param key tag key
     *  @param privCreator private creator name, may be NULL
     */
    const DcmDictEntry* findEntry(const DcmTagKey& key, const char *privCreator) const;

    /** dictionary lookup for the given attribute name.
     *  First the normal tag dictionary is searched.  If not found
     *  then the repeating tag dictionary is searched.
     *  Only considers standard attributes (i. e. without private creator)
     *  @param name attribute name
     */
    const DcmDictEntry* findEntry(const char *name) const;

    /// deletes all dictionary entries
    void clear();

    /** adds an entry to the dictionary.  Must be allocated via new.
     *  The entry becomes the property of the dictionary and will be
     *  deallocated (via delete) upon clear() or dictionary destruction.
     *  If an equivalent entry already exists it will be replaced by
     *  the new entry and the old entry deallocated (via delete).
     *  @param entry pointer to new entry
     */
    void addEntry(DcmDictEntry* entry);

    /* Iterators to access the normal and the repeating entries */

    /// returns an iterator to the start of the normal (non-repeating) dictionary
    DcmHashDictIterator normalBegin() { return hashDict.begin(); }

    /// returns an iterator to the end of the normal (non-repeating) dictionary
    DcmHashDictIterator normalEnd() { return hashDict.end(); }

    /// returns an iterator to the start of the repeating tag dictionary
    DcmDictEntryListIterator repeatingBegin() { return repDict.begin(); }

    /// returns an iterator to the end of the repeating tag dictionary
    DcmDictEntryListIterator repeatingEnd() { return repDict.end(); }

private:

    /** private undefined assignment operator
     */
    DcmDataDictionary &operator=(const DcmDataDictionary &);

    /** private undefined copy constructor
     */
    DcmDataDictionary(const DcmDataDictionary &);

    /** loads external dictionaries defined via environment variables
     *  @return true if successful
     */
    OFBool loadExternalDictionaries();

    /** loads a builtin (compiled) data dictionary.
     *  Depending on which code is in use, this function may not
     *  do anything.
     */
    void loadBuiltinDictionary();

    /** loads the skeleton dictionary (the bare minimum needed to run)
     *  @return true if successful
     */
    OFBool loadSkeletonDictionary();

    /** looks up the given directory entry in the two dictionaries.
     *  @return pointer to entry if found, NULL otherwise
     */
    const DcmDictEntry* findEntry(const DcmDictEntry& entry) const;

    /** deletes the given entry from either dictionary
     */
    void deleteEntry(const DcmDictEntry& entry);


    /** dictionary of normal tags
     */
    DcmHashDict hashDict;

    /** dictionary of repeating tags
     */
    DcmDictEntryList repDict;

    /** the number of skeleton entries
     */
    int skeletonCount;

    /** is a dictionary loaded (more than skeleton)
     */
    OFBool dictionaryLoaded;

};


/** encapsulates a data dictionary with access methods which allow safe
 *  read and write access from multiple threads in parallel.
 *  A read/write lock is used to protect threads from each other.
 *  This allows parallel read-only access by multiple threads, which is
 *  the most common case.
 */
class GlobalDcmDataDictionary
{
public:
  /** constructor.
   *  @param loadBuiltin if true, the dictionary constructor calls loadBuiltinDictionary().
   *  @param loadExternal if true, the dictionary constructor calls loadExternalDictionaries().
   */
  GlobalDcmDataDictionary(OFBool loadBuiltin, OFBool loadExternal);

  /** destructor
   */
  ~GlobalDcmDataDictionary();

  /** acquires a read lock and returns a const reference to
   *  the dictionary.
   *  @return const reference to dictionary
   */
  const DcmDataDictionary& rdlock();

  /** acquires a write lock and returns a non-const reference
   *  to the dictionary.
   *  @return non-const reference to dictionary.
   */
  DcmDataDictionary& wrlock();

  /** unlocks the read or write lock which must have been acquired previously.
   */
  void unlock();

  /** checks if a data dictionary has been loaded. This method acquires and
   *  releases a read lock. It must not be called with another lock on the
   *  dictionary being held by the calling thread.
   *  @return OFTrue if dictionary has been loaded, OFFalse otherwise.
   */
  OFBool isDictionaryLoaded();

  /** erases the contents of the dictionary. This method acquires and
   *  releases a write lock. It must not be called with another lock on the
   *  dictionary being held by the calling thread.  This method is intended
   *  as a help for debugging memory leaks.
   */
  void clear();

private:
  /** private undefined assignment operator
   */
  GlobalDcmDataDictionary &operator=(const GlobalDcmDataDictionary &);

  /** private undefined copy constructor
   */
  GlobalDcmDataDictionary(const GlobalDcmDataDictionary &);

  /** the data dictionary managed by this class
   */
  DcmDataDictionary dataDict;

#ifdef WITH_THREADS
  /** the read/write lock used to protect access from multiple threads
   */
  OFReadWriteLock dataDictLock;
#endif
};


/** The Global DICOM Data Dictionary.
 *  Will be created before main() starts.
 *  Tries to load a builtin data dictionary (if compiled in).
 *  Tries to load data dictionaries from files specified by
 *  the DCMDICTPATH environment variable.  If this environment
 *  variable does not exist then a default file is loaded (if
 *  it exists).
 *  It is possible that no data dictionary gets loaded.  This
 *  is likely to cause unexpected behaviour in the dcmdata
 *  toolkit classes.
 */
extern GlobalDcmDataDictionary dcmDataDict;

#endif


/*
** CVS/RCS Log:
** $Log: dcdict.h,v $
** Revision 1.24  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.23  2010-10-04 14:44:39  joergr
** Replaced "#ifdef _REENTRANT" by "#ifdef WITH_THREADS" where appropriate (i.e.
** in all cases where OFMutex, OFReadWriteLock, etc. are used).
**
** Revision 1.22  2009-02-05 13:13:51  joergr
** Added reload method to data dictionary class.
**
** Revision 1.21  2008-08-15 09:27:14  meichel
** Added type cast to fix a warning
**
** Revision 1.20  2005/12/08 16:28:09  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.19  2004/01/16 14:07:27  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.18  2002/07/23 14:21:25  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.17  2002/02/27 14:21:20  meichel
** Declare dcmdata read/write locks only when compiled in multi-thread mode
**
** Revision 1.16  2001/06/01 15:48:38  meichel
** Updated copyright header
**
** Revision 1.15  2000/05/03 14:19:08  meichel
** Added new class GlobalDcmDataDictionary which implements read/write lock
**   semantics for safe access to the DICOM dictionary from multiple threads
**   in parallel. The global dcmDataDict now uses this class.
**
** Revision 1.14  2000/03/08 16:26:13  meichel
** Updated copyright header.
**
** Revision 1.13  1999/03/31 09:24:35  meichel
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
** Added capability to search data dictionary by tag name.
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
