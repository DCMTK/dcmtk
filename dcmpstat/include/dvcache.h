/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Classes for caching of the image database (Header/Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-24 20:14:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/include/Attic/dvcache.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DVCACHE_H
#define __DVCACHE_H

#include "osconfig.h"

#include "oflist.h"
#include "ofstring.h"
#include "imagedb.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DVInstanceCache
{

 public:

    struct ItemStruct
    {
        ItemStruct(const OFString &uid,
                   const int pos,
                   const DVIFhierarchyStatus status,
                   const OFBool pstate,
                   const int size,
                   const OFString &filename)
          : UID(uid),
            Pos(pos),
            Status(status),
            PState(pstate),
            ImageSize(size),
            Filename(filename),
            Checked(OFFalse),
            Description(),
            List()
        {}
    
        OFString UID;
        int Pos;
        DVIFhierarchyStatus Status;
        OFBool PState;
        int ImageSize;
        OFString Filename;
        OFBool Checked;                 // do not check referencing pstates twice
        OFString Description;
        OFList<ItemStruct *> List;      // list of referencing pstates
    };

    DVInstanceCache()
    {
        Iterator = OldIterator = List.end();
    }

    virtual ~DVInstanceCache()
    {
        clear();
    }

    inline void clear()
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {     
            delete (*Iterator);
            Iterator = List.erase(Iterator);
        }
        List.clear();
        Iterator = OldIterator = List.end();
    }

    inline OFBool empty() const
    {
        return List.empty();
    }

    inline Uint32 getCount() const
    {
        return List.size();
    }

    inline OFBool gotoItem(Uint32 idx)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
                return OFTrue;
            idx--;
            ++Iterator;
        }
        return OFFalse;
    }
    
    inline OFBool gotoFirst()
    {
        OldIterator = Iterator;
        Iterator = List.begin();
        if (Iterator != List.end())
            return OFTrue;
        return OFFalse;
    }
    
    inline OFBool gotoNext()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (Iterator != last)
        {
            Iterator++;
            if (Iterator != last)
                return OFTrue;
        }
        return OFFalse;
    }
    
    inline OFBool reset()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (OldIterator != last)
        {
            Iterator = OldIterator;
            OldIterator = last;
            return OFTrue;
        }
        return OFFalse;
    }
    
    inline OFBool isElem(const OFString &uid)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                    return OFTrue;
            }
            ++Iterator;
        }
        return OFFalse;
    }

    inline int getPos() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->Pos;
        return 0;
    }

    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->Status;
        return DVIF_objectIsNew;
    }

    inline OFBool getPState() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->PState;
        return OFFalse;
    }

    inline OFBool getImageSize() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->ImageSize;
        return OFFalse;
    }

    inline const char *getFilename() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->Filename.c_str();
        return NULL;
    }

    inline ItemStruct *getItem() const
    {
        if (Iterator != List.end())
            return (*Iterator);
        return NULL;
    }

    inline void addItem(const OFString &uid,
                        const int pos,
                        const DVIFhierarchyStatus status,
                        const OFBool pstate,
                        const int size,
                        const OFString &filename)
    {
        ItemStruct *item = new ItemStruct(uid, pos, status, pstate, size, filename);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    inline DVIFhierarchyStatus updateStatus()
    {
        OFListIterator(ItemStruct *) first = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        OFListIterator(ItemStruct *) iter = first;
        DVIFhierarchyStatus status = DVIF_objectIsNew;
        while (iter != last)
        {
            ItemStruct *item = (*iter);
            if (item != NULL)
            {
                switch (item->Status)
                {
                    case DVIF_objectIsNew:
                        if (status == DVIF_objectIsNotNew)
                            status = DVIF_objectContainsNewSubobjects;
                        break;
                    case DVIF_objectIsNotNew:
                    case DVIF_objectContainsNewSubobjects:
                        if (iter == first)
                            status = DVIF_objectIsNotNew;
                        else if (status != DVIF_objectIsNotNew)
                            status = DVIF_objectContainsNewSubobjects;
                        break;
                }
            }
            ++iter;
        }
        return status;
    }


 protected:

    OFList<ItemStruct *> List;
    OFListIterator(ItemStruct *) Iterator;
    OFListIterator(ItemStruct *) OldIterator;
};


/* ------------------------------ */


class DVSeriesCache
{

 public:

    struct ItemStruct
    {
        ItemStruct(const OFString &uid,
                   const DVIFhierarchyStatus status = DVIF_objectIsNew,
                   const OFBool pstate = OFFalse)
          : UID(uid),
            Status(status),
            PState(pstate)
        {}
    
        OFString UID;
        DVIFhierarchyStatus Status;
        OFBool PState;
        DVInstanceCache List;
    };

    DVSeriesCache()
    {
        Iterator = OldIterator = List.end();
    }

    virtual ~DVSeriesCache()
    {
        clear();
    }

