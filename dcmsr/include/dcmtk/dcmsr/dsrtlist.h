/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRListOfItems
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTLIST_H
#define DSRTLIST_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oflist.h"

#include "dcmtk/dcmdata/dcerror.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to store and access items in a list structure.
 *  For instances of this class T needs to have at least a default constructor,
 *  assignment and comparison operator.
 */
template<class T> class DSRListOfItems
{

  public:

    /** default constructor
     */
    DSRListOfItems()
      : ItemList()
    {
    }

    /** copy constructor
     ** @param  lst  list to be copied
     */
    DSRListOfItems(const DSRListOfItems<T> &lst)
      : ItemList(lst.ItemList)
    {
    }

    /** destructor
     */
    virtual ~DSRListOfItems()
    {
    }

    /** assignment operator
     ** @param  lst  list to be copied
     ** @return reference to this list after 'lst' has been copied
     */
    inline DSRListOfItems<T> &operator=(const DSRListOfItems<T> &lst)
    {
        /* class OFList has no overloaded assignment operator */
        ItemList.clear();
        const OFLIST_TYPENAME OFListConstIterator(T) endPos = lst.ItemList.end();
        OFLIST_TYPENAME OFListConstIterator(T) iterator = lst.ItemList.begin();
        while (iterator != endPos)
        {
            ItemList.push_back(*iterator);
            iterator++;
        }
        return *this;
    }

    /** clear all internal variables
     */
    inline void clear()
    {
        ItemList.clear();
    }

    /** check whether the list is empty
     ** @return OFTrue if the list is empty, OFFalse otherwise
     */
    inline OFBool isEmpty() const
    {
        return ItemList.empty();
    }

    /** get number of items contained in the list
     ** @return number of items if any, 0 otherwise
     */
    inline size_t getNumberOfItems() const
    {
        return ItemList.size();
    }

    /** check whether specified item is contained in the list
     ** @param  item  item to be checked
     ** @return OFTrue if the item is in the list, OFFalse otherwise
     */
    OFBool isElement(const T &item) const
    {
        OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
        return gotoItem(item, iterator);
    }

    /** get reference to the specified item
     ** @param  idx  index of the item to be returned (starting from 1)
     ** @return reference to the specified item if successful, EmptyItem otherwise
     */
    const T &getItem(const size_t idx) const
    {
        OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
        if (gotoItemPos(idx, iterator))
            return *iterator;
        else
            return EmptyItem;
    }

    /** get copy of the specified item
     ** @param  idx   index of the item to be returned (starting from 1)
     *  @param  item  reference to a variable where the result should be stored.
     *                (not changed/cleared if an error occurs!)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItem(const size_t idx,
                        T &item) const
    {
        OFCondition result = EC_IllegalParameter;
        OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
        if (gotoItemPos(idx, iterator))
        {
            item = *iterator;
            result = EC_Normal;
        }
        return result;
    }

    /** add item to the list
     ** @param  item  item to be added
     */
    inline void addItem(const T &item)
    {
        ItemList.push_back(item);
    }

    /** add item to the list only if it's not already contained
     ** @param  item  item to be added
     */
    inline void addOnlyNewItem(const T &item)
    {
        if (!isElement(item))
            ItemList.push_back(item);
    }

    /** insert item at specified position to the list
     ** @param  idx   index of the item before the new one should be inserted (starting from 1)
     *  @param  item  item to be inserted
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition insertItem(const size_t idx,
                           const T &item)
    {
        OFCondition result = EC_IllegalParameter;
        if (idx == ItemList.size() + 1)
        {
            /* append to the end of the list */
            ItemList.push_back(item);
            result = EC_Normal;
        } else {
            OFLIST_TYPENAME OFListIterator(T) iterator = ItemList.begin();
            if (gotoItemPos(idx, iterator))
            {
                ItemList.insert(iterator, 1, item);
                result = EC_Normal;
            }
        }
        return result;
    }

    /** remove item from the list
     ** @param  idx  index of the item to be removed (starting from 1)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const size_t idx)
    {
        OFCondition result = EC_IllegalParameter;
        OFLIST_TYPENAME OFListIterator(T) iterator = ItemList.begin();
        if (gotoItemPos(idx, iterator))
        {
            ItemList.erase(iterator);
            result = EC_Normal;
        }
        return result;
    }

    /// default item which is returned in getItem() if the index is invalid.
    /// This static member variable needs to be defined (not only declared)
    /// in each derived class.
    static const T EmptyItem;


  protected:

    /** goto specified item position
     ** @param  idx       index of the item to go to (starting from 1)
     *  @param  iterator  list iterator storing the positition of the item
     ** @return OFTrue if specified item was found, OFFalse otherwise
     */
    OFBool gotoItemPos(size_t idx,
                       OFLIST_TYPENAME OFListConstIterator(T) &iterator) const
    {
        OFBool result = OFFalse;
        if (idx > 0)
        {
            const OFLIST_TYPENAME OFListConstIterator(T) endPos = ItemList.end();
            while ((--idx > 0) && (iterator != endPos))
                iterator++;
            /* index found? */
            result = (idx == 0);
        }
        return result;
    }

    /** goto specified item
     ** @param  item      value of the item to go to (starting from 1)
     *  @param  iterator  list iterator storing the positition of the item
     ** @return OFTrue if specified item was found, OFFalse otherwise
     */
    OFBool gotoItem(const T &item,
                    OFLIST_TYPENAME OFListConstIterator(T) &iterator) const
    {
        const OFLIST_TYPENAME OFListConstIterator(T) endPos = ItemList.end();
        /* operator== is used to reduce requirements for class T */
        while ((iterator != endPos) && (!(*iterator == item)))
            iterator++;
        return (iterator != endPos);
    }

  protected:

    /// the list maintained by this class
    OFList<T> ItemList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtlist.h,v $
 *  Revision 1.14  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2005-12-08 16:05:27  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2003/08/07 12:55:13  joergr
 *  Updated documentation to get rid of doxygen warnings.
 *
 *  Revision 1.11  2003/07/11 13:44:00  joergr
 *  Added workaround to get rid of "implicit typename" warnings on gcc 3.x
 *  (introduced macro OFLIST_TYPENAME).
 *
 *  Revision 1.10  2003/06/04 12:40:01  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.9  2003/06/03 10:16:44  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.8  2001/10/10 15:27:41  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.7  2001/09/26 13:04:13  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.6  2001/05/07 16:13:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.5  2001/01/25 11:48:11  joergr
 *  Added method to insert item into a list.
 *
 *  Revision 1.4  2000/12/12 14:17:13  joergr
 *  Renamed method to avoid ambiguity reported by gcc 2.7.
 *
 *  Revision 1.3  2000/10/26 14:19:38  joergr
 *  Fixed bug: index in search routine was starting from 0 not 1.
 *
 *  Revision 1.2  2000/10/18 17:08:44  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:34  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
