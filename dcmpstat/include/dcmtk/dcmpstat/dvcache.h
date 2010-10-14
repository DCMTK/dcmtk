/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Classes for caching of the image database (Header/Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:35 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DVCACHE_H
#define DVCACHE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"  /* for DVIFhierarchyStatus */


/*--------------------*
 *  type definitions  *
 *--------------------*/

/** describes the different types of instances stored in the database
 */
enum DVPSInstanceType
{
    /// image object
    DVPSI_image,
    /// presentation state object
    DVPSI_presentationState,
    /// structured reporting document
    DVPSI_structuredReport,
    /// stored print object
    DVPSI_storedPrint,
    /// hardcopy grayscale object
    DVPSI_hardcopyGrayscale
};


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class to handle an instance cache (list of items).
 *  This is the lowest level in the hierarchical cache structure.
 *  Images are handled as well as presentation states. This class
 *  is used by DVSeriesCache.
 */
class DVInstanceCache
{

 public:

    /** Internal structure defining the list items.
     */
    struct ItemStruct
    {
        /** Constructor.
         *  sets internal member variables.
         *
         ** @param  uid       unique identifier
         *  @param  pos       file position in index file
         *  @param  status    review status
         *  @param  type      type of instance
         *  @param  size      image size (in bytes)
         *  @param  filename  filename of instance
         */
        ItemStruct(const OFString &uid,
                   const int pos,
                   const DVIFhierarchyStatus status,
                   const DVPSInstanceType type,
                   const int size,
                   const OFString &filename)
          : UID(uid),
            Pos(pos),
            Status(status),
            Type(type),
            ImageSize(size),
            Filename(filename),
            Checked(OFFalse),
            Description(),
            Label(),
            List()
        {}

        /// instance UID
        OFString UID;
        /// position in the index file
        int Pos;
        /// review status
        DVIFhierarchyStatus Status;
        /// type of instance
        DVPSInstanceType Type;
        /// image size (in bytes)
        int ImageSize;
        /// filename of instance
        OFString Filename;
        /// status flag to avoid double checking of referencing pstates
        OFBool Checked;
        /// instance description
        OFString Description;
        /// instance label
        OFString Label;
        /// list of referencing pstates
        OFList<ItemStruct *> List;
    };

    /** Constructor
     */
    DVInstanceCache()
      : List(),
        Iterator(),
        OldIterator()
    {
        Iterator = OldIterator = List.end();
    }

    /** Destructor
     */
    virtual ~DVInstanceCache()
    {
        clear();
    }

    /** reset all member variables to initial state.
     *  delete all list items.
     */
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

    /** checks whether instance cache is empty
     *
     ** @result OFTrue if cache is empty, OFFalse otherwise
     */
    inline OFBool empty() const
    {
        return List.empty();
    }

    /** gets number of cache entries
     *
     ** @return number of cache entries
     */
    inline Uint32 getCount() const
    {
        return List.size();
    }

    /** sets internal cursor to specified position in cache list
     *
     ** @param  idx  index position in cache list (starting with 0)
     *
     ** @return OFTrue if successful, OFFalse if 'idx' is invalid
     */
    inline OFBool gotoItem(Uint32 idx)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
            {
                result = OFTrue;
                break;
            }
            idx--;
            ++Iterator;
        }
        return result;
    }

    /** sets internal cursor to first position in cache list
     *
     ** @return OFTrue if successful, OFFalse if list is empty
     */
    inline OFBool gotoFirst()
    {
        OldIterator = Iterator;
        Iterator = List.begin();
        return (Iterator != List.end());
    }

    /** sets internal cursor to next position in cache list
     *
     ** @return OFTrue if successful, OFFalse if new position is invalid
     */
    inline OFBool gotoNext()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (Iterator != last)
            Iterator++;
        return (Iterator != last);
    }

    /** sets internal cursor to last visited position in cache list
     *
     ** @return OFTrue if successful,
     *          OFFalse if last visited position was invalid or the last one in the list
     */
    inline OFBool reset()
    {
        OFBool result = OFFalse;
        OFListIterator(ItemStruct *) last = List.end();
        if (OldIterator != last)
        {
            Iterator = OldIterator;
            OldIterator = last;
            result = OFTrue;
        }
        return result;
    }

    /** checks whether an item with the specified UID exists in the cache list
     *
     ** @param  uid  UID which should be checked
     *
     ** @return OFTrue if such an item exists, OFFalse otherwise
     */
    inline OFBool isElem(const OFString &uid)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                {
                    result = OFTrue;
                    break;
                }
            }
            ++Iterator;
        }
        return result;
    }

    /** gets the file position of the current (selected) instance
     *
     ** @return file position if successful, 0 otherwise
     */
    inline int getPos() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Pos : 0;
    }

    /** gets review status of the current (selected) instance
     *
     ** @return hierarchical status code if successful, 'isNew' otherwise
     */
    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Status : DVIF_objectIsNew;
    }

    /** gets type of the instance
     *
     ** @return type of instance
     */
    inline DVPSInstanceType getType() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Type : DVPSI_image;
    }

    /** gets image size of current (selected) instance
     *
     ** @return image size in bytes if successful, 0 otherwise
     */
    inline int getImageSize() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->ImageSize : 0;
    }

    /** gets filename of current (selected) instance
     *
     ** @return filename if successful, NULL otherwise
     */
    inline const char *getFilename() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Filename.c_str() : (const char *)NULL;
    }

    /** gets reference to current (selected) instance
     *
     ** @return pointer to ItemStruct if successful, NULL otherwise
     */
    inline ItemStruct *getItem() const
    {
		OFListConstIterator(ItemStruct *) it = Iterator;
        return (it != List.end()) ? (*Iterator) : (ItemStruct *)NULL;
    }

    /** adds a new item to the cache list.
     *  sets internal cursor to new position.
     *
     ** @param  uid       unique identifier
     *  @param  pos       file position in index file
     *  @param  status    review status
     *  @param  type      type of instance
     *  @param  size      image size (in bytes)
     *  @param  filename  filename of instance
     */
    inline void addItem(const OFString &uid,
                        const int pos,
                        const DVIFhierarchyStatus status,
                        const DVPSInstanceType type,
                        const int size,
                        const OFString &filename)
    {
        ItemStruct *item = new ItemStruct(uid, pos, status, type, size, filename);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    /** updates hierarchical/review status for all list items.
     *
     ** @return resulting review status (summary of all items)
     */
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
                        else if (status == DVIF_objectIsNew)
                            status = DVIF_objectContainsNewSubobjects;
                        break;
                }
            }
            ++iter;
        }
        return status;
    }


 protected:

    /// list of instances
    OFList<ItemStruct *> List;
    /// internal cursor to current (selected) list item
    OFListIterator(ItemStruct *) Iterator;
    /// last visited position in item list
    OFListIterator(ItemStruct *) OldIterator;
};