    inline void clear()
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {     
            delete (*Iterator);
            Iterator = List.erase(Iterator);
        }
        List.clear();
        Iterator = OldIterator = List.end();
    }

    inline OFBool empty() const
    {
        return List.empty();
    }

    inline Uint32 getCount() const
    {
        return List.size();
    }

    inline OFBool gotoItem(Uint32 idx)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
                return OFTrue;
            idx--;
            ++Iterator;
        }
        return OFFalse;
    }
    
    inline OFBool gotoFirst()
    {
        OldIterator = Iterator;
        Iterator = List.begin();
        if (Iterator != List.end())
            return OFTrue;
        return OFFalse;
    }
    
    inline OFBool gotoNext()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (Iterator != last)
        {
            Iterator++;
            if (Iterator != last)
                return OFTrue;
        }
        return OFFalse;
    }
    
    inline OFBool reset()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (OldIterator != last)
        {
            Iterator = OldIterator;
            OldIterator = last;
            return OFTrue;
        }
        return OFFalse;
    }
    
    inline OFBool isElem(const OFString &uid)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                    return OFTrue;
            }
            ++Iterator;
        }
        return OFFalse;
    }

    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->Status;
        return DVIF_objectIsNew;
    }

    inline OFBool getPState() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->PState;
        return OFFalse;
    }

    inline ItemStruct *getItem() const
    {
        if (Iterator != List.end())
            return (*Iterator);
        return NULL;
    }

    inline void addItem(const OFString &uid,
                        const DVIFhierarchyStatus status = DVIF_objectIsNew)
    {
        ItemStruct *item = new ItemStruct(uid, status);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    inline DVIFhierarchyStatus updateStatus()
    {
        OFListIterator(ItemStruct *) first = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        OFListIterator(ItemStruct *) iter = first;
        DVIFhierarchyStatus status = DVIF_objectIsNew;
        while (iter != last)
        {
            ItemStruct *item = (*iter);
            if (item != NULL)
            {
                item->Status = item->List.updateStatus();
                switch (item->Status)
                {
                    case DVIF_objectIsNew:
                        if (status == DVIF_objectIsNotNew)
                            status = DVIF_objectContainsNewSubobjects;
                        break;
                    case DVIF_objectIsNotNew:
                        if (iter == first)
                            status = DVIF_objectIsNotNew;
                        else
                            status = DVIF_objectContainsNewSubobjects;
                        break;
                    case DVIF_objectContainsNewSubobjects:
                        status  = DVIF_objectContainsNewSubobjects;
                        break;
                }
            }
            ++iter;
        }
        return status;
    }


 protected:

    OFList<ItemStruct *> List;
    OFListIterator(ItemStruct *) Iterator;
    OFListIterator(ItemStruct *) OldIterator;
};


/* ------------------------------ */


class DVStudyCache
{

 public:

    struct ItemStruct
    {
        ItemStruct(const OFString &uid,
                   const DVIFhierarchyStatus status = DVIF_objectIsNew)
          : UID(uid),
            Status(status)
        {}
    
        OFString UID;
        DVIFhierarchyStatus Status;
        DVSeriesCache List;
    };

    DVStudyCache()
    {
        Iterator = List.end();
    }

    virtual ~DVStudyCache()
    {
        clear();
    }

    inline void clear()
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {     
            delete (*Iterator);
            Iterator = List.erase(Iterator);
        }
        List.clear();
        Iterator = List.end();
    }

    inline OFBool empty() const
    {
        return List.empty();
    }

    inline Uint32 getCount() const
    {
        return List.size();
    }

    inline OFBool gotoItem(Uint32 idx)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
                return OFTrue;
            idx--;
            ++Iterator;
        }
        return OFFalse;
    }
    
    inline OFBool isElem(const OFString &uid)
    {
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                    return OFTrue;
            }
            ++Iterator;
        }
        return OFFalse;
    }

    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        if (item != NULL)
            return item->Status;
        return DVIF_objectIsNew;
    }

    inline ItemStruct *getItem() const
    {
        if (Iterator != List.end())
            return (*Iterator);
        return NULL;
    }

    inline void addItem(const OFString &uid,
                        const DVIFhierarchyStatus status = DVIF_objectIsNew)
    {
        ItemStruct *item = new ItemStruct(uid, status);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    inline void updateStatus()
    {
        OFListIterator(ItemStruct *) iter = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (iter != last)
        {
            ItemStruct *item = (*iter);
            if (item != NULL)
                item->Status = item->List.updateStatus();
            ++iter;
        }
    }


 protected:

    OFList<ItemStruct *> List;
    OFListIterator(ItemStruct *) Iterator;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dvcache.h,v $
 * Revision 1.4  1999-02-24 20:14:39  joergr
 * Added support for presentation state caching (e.g. pstate description).
 * Removed unused methods.
 *
 * Revision 1.3  1999/02/19 18:56:08  joergr
 * Added new methods to interate through Caches (getFirst/getNext) - needed
 * for delete routines in Interface class.
 *
 * Revision 1.2  1999/02/19 09:45:19  joergr
 * Changed some comments, corrected typos and formatting.
 *
 * Revision 1.1  1999/02/18 18:50:18  joergr
 * Re-implemented methods to access index file (delete methods are still
 * missing).
 *
 *
 */
