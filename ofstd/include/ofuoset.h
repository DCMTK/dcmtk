/*
 *
 *  Copyright (C) 1997-2002, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Template class for administrating an unordered set of elements
 *           of an arbitrary type.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-07-02 15:19:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofuoset.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFUnorderedSet_h
#define OFUnorderedSet_h

#include "ofset.h"

/** This template class provides a data structure and operations for administrating an
 *  unordered set of elements of an arbitrary type. Note the following properties of this
 *  class:
 *  - an element which is added to the set will be copied
 *  - the datatype of the set's elements has to support operator== so that it is possible
 *    to find a certain element
 *  - it is allowed to add identical elements to the set
 *  - if a user requires to remove a certain element and if there are several elements
 *    which are identical to this element, only one element will be removed from the set
 *  - after removing an element of a set, the indeces of the set's elements might have
 *    changed; hence, looping over all elements of a set must be restarted if an element
 *    was removed from the set within the loop
 *  - the order of the elements is arbitrary, but it will not be changed unless an element
 *    is removed from the set
 */
template <class T> class OFUnorderedSet : public OFSet<T>
{
  protected:

  public:
      /** Default constructor.
       */
    OFUnorderedSet()
        : OFSet<T>()
      {
      }


      /** Copy constructor.
       *  @param src Source object of which this will be a copy.
       */
    OFUnorderedSet( const OFUnorderedSet<T> &src )
        : OFSet<T>( src )
      {
      }


      /** Destructor.
       */
    virtual ~OFUnorderedSet()
      {
      }


      /** operator=.
       *  @param src Source object whose values will be assigned to this.
       *  @return Reference to this.
       */
    const OFUnorderedSet<T> &operator=( const OFUnorderedSet<T> &src )
      {
        if( this == &src )
          return( *this );

        OFSet<T>::operator=( src );

        return( *this );
      }


      /** Determines if two sets are identical.
       *  @param other Set which shall be compared with this.
       *  @return OFTrue if sets are identical, OFFalse otherwise.
       */
    virtual OFBool operator==( const OFUnorderedSet<T> &other ) const
      {
        // check if both sets contain the same
        // amount of items; if not, return OFFalse
        if( num != other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // make a copy of this
        OFUnorderedSet<T> set1 = *this;

        // make a copy of other
        OFUnorderedSet<T> set2 = other;

        // as long as result is OFTrue go through all items in set2 (from
        // the last to the first item because then removing items in set2
        // will go together with this loop construction)
        for( unsigned int i=set2.num-1 ; i>=0 && result == OFTrue ; i-- )
        {
          // in case set1 contains the current item of set2
          if( set1.Contains( *set2.items[i] ) )
          {
            // remove this item from both sets (for set2 make sure to remove items[i])
            set1.Remove( *set2.items[i] );
            delete set2.items[i];
            set2.items[i] = NULL;
            set2.num--;
          }
          // in case set1 doesn't contain the current item of set2 the result is OFFalse
          else
            result = OFFalse;
        }

        // return result
        return( result );
      }


      /** Determines if two sets are not identical.
       *  @param other Set which shall be compared with this.
       *  @return OFTrue if sets are not identical, OFFalse otherwise.
       */
    virtual OFBool operator!=( const OFUnorderedSet<T> &other ) const
      {
        return( !( *this == other ) );
      }


      /** Adds a new item to the set.
       *  @param item Item which shall be added to the set.
       */
    virtual void Add( const T &item )
      {
        // if size equals num, we need more space
        if( size == num )
          Resize( size * 2 );

        // copy item
        T *newItem = new T( item );

        // add copy to array
        items[num] = newItem;

        // increase counter
        num++;
      }


      /** Adds all items of another set to this set.
       *  @param other set whose items shall be added to the set.
       */
    virtual void Add( const OFUnorderedSet<T> &other )
      {
        // go through all items in other and add each item to this
        for( unsigned int i=0 ; i<other.num ; i++ )
          Add( *other.items[i] );
      }


      /** Removes one item from the set.
       *  @param item Item which shall be added to the set.
       */
    virtual void Remove( const T &item )
      {
        // so far, nothing was deleted
        OFBool itemDeleted = OFFalse;

        // go through all items
        for( unsigned int i=0 ; i<num && !itemDeleted ; i++ )
        {
          // if current item is the one which shall be deleted
          if( *items[i] == item )
          {
            // delete item
            delete items[i];

            // and - so that there are no holes in the array - move the last
            // item to the array field from which the item was deleted;
            // only do so in case we did _not_ delete the last item
            if( i != num - 1 )
            {
              items[i] = items[num-1];
              items[num-1] = NULL;
            }
            else
              items[i] = NULL;

            // reduce counter
            num--;

            // remember that an item was deleted (so that always only one item will be deleted)
            itemDeleted = OFTrue;
          }
        }
      }


      /** Removes one item from the set.
       *  @param index Index of the item which shall be removed from the set.
       */
    virtual void RemoveByIndex( unsigned int index )
      {
        // do something only if the given index is not out of range
        if( index < num )
        {
          // delete item with given index
          delete items[index];

          // and - so that there are no holes in the array - move the last
          // item to the array field from which the item was deleted;
          // only do so in case we did _not_ delete the last item
          if( index != num - 1 )
          {
            items[index] = items[num-1];
            items[num-1] = NULL;
          }
          else
            items[index] = NULL;

          // reduce counter
          num--;
        }
      }


      /** Determines if a certain item is contained in the set.
       *  @param item - Item which shall be looked for.
       *  @return OFTrue, if item is contained in the set, OFFalse otherwise.
       */
    virtual OFBool Contains( const T &item ) const
      {
        OFBool itemFound = OFFalse;

        for( unsigned int i=0 ; i<num && !itemFound ; i++ )
        {
          if( *items[i] == item )
            itemFound = OFTrue;
        }

        return( itemFound );
      }


      /** Determines if this is an actual superset of other, i.e.
       *  if this completely contains other and furthermore has
       *  additional elements.
       *  @param other - Set which shall be compared with this.
       *  @return OFTrue if this is a superset of other, OFFalse otherwise.
       */
    virtual OFBool IsSupersetOf( const OFUnorderedSet<T> &other ) const
      {
        // if this contains less or the same amount of items than other, return OFFalse
        if( num <= other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // make a copy of this
        OFUnorderedSet<T> set1 = *this;

        // make a copy of other
        OFUnorderedSet<T> set2 = other;

        // as long as result is OFTrue go through all items in set2 (from
        // the last to the first item because then removing items in set2
        // will go together with this loop construction)
        for( unsigned int i=set2.num-1 ; i>=0 && result == OFTrue ; i-- )
        {
          // in case set1 contains the current item of set2
          if( set1.Contains( *set2.items[i] ) )
          {
            // remove this item from both sets (for set2 make sure to remove items[i])
            set1.Remove( *set2.items[i] );
            delete set2.items[i];
            set2.items[i] = NULL;
            set2.num--;
          }
          // in case set1 doesn't contain the current item of set2 the result is OFFalse
          else
            result = OFFalse;
        }

        // return result
        return( result );
      }


      /** Determines if this is an actual subset of other, i.e.
       *  if this is completely contained in other and other
       *  furthermore has additional elements.
       *  @param other - Set which shall be compared with this.
       *  @return OFTrue if this is a subset of other, OFFalse otherwise.
       */
    virtual OFBool IsSubsetOf( const OFUnorderedSet<T> &other ) const
      {
        return( other.IsSupersetOf( *this ) );
      }
};

#endif

/*
** CVS/RCS Log:
** $Log: ofuoset.h,v $
** Revision 1.1  2002-07-02 15:19:55  wilkens
** Added container classes OFOrderedSet and OFUnorderedSet which
** are based on the new abstract class OFSet.
**
**
*/