/* ------------------------------ */


/** A class to handle a series cache (list of items).
 *  This is the middle level in the hierarchical cache structure.
 *  This class is used by DVStudyCache. The internal structure
 *  is a list of DVInstanceCache.
 */
class DVSeriesCache
{

 public:

    /** Internal structure defining the list items.
     */
    struct ItemStruct
    {
        /** Constructor.
         *  sets internal member variables.
         *
         ** @param  uid     unique identifier
         *  @param  status  review status (optional)
         *  @param  type    type of series
         */
        ItemStruct(const OFString &uid,
                   const DVIFhierarchyStatus status = DVIF_objectIsNew,
                   const DVPSInstanceType type = DVPSI_image)
          : UID(uid),
            Status(status),
            Type(type),
            List()
        {}

        /// instance UID
        OFString UID;
        /// review status for the series
        DVIFhierarchyStatus Status;
        /// type of all instances within this series
        DVPSInstanceType Type;
        /// list of instances within this series
        DVInstanceCache List;
    };

    /** Constructor.
     */
    DVSeriesCache()
      : List(),
        Iterator(),
        OldIterator()
    {
        Iterator = OldIterator = List.end();
    }

    /** Destructor
     */
    virtual ~DVSeriesCache()
    {
        clear();
    }

    /** reset all member variables to initial state
     *  delete all list items.
     */
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

    /** checks whether instance cache is empty
     *
     ** @return OFTrue if cache is empty, OFFalse otherwise
     */
    inline OFBool empty() const
    {
        return List.empty();
    }

    /** gets number of cache entries
     *
     ** @return number of cache entries
     */
    inline Uint32 getCount() const
    {
        return List.size();
    }

