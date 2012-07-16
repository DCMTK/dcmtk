/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Purpose: Hash table interface for DICOM data dictionary
 *
 */

#ifndef DCHASHDI_H
#define DCHASHDI_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcdefine.h"

class DcmDictEntry;
class DcmTagKey;
class DcmHashDict;

typedef OFListIterator(DcmDictEntry *) DcmDictEntryListIterator;
typedef OFListConstIterator(DcmDictEntry *) DcmDictEntryListConstIterator;

/** an ordered list of pointers to DcmDictEntry objects
 */
class DCMTK_DCMDATA_EXPORT DcmDictEntryList
{
public:

    /// constructor
    DcmDictEntryList() : list_() {}

    /// destructor
    ~DcmDictEntryList();

    /// clears list and deletes all entries
    void clear();

    /// @return the number of elements in this list
    unsigned int size() const;

    /// @return True if this list is empty
    OFBool empty() const;

    /// @return Iterator to the beginning of the list
    DcmDictEntryListIterator begin();

    /// @return Iterator past the end of the list
    DcmDictEntryListIterator end();

    /// @return Iterator to the beginning of the list
    DcmDictEntryListConstIterator begin() const;

    /// @return Iterator past the end of the list
    DcmDictEntryListConstIterator end() const;

    /** Insert a new element in the list
     *  @param position Position to insert after
     *  @param entry Entry to insert
     *  @return Iterator to the new entry
     */
    DcmDictEntryListIterator insert(DcmDictEntryListIterator position, DcmDictEntry *entry);

    /** Remove an element from the list.
     *  @param entry The entry to remove
     */
    void remove(DcmDictEntry *entry);

    /** Append a new entry to the list.
     *  @param entry The entry to append.
     */
    void push_back(DcmDictEntry *entry);

    /** inserts an entry into the list and returns any replaced entry
     *  @param entry new list entry
     *  @return replaced list entry or NULL
     */
    DcmDictEntry* insertAndReplace(DcmDictEntry* entry);

    /** find an entry in the set
     *  @param key tag key of the entry to be searched for
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to entry (if found), otherwise NULL
     */
    DcmDictEntry *find(const DcmTagKey& key, const char *privCreator);

private:

    /// private undefined copy constructor
    DcmDictEntryList(const DcmDictEntryList&);

    /// private undefined copy assignment operator
    DcmDictEntryList& operator=(const DcmDictEntryList&);

    /// list of entries
    OFList<DcmDictEntry *> list_;
};


/** iterator class for traversing a DcmHashDict
 */
class DCMTK_DCMDATA_EXPORT DcmHashDictIterator
{
public:

    /// default constructor
    DcmHashDictIterator()
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
        { init(NULL); }

    /** constructor, creates iterator to existing hash dictionary
     *  @param d pointer to dictionary
     *  @param atEnd if true, iterator points after last element
     *   of hash dictionary, otherwise iterator points to first element
     */
    DcmHashDictIterator(const DcmHashDict* d, OFBool atEnd = OFFalse)
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
        { init(d, atEnd); }

    /// copy constructor
    DcmHashDictIterator(const DcmHashDictIterator& i)
      : dict(i.dict), hindex(i.hindex), iterating(i.iterating), iter(i.iter)
        { }

    /// copy assignment operator
    DcmHashDictIterator& operator=(const DcmHashDictIterator& i)
        { dict = i.dict; hindex = i.hindex;
          iterating = i.iterating; iter = i.iter; return *this; }

    /// comparison equality
    OFBool operator==(const DcmHashDictIterator& x) const
        { return (hindex == x.hindex) && (iter == x.iter); }

    /// comparison non-equality
    OFBool operator!=(const DcmHashDictIterator& x) const
        { return !(*this == x); }

    /// dereferencing of iterator
    const DcmDictEntry* operator*() const
        { return (*iter); }

    /// pre-increment operator
    DcmHashDictIterator& operator++()
        { stepUp(); return *this; }

    /// post-increment operator
    DcmHashDictIterator operator++(int)
        { DcmHashDictIterator tmp(*this); stepUp(); return tmp; }

private:

