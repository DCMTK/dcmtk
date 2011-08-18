/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Update Date:      $Date: 2011-08-18 14:40:42 $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dchashdi.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dctypes.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"

/*
** DcmDictEntryList
*/

DcmDictEntryList::~DcmDictEntryList()
{
    clear();
}

void
DcmDictEntryList::clear()
{
    while (!empty()) {
        delete list_.front();
        list_.pop_front();
    }
}

DcmDictEntry*
DcmDictEntryList::insertAndReplace(DcmDictEntry* entry)
{
    if (empty()) {
        list_.push_front(entry);
    } else {
        DcmDictEntryListIterator iter(begin());
        DcmDictEntryListIterator last(end());
        Uint32 eHash = entry->hash();
        Uint32 iterHash = 0;
        // insert smallest first
        for (iter = begin(); iter != last; ++iter) {
            iterHash = (*iter)->hash();
            if (eHash == iterHash)
            {
                if (entry->privateCreatorMatch(**iter))
                {
                    // entry is already there so replace it
                    DcmDictEntry* oldEntry = *iter;
                    *iter = entry;
                    return oldEntry;
                }
                else
                {
                    // insert before listEntry
                    insert(iter, entry);
                    return NULL;
                }
            }
            else if (eHash < iterHash)
            {
                // insert before listEntry
                insert(iter, entry);
                return NULL;
            }
        }
        // add to end
        push_back(entry);
    }
    return NULL;
}


DcmDictEntry *DcmDictEntryList::find(const DcmTagKey& key, const char *privCreator)
{
    if (!empty()) {
        DcmDictEntryListIterator iter;
        DcmDictEntryListIterator last = end();
        Uint32 kHash = key.hash();
        Uint32 iterHash = 0;
        for (iter = begin(); iter != last; ++iter) {
            iterHash = (*iter)->hash();
            if ((iterHash == kHash) && (*iter)->privateCreatorMatch(privCreator))
            {
                return *iter;
            } else if (iterHash > kHash) {
                return NULL; // not there
            }
        }
    }
    return NULL;
}

unsigned int
DcmDictEntryList::size() const
{
    return list_.size();
}

OFBool
DcmDictEntryList::empty() const
{
    return list_.empty();
}

DcmDictEntryListIterator
DcmDictEntryList::begin()
{
    return list_.begin();
}

DcmDictEntryListIterator
DcmDictEntryList::end()
{
    return list_.end();
}

DcmDictEntryListConstIterator
DcmDictEntryList::begin() const
{
    return list_.begin();
}

DcmDictEntryListConstIterator
DcmDictEntryList::end() const
{
    return list_.end();
}

DcmDictEntryListIterator
DcmDictEntryList::insert(DcmDictEntryListIterator position, DcmDictEntry *entry)
{
    return list_.insert(position, entry);
}

void
DcmDictEntryList::remove(DcmDictEntry *entry)
{
    list_.remove(entry);
}

void
DcmDictEntryList::push_back(DcmDictEntry *entry)
{
    list_.push_back(entry);
}

/*
** DcmHashDictIterator
*/

void
DcmHashDictIterator::init(const DcmHashDict* d, OFBool atEnd)
{
    dict = d;
    hindex = 0;
    iterating = OFFalse;
    if (dict != NULL) {
        if (atEnd || dict->size() == 0) {
            hindex = dict->highestBucket;
            if (dict->size() > 0) {
                iter = dict->hashTab[hindex]->end();
                iterating = OFTrue;
            }
        } else {
            hindex = dict->lowestBucket;
            iter = dict->hashTab[hindex]->begin();
            iterating = OFTrue;
        }
    }
}

void
DcmHashDictIterator::stepUp()
{
    assert(dict != NULL);

    while (hindex <= dict->highestBucket) {
        DcmDictEntryList* bucket = dict->hashTab[hindex];
        if (bucket == NULL) {
            if (hindex == dict->highestBucket)
                return; /* We reached the end of the dictionary */
            hindex++; // move on to next bucket
            iterating = OFFalse;
        } else {
            if (!iterating) {
                iter = bucket->begin();
                iterating = OFTrue;
                if (iter != bucket->end()) {
                    return; /* we have found the next one */
                }
            }
            if (iter == bucket->end()) {
                if (hindex == dict->highestBucket)
                    return; /* We reached the end of the dictionary */
                iterating = OFFalse;
                hindex++;
            } else {
                ++iter;
                if (iter != bucket->end()) {
                    return; /* we have found the next one */
                }
            }
        }
    }
}


