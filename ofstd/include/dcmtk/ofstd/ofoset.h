/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Template class for administrating an ordered set of elements
 *           of an arbitrary type.
 *
 */

#ifndef OFOrderedSet_h
#define OFOrderedSet_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofset.h"

/** This template class provides a data structure and operations for administrating an
 *  ordered set of elements of an arbitrary type. Note the following properties of this
 *  class:
 *   - an element which is inserted into the set will be copied
 *   - the datatype of the set's elements has to support operator== so that it is possible
 *     to find a certain element
 *   - it is allowed to insert identical elements into the set
 *   - if a user requires to remove a certain element and if there are several elements
 *     which are identical to this element, only one element will be removed from the set
 *   - when removing an element, the indeces of the elements behind the removed element will
 *     be reduced by one
 *   - the set will be ordered according to the point in time at which an element is inserted
 *     into the set; a new element will always be inserted at the end of the set
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
        return( assign( src ) );
      }


      /** This function is a workaround for avoiding a compiler warning on
       *  Solaris 2.5.1 using compiler SC 2.0.1.
       */
    const OFOrderedSet<T> &assign( const OFOrderedSet<T> &src )
      {
        if( this != &src )
          this->operator=( src );

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
        if( this->num != other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // as long as result is OFTrue go through all items in this
        for( unsigned int i=0 ; i < this->num && result == OFTrue ; i++ )
        {
          // in case the current element does not equal the current
          // element in other, result shall be set to OFFalse
          if( *this->items[i] != *other.items[i] )
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


      /** Inserts a new item into the set.
       *  @param item Item which shall be inserted into the set.
       */
    virtual void Insert( const T &item )
      {
        // if size equals num, we need more space
        if( this->size == this->num )
          this->Resize( this->size * 2 );

        // copy item
        T *newItem = new T( item );

        // insert copy into array
        this->items[this->num] = newItem;

        // increase counter
        this->num++;
      }


      /** Inserts all items of another set into this set.
       *  @param other set whose items shall be inserted into the set.
       */
    virtual void Insert( const OFOrderedSet<T> &other )
      {
        // go through all items in other and insert each item into this
        for( unsigned int i=0 ; i<other.num ; i++ )
          Insert( *other.items[i] );
      }


      /** Inserts a new item at a certain position into the set.
       *  @param item Item which shall be inserted into the set.
       *  @param idx  Index of the position at which the item shall be inserted.
       *              The first position has index 0. Note that in case index
       *              is greater than the index of the last item, the new item will
       *              be inserted right behind the last item of the set.
       */
    virtual void InsertAt( const T &item, unsigned int idx )
      {
        unsigned int i;

        // in case index is greater than the index of the last item,
        // insert the new item right behind the last item of the set
        if( idx > this->num - 1 )
          Insert( item );
        else
        {
          // if size equals num, we need more space
          if( this->size == this->num )
            this->Resize( this->size * 2 );

          // copy item
          T *newItem = new T( item );

          // create a new temporary array and assign all pointers correspondingly
          T **tmp = new T*[this->size];

          for( i=0 ; i<idx ; i++ )
            tmp[i] = this->items[i];

          tmp[idx] = newItem;

          for( i=idx ; i < this->size - 1 ; i++ )
          {
            if( i < this->num )
              tmp[i+1] = this->items[i];
            else
              tmp[i+1] = NULL;
          }

          // delete old array
          delete this->items;

          // assign new array to member variable
          this->items = tmp;

          // increase counter
          this->num++;
        }
      }


      /** Removes one item from the set.
       *  @param item Item which shall be inserted into the set.
       */
    virtual void Remove( const T &item )
      {
        // so far, nothing was deleted
        OFBool itemDeleted = OFFalse;

        // go through all items
        for( unsigned int i=0 ; i < this->num && !itemDeleted ; i++ )
        {
          // if current item is the one which shall be deleted
          if( *this->items[i] == item )
          {
            // delete item
            delete this->items[i];

            // and - so that there are no holes in the array - move all elements
            // behind the current element up one array field; only do so in case
            // we did _not_ delete the last item
            if( i != this->num - 1 )
            {
              unsigned int j;
              for( j=i+1 ; j < this->num ; j++ )
              {
                this->items[j-1] = this->items[j];
              }

              this->items[j-1] = NULL;
            }
            else
              this->items[i] = NULL;

            // reduce counter
            this->num--;

            // remember that an item was deleted (so that always only one item will be deleted)
            itemDeleted = OFTrue;
          }
        }
      }


      /** Removes one item from the set.
       *  @param idx Index of the item which shall be removed from the set.
       */
    virtual void RemoveByIndex( unsigned int idx )
      {
        // do something only if the given index is not out of range
        if( idx < this->num )
        {
          // delete item with given index
          delete this->items[idx];

          // and - so that there are no holes in the array - move all elements
          // behind the current element up one array field; only do so in case
          // we did _not_ delete the last item
          if( idx != this->num - 1 )
          {
            unsigned int j;
            for( j=idx+1 ; j < this->num ; j++ )
            {
              this->items[j-1] = this->items[j];
            }

            this->items[j-1] = NULL;
          }
          else
            this->items[idx] = NULL;

          // reduce counter
          this->num--;
        }
      }


      /** Tries to find a given object in the set. In case the specified object could
       *  be found, a pointer to the corresponding element within the set is returned;
       *  in case the specified object could not be found, NULL will be returned.
       *  @param item Search pattern.
       *  @return Pointer to the corresponding element within the set or NULL.
       */
    virtual T *Find( const T &item ) const
      {
        unsigned int i;
        OFBool itemFound = OFFalse;

        for( i=0 ; i < this->num && !itemFound ; i++ )
        {
          if( *this->items[i] == item )
            itemFound = OFTrue;
        }

        if( itemFound )
          return( this->items[i-1] );
        else
          return( NULL );
      }


      /** Determines if a certain item is contained in the set.
       *  @param item - Item which shall be looked for.
       *  @return OFTrue, if item is contained in the set, OFFalse otherwise.
       */
    virtual OFBool Contains( const T &item ) const
      {
        OFBool itemFound = OFFalse;

        for( unsigned int i=0 ; i < this->num && !itemFound ; i++ )
        {
          if( *this->items[i] == item )
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
        if( this->num <= other.num )
          return( OFFalse );

        // initialize result with OFTrue
        OFBool result = OFTrue;

        // make a copy of this
        OFOrderedSet<T> s = *this;

        // as long as result is OFTrue go through all items in other
        for( unsigned int i=0 ; i<other.num && result == OFTrue ; i++ )
        {
          // in case s contains the current item of other
          if( s.Contains( *other.items[i] ) )
          {
            // remove this item from s so that it will not be
            // considered again in a later call to s.Contains()
            s.Remove( *other.items[i] );
          }
          // in case s does not contain the current item of other the result is OFFalse
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


      /** Determines the union of the two sets this and other, i.e. the set
       *  containing all items which can be found either in this or in other,
       *  and returns the resulting new set.
       *  @param other Second parameter for union.
       *  @return New set.
       */
    OFOrderedSet<T> Union( const OFOrderedSet<T> &other ) const
      {
        // initialize result set
        OFOrderedSet<T> resultSet = *this;

        // insert other set into result set
        resultSet.Insert( other );

        // return result set
        return( resultSet );
      }


      /** Determines the intersection of the two sets this and other, i.e. the set
       *  containing all items which can be found in both this and other, and
       *  returns the resulting new set.
       *  @param other Second parameter for intersection.
       *  @return New set.
       */
    OFOrderedSet<T> Intersection( const OFOrderedSet<T> &other ) const
      {
        // initialize result set
        OFOrderedSet<T> resultSet;

        // make a copy of other
        OFOrderedSet<T> s = other;

        // go through all items in this
        for( unsigned int i=0 ; i < this->num ; i++ )
        {
          // if s contains the current item
          if( s.Contains( *this->items[i] ) )
          {
            // insert the item into the result set
            resultSet.Insert( *this->items[i] );

            // and remove the item from s so that it will not be
            // considered again in a later call to s.Contains()
            s.Remove( *this->items[i] );
          }
        }

        // return result set
        return( resultSet );
      }


      /** Determines the difference this - other, i.e. the set containing all
       *  the items found in this but not in other, and returns the resulting
       *  new set.
       *  @param other Second parameter for difference.
       *  @return New set.
       */
    OFOrderedSet<T> Difference( const OFOrderedSet<T> &other ) const
      {
        // initialize result set
        OFOrderedSet<T> resultSet;

        // make a copy of other
        OFOrderedSet<T> s = other;

        // go through all items in this
        for( unsigned int i=0 ; i < this->num ; i++ )
        {
          // if s does not contain the current item
          if( !s.Contains( *this->items[i] ) )
          {
            // insert the item into the result set
            resultSet.Insert( *this->items[i] );
          }
          else
          {
            // else remove the item from s so that it will not be
            // considered again in a later call to s.Contains()
            s.Remove( *this->items[i] );
          }
        }

        // return result set
        return( resultSet );
      }


      /** Determines the symmetric difference of this and other, i.e. the set
       *  containing all the items which can be found either in this or in other
       *  but not in the intersection of this and other, and returns the resulting
       *  new set.
       *  @param other Second parameter for symmetric difference.
       *  @return New set.
       */
    OFOrderedSet<T> SymmetricDifference( const OFOrderedSet<T> &other ) const
      {
        // determine s1 = this - other
        OFOrderedSet<T> s1 = (*this).Difference( other );

        // determine s2 = other - this
        OFOrderedSet<T> s2 = other.Difference( *this );

        // determine the union of s1 and s2
        OFOrderedSet<T> resultSet = s1.Union( s2 );

        // return result set
        return( resultSet );
      }
};


#endif
