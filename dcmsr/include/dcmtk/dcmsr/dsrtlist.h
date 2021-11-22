/*
 *
 *  Copyright (C) 2000-2018, OFFIS e.V.
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
 */


#ifndef DSRTLIST_H
#define DSRTLIST_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcerror.h"

#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofvector.h"


/** get the default item which is returned in DSRListOfItems::getItem() if the index is invalid.
 *  This function needs to be specialized and instantiated for each different use of DSRListOfItems.
 *  @tparam T the type of the object that will be returned.
 *  @return a reference to an object of type T.
 */
template<typename T>
const T& DSRgetEmptyItem();


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
        /* check for self-assignment, which would not work */
        if (this != &lst)
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
        }
        return *this;
    }

    /** comparison operator "equal"
     ** @param  lst  list that should be compared to the current one
     ** @return OFTrue if both lists are equal, OFFalse otherwise
     */
    OFBool operator==(const DSRListOfItems<T> &lst) const
    {
        /* first check whether the size of both lists is equal */
        OFBool result = (ItemList.size() == lst.ItemList.size());
        /* then iterate over all list entries (if any) */
        if (result && !ItemList.empty())
        {
            /* since OFList does not implement a comparison operator we need the following */
            const OFLIST_TYPENAME OFListConstIterator(T) endPos = ItemList.end();
            OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
            OFLIST_TYPENAME OFListConstIterator(T) lstIter = lst.ItemList.begin();
            do {
                result = (*iterator == *lstIter);
                iterator++;
                lstIter++;
            } while (result && (iterator != endPos));
        }
        return result;
    }

    /** comparison operator "not equal"
     ** @param  lst  list that should be compared to the current one
     ** @return OFTrue if both lists are not equal, OFFalse otherwise
     */
    OFBool operator!=(const DSRListOfItems<T> &lst) const
    {
        /* first check whether the size of both lists is not equal */
        OFBool result = (ItemList.size() != lst.ItemList.size());
        /* then iterate over all list entries (if any) */
        if (!result && !ItemList.empty())
        {
            /* since OFList does not implement a comparison operator we need the following */
            const OFLIST_TYPENAME OFListConstIterator(T) endPos = ItemList.end();
            OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
            OFLIST_TYPENAME OFListConstIterator(T) lstIter = lst.ItemList.begin();
            do {
                result = (*iterator != *lstIter);
                iterator++;
                lstIter++;
            } while (!result && (iterator != endPos));
        }
        return result;
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
            return DSRgetEmptyItem<T>();
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

    /** get copy of all items (as a vector)
     ** @param  items  reference to a variable where the result should be stored.
     *                 (always cleared before items are added)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItems(OFVector<T> &items) const
    {
        items.clear();
        if (!ItemList.empty())
        {
            /* avoid re-allocations */
            items.reserve(ItemList.size());
            /* iterate over all list items */
            const OFLIST_TYPENAME OFListConstIterator(T) endPos = ItemList.end();
            OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
            while (iterator != endPos)
            {
                /* and copy them to the passed vector */
                items.push_back(*iterator);
                iterator++;
            }
        }
        /* always return OK */
        return EC_Normal;
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

    /** add items to the list
     ** @param  items  items to be added (stored as a vector)
     */
    inline void addItems(const OFVector<T> &items)
    {
        const OFTypename OFVector<T>::const_iterator endPos = items.end();
        OFTypename OFVector<T>::const_iterator iterator = items.begin();
        while (iterator != endPos)
        {
            ItemList.push_back(*iterator);
            iterator++;
        }
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
        OFLIST_TYPENAME OFListConstIterator(T) iterator = ItemList.begin();
        if (gotoItemPos(idx, iterator))
        {
            ItemList.erase(iterator);
            result = EC_Normal;
        }
        return result;
    }


  protected:

    /** goto specified item position
     ** @param  idx       index of the item to go to (starting from 1)
     *  @param  iterator  list iterator storing the position of the item
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
     *  @param  iterator  list iterator storing the position of the item
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
