/*
**
** Author: Andrew Hewett        Created: 14.07.97
** Kuratorium OFFIS e.V.
**
** Module: dchashdi.cc
**
** Purpose:
** Hashtable implementation for DICOM data dictionary
** 
**
** Last Update:         $Author: meichel $
** Update Date:         $Date: 1998-07-28 15:52:37 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dchashdi.cc,v $
** CVS/RCS Revision:    $Revision: 1.5 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <assert.h>

#include "dchashdi.h"

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
        delete front();
        pop_front();
    }
}

DcmDictEntry*
DcmDictEntryList::insertAndReplace(DcmDictEntry* e)
{
    if (empty()) {
        push_front(e);
    } else {
        DcmDictEntryListIterator iter(begin());
        DcmDictEntryListIterator last(end());
        Uint32 eHash = e->hash();
        Uint32 iterHash = 0;
        // insert smallest first
        for (iter = begin(); iter != last; ++iter) {
            iterHash = (*iter)->hash();
            if (eHash == iterHash) {
                // entry is already there so replace it 
                DcmDictEntry* oldEntry = *iter;
                *iter = e;
                return oldEntry;
            } else if (eHash < iterHash) {
                // insert before listEntry
                insert(iter, e);
                return NULL;
            }           
        }
        // add to end
        push_back(e);
    }
    return NULL;
}

DcmDictEntry*
DcmDictEntryList::find(const DcmTagKey& k)
{
    if (!empty()) {
        DcmDictEntryListIterator iter;
        DcmDictEntryListIterator last = end();
        Uint32 kHash = k.hash();
        Uint32 iterHash = 0;
        for (iter = begin(); iter != last; ++iter) {
            iterHash = (*iter)->hash();
            if (iterHash == kHash) {
                return *iter;
            } else if (iterHash > kHash) {
                return NULL; // not there 
            }
        }
    }
    return NULL;
}


/*
** DcmHashDictIterator
*/

