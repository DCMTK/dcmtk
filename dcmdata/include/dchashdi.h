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
 *  Purpose: Hash table interface for DICOM data dictionary
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:24:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dchashdi.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCHASHDI_H
#define DCHASHDI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "oflist.h"
#include "dcdicent.h"

/** the default size for a data dictionary hash table */
const int DCMHASHDICT_DEFAULT_HASHSIZE = 2047;

class DcmHashDict;

/**
** DcmDictEntryListIterator
**     An iterator for traversing a DcmDictEntryList
*/
class DcmDictEntryListIterator : public OFListIterator(DcmDictEntry*) {
public:
    DcmDictEntryListIterator() {}
    DcmDictEntryListIterator(OFListIterator(DcmDictEntry*) iter) 
        : OFListIterator(DcmDictEntry*)(iter) {}
    DcmDictEntryListIterator& operator=(const DcmDictEntryListIterator& i)
    {
      OFListIterator(DcmDictEntry*)::operator=(i);
      return *this;
    }
};

/**
** DcmDictEntryList
**     An ordered list of DcmDictEntry*
*/
class DcmDictEntryList : public OFList<DcmDictEntry*> {
public:
    DcmDictEntryList() {}
    /** destructor deletes all members */
    ~DcmDictEntryList();
    /** clear list and delete all entries */
    void clear();
    /** insert an entry into the list and return any replaced entry */
    DcmDictEntry* insertAndReplace(DcmDictEntry* e);
    /* find an entry in the set */
    DcmDictEntry* find(const DcmTagKey& k);
};


/**
** DcmHashDictIterator
**     An iterator for traversing a DcmHashDict
*/
class DcmHashDictIterator {
private:
    DcmHashDict* dict;
    int hindex;
    OFBool iterating;
    DcmDictEntryListIterator iter;
    
    void init(DcmHashDict *d, OFBool atEnd = OFFalse);
    void stepUp();

public:
    DcmHashDictIterator()
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
          { init(NULL); }
    DcmHashDictIterator(DcmHashDict* d, OFBool atEnd = OFFalse)
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
          { init(d, atEnd); }
    DcmHashDictIterator(const DcmHashDictIterator& i) 
      : dict(i.dict), hindex(i.hindex), iterating(i.iterating), iter(i.iter)
          { }

    DcmHashDictIterator& operator=(const DcmHashDictIterator& i) 
        { dict = i.dict; hindex = i.hindex; 
        iterating = i.iterating; iter = i.iter; return *this; }

    OFBool operator==(const DcmHashDictIterator& x) const 
        { return (hindex == x.hindex) && (iter == x.iter); }
    OFBool operator!=(const DcmHashDictIterator& x) const 
        { return (hindex != hindex) || (iter != x.iter); }

    const DcmDictEntry* operator*() const
        { return (*iter); }

    DcmHashDictIterator& operator++()
        { stepUp(); return *this; }
    DcmHashDictIterator operator++(int)
        { DcmHashDictIterator tmp(*this); stepUp(); return tmp; }

};

/**
** DcmHashDict
**     A hash table of DcmDictEntry*
*/
class DcmHashDict {
private:
    DcmDictEntryList** hashTab;
    int hashTabLength;
    int lowestBucket;
    int highestBucket;
    int entryCount;

    void _init(int hashSize);

 // --- declarations to avoid compiler warnings
 
    DcmHashDict &operator=(const DcmHashDict &);
    DcmHashDict(const DcmHashDict &);

protected:
    // calculate the hash function
    int hash(const DcmTagKey* k);
    // bucket access
    DcmDictEntry* insertInList(DcmDictEntryList& list, DcmDictEntry* e);
    DcmDictEntry* findInList(DcmDictEntryList& list, const DcmTagKey& k);
    DcmDictEntry* removeInList(DcmDictEntryList& list, const DcmTagKey& k);

public:
    DcmHashDict(int hashTabLen = DCMHASHDICT_DEFAULT_HASHSIZE)
     : hashTab(NULL), hashTabLength(0), lowestBucket(0), highestBucket(0), entryCount(0)
        { _init(hashTabLen); }


    ~DcmHashDict();

    // count total number of entries
    int size() { return entryCount; }

    // clear the hash table of all entries
    void clear();

    // insert an entry into hash table (deletes old entry if present)
    void put(DcmDictEntry* e);

    // lookup a key
    const DcmDictEntry* get(const DcmTagKey& k);

    // delete a key's entry
    void del(const DcmTagKey& k);

    // iterator over the contents of the hash table
    friend class DcmHashDictIterator;
    DcmHashDictIterator begin() 
        { DcmHashDictIterator iter(this); return iter; }
    DcmHashDictIterator end()   
        { DcmHashDictIterator iter(this, OFTrue); return iter; }

    // print some information about hash table bucket utilization
    ostream& loadSummary(ostream& out);
};

#endif /* DCHASHDI_H */

/*
** CVS/RCS Log:
** $Log: dchashdi.h,v $
** Revision 1.7  1999-03-31 09:24:39  meichel
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