    /** sets internal cursor to specified position in cache list
     *
     ** @param  idx  index position in cache list (starting with 0)
     *
     ** @return OFTrue if successful, OFFalse if 'idx' is invalid
     */
    inline OFBool gotoItem(Uint32 idx)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
            {
                result = OFTrue;
                break;
            }
            idx--;
            ++Iterator;
        }
        return result;
    }

    /** sets internal cursor to first position in cache list
     *
     ** @return OFTrue if successful, OFFalse if list is empty
     */
    inline OFBool gotoFirst()
    {
        OldIterator = Iterator;
        Iterator = List.begin();
        return (Iterator != List.end());
    }

    /** sets internal cursor to next position in cache list
     *
     ** @return OFTrue if successful, OFFalse if new position is invalid
     */
    inline OFBool gotoNext()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (Iterator != last)
            Iterator++;
        return (Iterator != last);
    }

    /** sets internal cursor to last visited position in cache list
     *
     ** @return OFTrue if successful,
     *          OFFalse if last visited position was invalid or the last one in the list
     */
    inline OFBool reset()
    {
        OFBool result = OFFalse;
        OFListIterator(ItemStruct *) last = List.end();
        if (OldIterator != last)
        {
            Iterator = OldIterator;
            OldIterator = last;
            result = OFTrue;
        }
        return result;
    }

    /** checks whether an item with the specified UID exists in the cache list
     *
     ** @param  uid  UID which should be checked
     *
     ** @return OFTrue if such an item exists, OFFalse otherwise
     */
    inline OFBool isElem(const OFString &uid)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                {
                    result = OFTrue;
                    break;
                }
            }
            ++Iterator;
        }
        return result;
    }

    /** gets review status of the current (selected) series
     *
     ** @return hierarchical status code if successful, 'isNew' otherwise
     */
    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Status : DVIF_objectIsNew;
    }

    /** gets type of all instances in the series
     *
     ** @return type of all instances
     */
    inline DVPSInstanceType getType() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Type : DVPSI_image;
    }

    /** gets reference to current (selected) series
     *
     ** @return pointer to ItemStruct if successful, NULL otherwise
     */
    inline ItemStruct *getItem() const
    {
		OFListConstIterator(ItemStruct *) it = Iterator;
        return (it != List.end()) ? (*Iterator) : (ItemStruct *)NULL;
    }

    /** adds a new item to the cache list.
     *  sets internal cursor to new position.
     *
     ** @param  uid       unique identifier
     *  @param  status    review status (optional)
     */
    inline void addItem(const OFString &uid,
                        const DVIFhierarchyStatus status = DVIF_objectIsNew)
    {
        ItemStruct *item = new ItemStruct(uid, status);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    /** updates hierarchical/review status for all list items.
     *
     ** @return resulting review status (summary of all items)
     */
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
                        else if (status == DVIF_objectIsNew)
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

    /// list of series
    OFList<ItemStruct *> List;
    /// internal cursor to current (selected) list item
    OFListIterator(ItemStruct *) Iterator;
    /// last visited position in item list
    OFListIterator(ItemStruct *) OldIterator;
};


/* ------------------------------ */


/** A class to handle a study cache (list of items).
 *  This is the highest level in the hierarchical cache structure.
 *  This class is used by DVInterface. The internal structure
 *  is a list of DVSeriesCache.
 */
class DVStudyCache
{

 public:

    /** Internal structure defining the list items.
     */
    struct ItemStruct
    {
        /** Constructor.
         *  sets internal member variables.
         *
         ** @param  uid       unique identifier
         *  @param  status    review status (optional)
         */
        ItemStruct(const OFString &uid,
                   const DVIFhierarchyStatus status = DVIF_objectIsNew)
          : UID(uid),
            Status(status),
            List()
        {}

        /// instance UID
        OFString UID;
        /// review status for the series
        DVIFhierarchyStatus Status;
        /// list of series within this study
        DVSeriesCache List;
    };

    /** Constructor.
     */
    DVStudyCache()
      : List(),
        Iterator()
    {
        Iterator = List.end();
    }

    /** Destructor
     */
    virtual ~DVStudyCache()
    {
        clear();
    }

    /** reset all member variables to initial state
     *  delete all list items.
     */
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

    /** checks whether study cache is empty
     *
     ** @return OFTrue if cache is empty, OFFalse otherwise
     */
    inline OFBool empty() const
    {
        return List.empty();
    }

    /** gets number of cache entries
     *
     ** @return number of cache entries
     */
    inline Uint32 getCount() const
    {
        return List.size();
    }