/*
** DcmHashDict
*/

// This number shouldn't have any small factors, so that
// DcmHashDict::hash() produces fewer collisions.
const int DcmHashDict::hashTabLength = 2011;

void
DcmHashDict::_init()
{
    hashTab = new DcmDictEntryList*[hashTabLength];
    assert(hashTab != NULL);
    for (int i=0; i<hashTabLength; i++) {
        hashTab[i] = NULL;
    }
    lowestBucket = hashTabLength-1;
    highestBucket = 0;
    entryCount = 0;
}


DcmHashDict::~DcmHashDict()
{
    clear();
    delete[] hashTab;
}

void
DcmHashDict::clear()
{
    for (int i=0; i<hashTabLength; i++) {
        delete hashTab[i];
        hashTab[i] = NULL;
    }
    lowestBucket = hashTabLength-1;
    highestBucket = 0;
    entryCount = 0;
}

int
DcmHashDict::hash(const DcmTagKey* key, const char *privCreator) const
{
    int i = 0;
    Uint32 h = key->hash();

    // If there is a private creator, hash that in, too
    for (; privCreator != NULL && *privCreator != '\0'; privCreator++) {
        h ^= *privCreator << ((++i & 3) << 3);
    }

    // This 'hash function' only works well if hashTabLength is prime
    int res = h % hashTabLength;

    assert((res >= 0) && (res < hashTabLength));

    return res;
}

DcmDictEntry*
DcmHashDict::insertInList(DcmDictEntryList& l, DcmDictEntry* entry)
{
    return l.insertAndReplace(entry);
}

void
DcmHashDict::put(DcmDictEntry* entry)
{
    int idx = hash(entry, entry->getPrivateCreator());

    DcmDictEntryList* bucket = hashTab[idx];
    // if there is no bucket then create one
    if (bucket == NULL) {
        bucket = new DcmDictEntryList;
        assert(bucket != NULL);
        hashTab[idx] = bucket;
    }

    DcmDictEntry* old = insertInList(*bucket, entry);
    if (old != NULL) {
        /* an old entry has been replaced */
#ifdef PRINT_REPLACED_DICTIONARY_ENTRIES
        DCMDATA_WARN("replacing " << *old);
#endif
        delete old;
    } else {
        entryCount++;
    }

    lowestBucket = (lowestBucket<idx)?(lowestBucket):(idx);
    highestBucket = (highestBucket>idx)?(highestBucket):(idx);
}

DcmDictEntry *DcmHashDict::findInList(DcmDictEntryList& l, const DcmTagKey& key, const char *privCreator) const
{
    return l.find(key, privCreator);
}

const DcmDictEntry*
DcmHashDict::get(const DcmTagKey& key, const char *privCreator) const
{
    const DcmDictEntry* entry = NULL;

    // first we look for an entry that exactly matches the given tag key
    Uint32 idx = hash(&key, privCreator);
    DcmDictEntryList* bucket = hashTab[idx];
    if (bucket) entry = findInList(*bucket, key, privCreator);

    if ((entry == NULL) && privCreator)
    {
      // As a second guess, we look for a private tag with flexible element number.
      DcmTagKey tk(key.getGroup(), OFstatic_cast(unsigned short, key.getElement() & 0xff));
      idx = hash(&tk, privCreator);
      bucket = hashTab[idx];
      if (bucket) entry = findInList(*bucket, tk, privCreator);
    }

    return entry;
}

DcmDictEntry*
DcmHashDict::removeInList(DcmDictEntryList& l, const DcmTagKey& key, const char *privCreator)
{
    DcmDictEntry* entry = findInList(l, key, privCreator);
    l.remove(entry); // does not delete entry
    return entry;
}

void
DcmHashDict::del(const DcmTagKey& key, const char *privCreator)
{
    Uint32 idx = hash(&key, privCreator);

    DcmDictEntryList* bucket = hashTab[idx];
    if (bucket != NULL) {
        DcmDictEntry* entry = removeInList(*bucket, key, privCreator);
        delete entry;
    }
}

