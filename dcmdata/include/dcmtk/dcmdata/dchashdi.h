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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-08-18 14:24:08 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCHASHDI_H
#define DCHASHDI_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstream.h"

class DcmDictEntry;
class DcmTagKey;
class DcmHashDict;

typedef OFListIterator(DcmDictEntry *) DcmDictEntryListIterator;
typedef OFListConstIterator(DcmDictEntry *) DcmDictEntryListConstIterator;

/** an ordered list of pointers to DcmDictEntry objects
 */
class DcmDictEntryList
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
class DcmHashDictIterator
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
class DcmHashDict
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


/*
** CVS/RCS Log:
** $Log: dchashdi.h,v $
** Revision 1.25  2011-08-18 14:24:08  joergr
** Fixed API documentation (required after last commit).
**
** Revision 1.24  2011-08-17 14:45:22  uli
** Improved hashing function for tags (less code, handles private creator).
**
** Revision 1.23  2011-02-04 11:14:38  uli
** Stop inheriting from OFList and OFListIterator.
**
** Revision 1.22  2010-11-10 12:04:06  uli
** Corrected DcmHashDictIterator::operator!=. Previously it ignored hindex.
**
** Revision 1.21  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.20  2009-01-05 14:14:14  joergr
** Fixed bug in DcmHashDictIterator::operator!=() introduced with last commit.
** Reverted to old implementation.
**
** Revision 1.19  2008-12-19 14:57:59  joergr
** Fixed bug in DcmHashDictIterator::operator!=() - wrong comparison operand.
**
** Revision 1.18  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.17  2005/12/08 16:28:14  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.16  2003/07/03 15:38:10  meichel
** Introduced DcmDictEntryListConstIterator, needed for compiling with HAVE_STL.
**
** Revision 1.15  2003/06/12 13:35:04  joergr
** Fixed inconsistent API documentation reported by Doxygen.
**
** Revision 1.14  2003/06/03 10:26:17  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.13  2003/06/02 17:03:58  meichel
** Added typedef needed by MSVC5 when compiling with STL support
**
** Revision 1.12  2003/03/21 13:06:46  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.11  2002/07/23 14:21:26  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.10  2001/06/01 15:48:40  meichel
** Updated copyright header
**
** Revision 1.9  2000/05/03 14:19:08  meichel
** Added new class GlobalDcmDataDictionary which implements read/write lock
**   semantics for safe access to the DICOM dictionary from multiple threads
**   in parallel. The global dcmDataDict now uses this class.
**
** Revision 1.8  2000/03/08 16:26:15  meichel
** Updated copyright header.
**
** Revision 1.7  1999/03/31 09:24:39  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/07/15 15:48:48  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.5  1997/09/18 11:41:13  meichel
** Corrected forward and friend declarations (needed for DEC cxx).
**
** Revision 1.4  1997/09/18 07:24:07  meichel
** Missing operator= added to class DcmDictEntryListIterator
**
** Revision 1.3  1997/08/29 13:11:09  andreas
** Corrected copy constructor for DcmHashDictIterator
**
** Revision 1.2  1997/08/26 13:41:11  hewett
** Corrected a couple of minor spelling errors.
**
** Revision 1.1  1997/08/26 13:30:29  hewett
** Initial Version - Interface for hash table data structure for data dictionary.
**
*/