    /** sets internal cursor to specified position in cache list
     *
     ** @param  idx  index position in cache list (starting with 0)
     *
     ** @return OFTrue if successful, OFFalse if 'idx' is invalid
     */
    inline OFBool gotoItem(Uint32 idx)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            if (idx == 0)
            {
                result = OFTrue;
                break;
            }
            idx--;
            ++Iterator;
        }
        return result;
    }

    /** sets internal cursor to first position in cache list
     *
     ** @return OFTrue if successful, OFFalse if list is empty
     */
    inline OFBool gotoFirst()
    {
        //OldIterator = Iterator;
        Iterator = List.begin();
        return (Iterator != List.end());
    }

    /** sets internal cursor to next position in cache list
     *
     ** @return OFTrue if successful, OFFalse if new position is invalid
     */
    inline OFBool gotoNext()
    {
        OFListIterator(ItemStruct *) last = List.end();
        if (Iterator != last)
            Iterator++;
        return (Iterator != last);
    }

    /** checks whether an item with the specified UID exists in the cache list
     *
     ** @param  uid  UID which should be checked
     *
     ** @return OFTrue if such an item exists, OFFalse otherwise
     */
    inline OFBool isElem(const OFString &uid)
    {
        OFBool result = OFFalse;
        Iterator = List.begin();
        OFListIterator(ItemStruct *) last = List.end();
        while (Iterator != last)
        {
            const ItemStruct *item = (*Iterator);
            if (item != NULL)
            {
                if (item->UID == uid)
                {
                    result= OFTrue;
                    break;
                }
            }
            ++Iterator;
        }
        return result;
    }

    /** gets review status of the current (selected) sstudy
     *
     ** @return hierarchical status code if successful, 'isNew' otherwise
     */
    inline DVIFhierarchyStatus getStatus() const
    {
        const ItemStruct *item = getItem();
        return (item != NULL) ? item->Status : DVIF_objectIsNew;
    }

    /** gets reference to current (selected) study
     *
     ** @return pointer to ItemStruct if successful, NULL otherwise
     */
    inline ItemStruct *getItem() const
    {
		OFListConstIterator(ItemStruct *) it = Iterator;
        return (it != List.end()) ? (*Iterator) : (ItemStruct *)NULL;
    }

    /** adds a new item to the cache list.
     *  sets internal cursor to new position.
     *
     ** @param  uid       unique identifier
     *  @param  status    review status (optional)
     */
    inline void addItem(const OFString &uid,
                        const DVIFhierarchyStatus status = DVIF_objectIsNew)
    {
        ItemStruct *item = new ItemStruct(uid, status);
        List.push_back(item);
        Iterator = --List.end();                // set to new position
    }

    /** updates hierarchical/review status for all list items.
     *
     ** @return resulting review status (summary of all items)
     */
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

    /// list of studies
    OFList<ItemStruct *> List;
    /// internal cursor to current (selected) list item
    OFListIterator(ItemStruct *) Iterator;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dvcache.h,v $
 * Revision 1.20  2010-10-14 13:16:35  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2010-10-07 14:31:35  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved).
 *
 * Revision 1.18  2009-09-07 12:51:40  joergr
 * Converted Windows line breaks to Unix format.
 *
 * Revision 1.17  2009-09-04 13:53:09  meichel
 * Minor const iterator related changes needed to compile with VC6 with HAVE_STL
 *
 * Revision 1.16  2005-12-08 16:03:30  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.15  2005/04/04 10:11:57  meichel
 * Module dcmpstat now uses the dcmqrdb API instead of imagectn for maintaining
 *   the index database
 *
 * Revision 1.14  2001/06/01 15:50:11  meichel
 * Updated copyright header
 *
 * Revision 1.13  2000/10/16 11:39:10  joergr
 * Added method allowing to select an instance by instance UID and SOP class
 * UID (without series and study UID). Required for composite references in
 * DICOM SR.
 *
 * Revision 1.12  2000/06/30 09:08:39  joergr
 * Fixed bug in database cache routines (re. study status).
 *
 * Revision 1.11  2000/05/30 13:37:15  joergr
 * Renamed GrayscaleHardcopy to HardcopyGrayscale (which is the correct term
 * according to the DICOM standard).
 *
 * Revision 1.10  2000/03/08 16:28:47  meichel
 * Updated copyright header.
 *
 * Revision 1.9  1999/09/08 17:03:00  joergr
 * Added support for new instance types in database (grayscale hardcopy and
 * stored print).
 *
 * Revision 1.8  1999/08/17 10:32:54  joergr
 * Added Doc++ styled comments.
 * Corrected wrong return type for method 'getImageSize()'.
 *
 * Revision 1.7  1999/05/03 11:01:08  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.6  1999/04/29 15:25:36  joergr
 * Added PresentationLabel to index file.
 *
 * Revision 1.5  1999/04/27 11:20:49  joergr
 * Add remaining member variables to member initialization list to avoid
 * compiler warnings.
 *
 * Revision 1.4  1999/02/24 20:14:39  joergr
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
