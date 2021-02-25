/*
 *
 *  Copyright (C) 1997-2021, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dchashdi.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dctypes.h"

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
    return OFstatic_cast(unsigned int, list_.size());
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
