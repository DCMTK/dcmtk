/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRListOfItems
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:34 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTLIST_H
#define DSRTLIST_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "oflist.h"

#include "dcerror.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to store and access items in a list structure.
 *  For instances of this class T needs to have at least a default constructor,
 *  assignment and comparison operator.
 */
template<class T>
class DSRListOfItems
  : protected OFList<T>
{

  public:

    DSRListOfItems()
      : OFList<T>()
    {
    }

    DSRListOfItems(const DSRListOfItems<T> &list)
      : OFList<T>(list)
    {
    }

    virtual ~DSRListOfItems()
    {
    }

    DSRListOfItems<T> &operator=(const DSRListOfItems<T> &list)
    {
        /* class OFList has no overloaded assignment operator */
        OFList<T>::clear();
        const OFListIterator(T) endPos = list.end();
        OFListIterator(T) iterator = list.begin();
        while (iterator != endPos)
        {
            OFList<T>::push_back(*iterator);
            iterator++;
        }
        return *this;
    }

    void clear()
    {
        OFList<T>::clear();
    }

    OFBool isEmpty() const
    {
        return OFList<T>::empty();
    }

    size_t getNumberOfItems() const
    {
        return OFList<T>::size();
    }

    OFBool isElement(const T &item) const
    {
        OFListIterator(T) iterator = OFList<T>::begin();
        return gotoItem(item, iterator);
    }

    const T &getItem(const size_t idx) const
    {
        OFListIterator(T) iterator = OFList<T>::begin();
        if (gotoItem(idx, iterator))
            return *iterator;
        else
            return EmptyItem;
    }

    E_Condition getItem(const size_t idx,
                        T &item) const
    {
        E_Condition result = EC_IllegalCall;
        OFListIterator(T) iterator = OFList<T>::begin();
        if (gotoItem(idx, iterator))
        {
            item = *iterator;
            result = EC_Normal;
        }
/*
         else
            item.clear();
*/
        return result;
    }

    E_Condition removeItem(const size_t idx)
    {
        E_Condition result = EC_IllegalCall;
        OFListIterator(T) iterator = OFList<T>::begin();
        if (gotoItem(idx, iterator))
        {
            OFList<T>::erase(iterator);
            result = EC_Normal;
        }
        return result;
    }

    void addItem(const T &item)
    {
        OFList<T>::push_back(item);
    }

    void addOnlyNewItem(const T &item)
    {
        if (!isElement(item))
            OFList<T>::push_back(item);
    }


    static const T EmptyItem;


  protected:

    OFBool gotoItem(size_t idx,
                    OFListIterator(T) &iterator) const
    {
        OFBool result = OFFalse;
        if (idx > 0)
        {
            const OFListIterator(T) endPos = OFList<T>::end();
            while ((idx > 0) && (iterator != endPos))
            {
                idx--;
                iterator++;
            }
            result = (idx == 0);
        }
        return result;
    }


    OFBool gotoItem(const T &item,
                    OFListIterator(T) &iterator) const
    {
        const OFListIterator(T) endPos = OFList<T>::end();
        /* operator== is used to reduce requirement for class T */
        while ((iterator != endPos) && (!(*iterator == item)))
            iterator++;
        return (iterator != endPos);
    }
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtlist.h,v $
 *  Revision 1.1  2000-10-13 07:49:34  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