void
DcmHashDictIterator::init(DcmHashDict* d, OFBool atEnd)
{
    dict = d;
    hindex = 0;
    iterating = OFFalse;
    if (dict != NULL) {
        if (atEnd) {
            hindex = dict->highestBucket;
            if (dict->size() > 0) {
                iter = dict->hashTab[hindex]->end();
                iterating = OFTrue;
            }
        } else {
            hindex = dict->lowestBucket;
            if (dict->size() > 0) {
                iter = dict->hashTab[hindex]->begin();
                iterating = OFTrue;
            }
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

void 
DcmHashDict::_init(int hashTabLen)
{
    hashTab = new DcmDictEntryList*[hashTabLen];
    assert(hashTab != NULL);
    hashTabLength = hashTabLen;
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
    delete hashTab;
}

void
DcmHashDict::clear()
{
    for (int i=0; i<hashTabLength; i++) {
        delete hashTab[i];
        hashTab[i] = NULL;
    }
    entryCount = 0;
}

int
DcmHashDict::hash(const DcmTagKey* k) 
{
    /*
    ** Use a hash function based upon the relative number of 
    ** data dictionary entries in each group by splitting
    ** the hash table into proportional sections .
    */
    int h = 0;

    int lower = 0; /* default */
    int upper = hashTabLength-1; /* default */

    switch (k->getGroup()) {
        /* the code in this switch statement was generated automatically */
    case 0x0: /* %usage: 3.47 */
        lower = int(0 * hashTabLength);
        upper = int(0.0346608 * hashTabLength);
        break;
    case 0x2: /* %usage: 0.74 */
        lower = int(0.0346608 * hashTabLength);
        upper = int(0.0420354 * hashTabLength);
        break;
    case 0x4: /* %usage: 1.40 */
        lower = int(0.0420354 * hashTabLength);
        upper = int(0.0560472 * hashTabLength);
        break;
    case 0x8: /* %usage: 7.30 */
        lower = int(0.0560472 * hashTabLength);
        upper = int(0.129056 * hashTabLength);
        break;
    case 0x10: /* %usage: 2.43 */
        lower = int(0.129056 * hashTabLength);
        upper = int(0.153392 * hashTabLength);
        break;
    case 0x18: /* %usage: 18.36 */
        lower = int(0.153392 * hashTabLength);
        upper = int(0.337021 * hashTabLength);
        break;
    case 0x20: /* %usage: 3.98 */
        lower = int(0.337021 * hashTabLength);
        upper = int(0.376844 * hashTabLength);
        break;
    case 0x28: /* %usage: 8.63 */
        lower = int(0.376844 * hashTabLength);
        upper = int(0.463127 * hashTabLength);
        break;
    case 0x32: /* %usage: 1.92 */
        lower = int(0.463127 * hashTabLength);
        upper = int(0.482301 * hashTabLength);
        break;
    case 0x38: /* %usage: 1.62 */
        lower = int(0.482301 * hashTabLength);
        upper = int(0.498525 * hashTabLength);
        break;
    case 0x40: /* %usage: 6.93 */
        lower = int(0.498525 * hashTabLength);
        upper = int(0.567847 * hashTabLength);
        break;
    case 0x41: /* %usage: 2.29 */
        lower = int(0.567847 * hashTabLength);
        upper = int(0.590708 * hashTabLength);
        break;
    case 0x50: /* %usage: 0.59 */
        lower = int(0.590708 * hashTabLength);
        upper = int(0.596608 * hashTabLength);
        break;
    case 0x54: /* %usage: 5.75 */
        lower = int(0.596608 * hashTabLength);
        upper = int(0.65413 * hashTabLength);
        break;
    case 0x88: /* %usage: 0.59 */
        lower = int(0.65413 * hashTabLength);
        upper = int(0.660029 * hashTabLength);
        break;
    case 0x1000: /* %usage: 0.52 */
        lower = int(0.660029 * hashTabLength);
        upper = int(0.665192 * hashTabLength);
        break;
    case 0x1010: /* %usage: 0.15 */
        lower = int(0.665192 * hashTabLength);
        upper = int(0.666667 * hashTabLength);
        break;
    case 0x2000: /* %usage: 0.59 */
        lower = int(0.666667 * hashTabLength);
        upper = int(0.672566 * hashTabLength);
        break;
    case 0x2010: /* %usage: 1.18 */
        lower = int(0.672566 * hashTabLength);
        upper = int(0.684366 * hashTabLength);
        break;
    case 0x2020: /* %usage: 0.59 */
        lower = int(0.684366 * hashTabLength);
        upper = int(0.690265 * hashTabLength);
        break;
    case 0x2030: /* %usage: 0.22 */
        lower = int(0.690265 * hashTabLength);
        upper = int(0.692478 * hashTabLength);
        break;
    case 0x2040: /* %usage: 0.66 */
        lower = int(0.692478 * hashTabLength);
        upper = int(0.699115 * hashTabLength);
        break;
    case 0x2050: /* %usage: 0.22 */
        lower = int(0.699115 * hashTabLength);
        upper = int(0.701327 * hashTabLength);
        break;
    case 0x2100: /* %usage: 0.81 */
        lower = int(0.701327 * hashTabLength);
        upper = int(0.70944 * hashTabLength);
        break;
    case 0x2110: /* %usage: 0.37 */
        lower = int(0.70944 * hashTabLength);
        upper = int(0.713127 * hashTabLength);
        break;
    case 0x2120: /* %usage: 0.29 */
        lower = int(0.713127 * hashTabLength);
        upper = int(0.716077 * hashTabLength);
        break;
    case 0x2130: /* %usage: 0.66 */
        lower = int(0.716077 * hashTabLength);
        upper = int(0.722714 * hashTabLength);
        break;
    case 0x3002: /* %usage: 1.25 */
        lower = int(0.722714 * hashTabLength);
        upper = int(0.735251 * hashTabLength);
        break;
    case 0x3004: /* %usage: 1.62 */
        lower = int(0.735251 * hashTabLength);
        upper = int(0.751475 * hashTabLength);
        break;
    case 0x3006: /* %usage: 3.24 */
        lower = int(0.751475 * hashTabLength);
        upper = int(0.783923 * hashTabLength);
        break;
    case 0x300a: /* %usage: 16.52 */
        lower = int(0.783923 * hashTabLength);
        upper = int(0.949115 * hashTabLength);
        break;
    case 0x300c: /* %usage: 1.84 */
        lower = int(0.949115 * hashTabLength);
        upper = int(0.967552 * hashTabLength);
        break;
    case 0x300e: /* %usage: 0.29 */
        lower = int(0.967552 * hashTabLength);
        upper = int(0.970501 * hashTabLength);
        break;
    case 0x4000: /* %usage: 0.22 */
        lower = int(0.970501 * hashTabLength);
        upper = int(0.972714 * hashTabLength);
        break;
    case 0x4008: /* %usage: 2.06 */
        lower = int(0.972714 * hashTabLength);
        upper = int(0.993363 * hashTabLength);
        break;
    case 0x7fe0: /* %usage: 0.37 */
        lower = int(0.993363 * hashTabLength);
        upper = int(0.99705 * hashTabLength);
        break;
    case 0xfffc: /* %usage: 0.07 */
        lower = int(0.99705 * hashTabLength);
        upper = int(0.997788 * hashTabLength);
        break;
    case 0xfffe: /* %usage: 0.22 */
        lower = int(0.997788 * hashTabLength);
        upper = int(1 * hashTabLength);
        break;
    default:
        lower = 0; /* default */
        upper = hashTabLength-1; /* default */
        break;
    }

    int span = upper - lower;
    int offset = 0;
    if (span > 0) {
        offset = (int)((k->hash() & 0x7FFFFFFF) % span);
    }
    h =  lower + offset;

    assert((h >= 0) && (h < hashTabLength));

    return h;
}

DcmDictEntry*  
DcmHashDict::insertInList(DcmDictEntryList& list, DcmDictEntry* e)
{
    return list.insertAndReplace(e);
}

void
DcmHashDict::put(DcmDictEntry* e)
{
    int index = hash(e);

    DcmDictEntryList* bucket = hashTab[index];
    // if there is no bucket then create one
    if (bucket == NULL) {
        bucket = new DcmDictEntryList;
        assert(bucket != NULL);
        hashTab[index] = bucket;
    }

    DcmDictEntry* old = insertInList(*bucket, e);
    if (old != NULL) {
        /* an old entry has been replaced */
#ifdef PRINT_REPLACED_DICTIONARY_ENTRIES 
        cerr << "replacing " << *old << endl;
#endif
        delete old;
    } else {
        entryCount++;
    }

    lowestBucket = (lowestBucket<index)?(lowestBucket):(index);
    highestBucket = (highestBucket>index)?(highestBucket):(index);
}

DcmDictEntry* 
DcmHashDict::findInList(DcmDictEntryList& list, const DcmTagKey& k)
{
    return list.find(k);
}

const DcmDictEntry* 
DcmHashDict::get(const DcmTagKey& k)
{
    const DcmDictEntry* entry = NULL;
    Uint32 index = hash(&k);

    DcmDictEntryList* bucket = hashTab[index];
    if (bucket != NULL) {
        entry = findInList(*bucket, k);
    }
    return entry;
}

DcmDictEntry*
DcmHashDict::removeInList(DcmDictEntryList& list, const DcmTagKey& k)
{
    DcmDictEntry* entry = findInList(list, k);
    list.remove(entry); // does not delete entry
    return entry;
}

void
DcmHashDict::del(const DcmTagKey& k) 
{
    Uint32 index = hash(&k);

    DcmDictEntryList* bucket = hashTab[index];
    if (bucket != NULL) {
        DcmDictEntry* entry = removeInList(*bucket, k);
        delete entry;
    }
}

ostream& 
DcmHashDict::loadSummary(ostream& out)
{
    out << "DcmHashDict: size=" << hashTabLength << 
        ", total entries=" << size() << endl;
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
            out << "0 entries" << endl;
        } else {
            out << bucket->size() << " entries" << endl;
        }
    }
    out << "Bucket Sizes" << endl;
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
        out << "    entries{" << x << "}: " << n << " buckets" << endl;
    }

    return out;
}

/*
** CVS/RCS Log:
** $Log: dchashdi.cc,v $
** Revision 1.5  1998-07-28 15:52:37  meichel
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
**
*/