STD_NAMESPACE ostream&
DcmHashDict::loadSummary(STD_NAMESPACE ostream& out)
{
    out << "DcmHashDict: size=" << hashTabLength <<
        ", total entries=" << size() << OFendl;
    DcmDictEntryList* bucket = NULL;
    int largestBucket = 0;
    for (int i=0; i<hashTabLength; i++) {
        bucket = hashTab[i];
        if (bucket != NULL) {
            if (int(bucket->size()) > largestBucket) {
                largestBucket = bucket->size();
            }
        }
    }

    for (int j=0; j<hashTabLength; j++) {
        out << "    hashTab[" << j << "]: ";
        bucket = hashTab[j];
        if (bucket == NULL) {
            out << "0 entries" << OFendl;
        } else {
            out << bucket->size() << " entries" << OFendl;
        }
    }
    out << "Bucket Sizes" << OFendl;
    int n, x, k, l_size;
    for (x=0; x<=largestBucket; x++) {
        n = 0;
        for (k=0; k<hashTabLength; k++) {
            bucket = hashTab[k];
            l_size = 0;
            if (bucket != NULL) {
                l_size = bucket->size();
            }
            if (l_size == x) {
                n++;
            }
        }
        out << "    entries{" << x << "}: " << n << " buckets" << OFendl;
    }

    return out;
}


/*
** CVS/RCS Log:
** $Log: dchashdi.cc,v $
** Revision 1.31  2011-08-18 14:40:42  joergr
** Replaced the private creator part of the hash function by an equivalent
** but probably more efficient implementation.
**
** Revision 1.30  2011-08-17 14:45:22  uli
** Improved hashing function for tags (less code, handles private creator).
**
** Revision 1.29  2011-08-17 14:38:24  uli
** Fixed a crash when iterating over an empty dictionary.
**
** Revision 1.28  2011-08-11 09:37:11  joergr
** Added missing header file to compile when PRINT_REPLACED_DICTIONARY_ENTRIES
** is defined.
**
** Revision 1.27  2011-02-04 11:14:38  uli
** Stop inheriting from OFList and OFListIterator.
**
** Revision 1.26  2010-11-10 12:02:01  uli
** Made DcmHashDictIterator::stepUp correctly stop at the end of the dict.
**
** Revision 1.25  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.24  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.23  2009-11-10 12:38:29  uli
** Fix compilation on windows.
**
** Revision 1.22  2009-11-04 09:58:09  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.21  2006-08-15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.20  2005/12/08 15:41:11  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.19  2004/02/04 16:33:02  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.18  2003/06/02 16:58:01  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.17  2003/03/21 13:08:04  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.16  2002/11/27 12:06:47  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.15  2002/07/23 14:21:33  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.14  2001/06/01 15:49:04  meichel
** Updated copyright header
**
** Revision 1.13  2000/10/12 10:26:52  meichel
** Updated data dictionary for 2000 edition of the DICOM standard
**
** Revision 1.12  2000/05/03 14:19:09  meichel
** Added new class GlobalDcmDataDictionary which implements read/write lock
**   semantics for safe access to the DICOM dictionary from multiple threads
**   in parallel. The global dcmDataDict now uses this class.
**
** Revision 1.11  2000/04/14 16:16:22  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.10  2000/03/08 16:26:36  meichel
** Updated copyright header.
**
** Revision 1.9  2000/03/03 14:05:33  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/02 14:32:51  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.7  1999/03/31 09:25:29  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1999/03/22 09:58:32  meichel
** Fixed bug in data dictionary causing a segmentation fault
**   if dictionary was cleared and a smaller version reloaded.
**
** Revision 1.5  1998/07/28 15:52:37  meichel
** Introduced new compilation flag PRINT_REPLACED_DICTIONARY_ENTRIES
**   which causes the dictionary to display all duplicate entries.
**
** Revision 1.4  1998/07/15 15:51:57  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.3  1998/06/29 12:17:57  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.2  1997/09/18 08:10:54  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.1  1997/08/26 13:35:02  hewett
** Initial Version - Implementation of hash table for data dictionary.
**
*/
