/*
 *
 *  Copyright (C) 1994-2015, OFFIS e.V.
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
class DCMTK_DCMDATA_EXPORT DcmDataDictionary
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


/** global singleton dicom dictionary that is used by DCMTK in order to lookup
 *  attribute VR, tag names and so on.  The dictionary is internally populated
 *  on first use, if the user accesses it via rdlock() or wrlock().  The
 *  dictionary allows safe read (shared) and write (exclusive) access from
 *  multiple threads in parallel.
 */
class DCMTK_DCMDATA_EXPORT GlobalDcmDataDictionary
{
public:
  /** constructor.
   */
  GlobalDcmDataDictionary();

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

  /** create the data dictionary instance for this class. Used for first
   * intialization.  The caller must not have dataDictLock locked.
   */
  void createDataDict();

  /** the data dictionary managed by this class
   */
  DcmDataDictionary *dataDict;

#ifdef WITH_THREADS
  /** the read/write lock used to protect access from multiple threads
   */
  OFReadWriteLock dataDictLock;
#endif
};


/** The Global DICOM Data Dictionary.
 *  Will be created before main() starts and gets populated on its first use.
 *  Tries to load a builtin data dictionary (if compiled in).
 *  Tries to load data dictionaries from files specified by
 *  the DCMDICTPATH environment variable.  If this environment
 *  variable does not exist then a default file is loaded (if
 *  it exists).
 *  It is possible that no data dictionary gets loaded.  This
 *  is likely to cause unexpected behaviour in the dcmdata
 *  toolkit classes.
 */
extern DCMTK_DCMDATA_EXPORT GlobalDcmDataDictionary dcmDataDict;

#endif