    /** initializes the iterator
     *  @param d pointer to hash dictionary, may be NULL
     *  @param atEnd if true, iterator points after last element
     *   of hash dictionary, otherwise iterator points to first element
     */
    void init(const DcmHashDict *d, OFBool atEnd = OFFalse);

    /** implements increment operator on hash dictionary
     */
    void stepUp();

    /// pointer to the hash dictionary this iterator traverses
    const DcmHashDict* dict;

    /// index of current bucket
    int hindex;

    /// flag indicating if iter is currently valid
    OFBool iterating;

    /// iterator for traversing a bucket in the hash table
    DcmDictEntryListIterator iter;
};


/** a hash table of pointers to DcmDictEntry objects
 */
class DCMTK_DCMDATA_EXPORT DcmHashDict
{

public:
    /// default constructor
    DcmHashDict()
     : hashTab(NULL), lowestBucket(0), highestBucket(0), entryCount(0)
        { _init(); }

    /// destructor
    ~DcmHashDict();

    /** counts total number of entries
     *  @return number of entries
     */
    int size() const { return entryCount; }

    /// clears the hash table of all entries
    void clear();

    /** inserts an entry into hash table (deletes old entry if present)
     *  @param entry pointer to new entry
     */
    void put(DcmDictEntry* entry);

    /** hash table lookup for the given tag key and private creator name
     *  @param key tag key of the entry to be searched for
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to entry (if found), otherwise NULL
     */
    const DcmDictEntry* get(const DcmTagKey& key, const char *privCreator) const;

    /** deletes the entry for the given tag and private creator
     *  @param key tag key of the entry to be deleted
     *  @param privCreator private creator name, may be NULL
     */
    void del(const DcmTagKey& key, const char *privCreator);

    // iterator over the contents of the hash table
    friend class DcmHashDictIterator;

    /// returns iterator to start of hash table
    DcmHashDictIterator begin() const
        { DcmHashDictIterator iter(this); return iter; }

    /// returns iterator to end of hash table
    DcmHashDictIterator end() const
        { DcmHashDictIterator iter(this, OFTrue); return iter; }

    /// prints some information about hash table bucket utilization
    STD_NAMESPACE ostream& loadSummary(STD_NAMESPACE ostream& out);

private:

    /// private unimplemented copy constructor
    DcmHashDict(const DcmHashDict &);

    /// private unimplemented copy assignment operator
    DcmHashDict &operator=(const DcmHashDict &);

    /// performs initialization for given hash table size, called from constructor
    void _init();

    /** compute hash value for given tag key
     *  @param key pointer to tag key
     *  @param privCreator private creator name, may be NULL
     *  @return hash value
     */
    int hash(const DcmTagKey* key, const char *privCreator) const;

    /** inserts new entry into given list
     *  @param lst list to add to
     *  @param entry new element to add, will be deleted upon destruction of the hash table
     *  @return pointer to replaced element, if any
     */
    DcmDictEntry* insertInList(DcmDictEntryList& lst, DcmDictEntry* entry);

    /** removes the entry for the given tag and private creator
     *  @param lst list to remove from
     *  @param key tag key of the entry to be removed
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to removed element, if any
     */
    DcmDictEntry* removeInList(DcmDictEntryList& lst, const DcmTagKey& key, const char *privCreator);

    /** searches entry for the given tag and private creator
     *  @param lst list to search in
     *  @param key tag key of the entry to be searched for
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to found element, NULL if not found
     */
    DcmDictEntry* findInList(DcmDictEntryList& lst, const DcmTagKey& key, const char *privCreator) const;

    /** array of (hash table size) pointers to DcmDictEntryList elements
     *  implementing the different buckets of the hash table
     */
    DcmDictEntryList** hashTab;

    /// number of buckets in hash table
    static const int hashTabLength;

    /// index of lowest bucket for which the DcmDictEntryList has been initialized
    int lowestBucket;

    /// index of highest bucket for which the DcmDictEntryList has been initialized
    int highestBucket;

    /// number of entries in hash table
    int entryCount;

};

#endif /* DCHASHDI_H */
