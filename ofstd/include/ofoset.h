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
 *  Purpose: Template class for administrating an ordered set of elements
 *           of an arbitrary type.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-07-02 15:19:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofoset.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFOrderedSet_h
#define OFOrderedSet_h

#include "ofset.h"

/** This template class provides a data structure and operations for administrating an
 *  ordered set of elements of an arbitrary type. Note the following properties of this
 *  class:
 *   - an element which is added to the set will be copied
 *   - the datatype of the set's elements has to support operator== so that it is possible
 *     to find a certain element
 *   - it is allowed to add identical elements to the set
 *   - if a user requires to remove a certain element and if there are several elements
 *     which are identical to this element, only one element will be removed from the set
 *   - when removing an element, the indeces of the elements behind the removed element will
 *     be reduced by one
 *   - the set will be ordered according to the point in time at which an element is added
 *     to the set; a new element will always be added at the end of the set
 */
template <class T> class OFOrderedSet : public OFSet<T>
{
  protected:

  public:
      /** Default constructor.
       */
    OFOrderedSet()
        : OFSet<T>()
      {
      }


      /** Copy constructor.
       *  @param src Source object of which this will be a copy.
       */
    OFOrderedSet( const OFOrderedSet<T> &src )
        : OFSet<T>( src )
      {
      }


      /** Destructor.
       */
    virtual ~OFOrderedSet()
      {
      }


      /** operator=.
       *  @param src Source object whose values will be assigned to this.
       *  @return Reference to this.
       */
    const OFOrderedSet<T> &operator=( const OFOrderedSet<T> &src )
      {
        if( this == &src )
          return( *this );

        OFSet<T>::operator=( src );

        return( *this );
      }


      /** Determines if two sets are identical. Note that for ordered sets
       *  not only their elements have to be identical, but also the order
       *  of their elements has to be identical.
       *  @param other Set which shall be compared with this.
       *  @return OFTrue if sets are identical, OFFalse otherwise.
       */
    virtual OFBool operator==( const OFOrderedSet<T> &other ) const
      {
        // check if both sets contain the same
        // amount of items; if not, return OFFalse
        if( num != other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // as long as result is OFTrue go through all items in this
        for( unsigned int i=0 ; i<num && result == OFTrue ; i++ )
        {
          // in case the current element does not equal the current
          // element in other, result shall be set to OFFalse
          if( items[i] != other.items[i] )
            result = OFFalse;
        }

        // return result
        return( result );
      }


      /** Determines if two sets are not identical.
       *  @param other Set which shall be compared with this.
       *  @return OFTrue if sets are not identical, OFFalse otherwise.
       */
    virtual OFBool operator!=( const OFOrderedSet<T> &other ) const
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
    virtual void Add( const OFOrderedSet<T> &other )
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

            // and - so that there are no holes in the array - move all elements
            // behind the current element up one array field; only do so in case
            // we did _not_ delete the last item
            if( i != num - 1 )
            {
              for( unsigned j=i+1 ; j<num ; j++ )
                items[j-1] = items[j];

              items[j-1] = NULL;
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

          // and - so that there are no holes in the array - move all elements
          // behind the current element up one array field; only do so in case
          // we did _not_ delete the last item
          if( index != num - 1 )
          {
            for( unsigned j=index+1 ; j<num ; j++ )
              items[j-1] = items[j];

            items[j-1] = NULL;
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
    virtual OFBool IsSupersetOf( const OFOrderedSet<T> &other ) const
      {
        // if this contains less or the same amount of items than other, return OFFalse
        if( num <= other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // make a copy of this
        OFOrderedSet<T> set1 = *this;

        // make a copy of other
        OFOrderedSet<T> set2 = other;

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
    virtual OFBool IsSubsetOf( const OFOrderedSet<T> &other ) const
      {
        return( other.IsSupersetOf( *this ) );
      }
};

#endif

/*
** CVS/RCS Log:
** $Log: ofoset.h,v $
** Revision 1.1  2002-07-02 15:19:54  wilkens
** Added container classes OFOrderedSet and OFUnorderedSet which
** are based on the new abstract class OFSet.
**
**
*/
