/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Purpose: Template class for administrating a set of elements of an
 *           arbitrary type.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFSET_H
#define OFSET_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"

#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"

#define STARTING_SIZE 8

/** This abstract template class provides a data structure and operations for administrating a
 *  set of elements of an arbitrary type.
 */
template <class T> class OFSet
{
  protected:
    /// array containing the entries
    T **items;
    /// number of entries in the set
    unsigned int num;
    /// current size of the set (always >= num)
    unsigned int size;

  public:
      /** Default constructor.
       */
    OFSet()
        : items( new T*[ STARTING_SIZE ] ), num( 0 ), size( STARTING_SIZE )
      {
        init();
      }


      /** This function is a workaround for avoiding a compiler warning on
       *  Solaris 2.5.1 using compiler SC 2.0.1.
       */
    void init()
      {
        for( unsigned i=0 ; i<size ; i++ )
          items[i] = NULL;
      }


      /** Copy constructor.
       *  @param src Source object of which this will be a copy.
       */
    OFSet( const OFSet<T> &src )
        : items( NULL ), num ( src.num ), size ( src.size )
      {
        init( src );
      }


      /** This function is a workaround for avoiding a compiler warning on
       *  Solaris 2.5.1 using compiler SC 2.0.1.
       */
    void init( const OFSet<T> &src )
      {
        items = new T*[size];
        for( unsigned int i=0 ; i<size ; i++ )
        {
          if( i<num )
            items[i] = new T( *src.items[i] );
          else
            items[i] = NULL;
        }
      }


      /** Destructor.
       */
    virtual ~OFSet()
      {
        for( unsigned int i=0 ; i<num ; i++ )
          delete items[i];
        delete[] items;
      }


      /** operator=.
       *  @param src Source object whose values will be assigned to this.
       *  @return Reference to this.
       */
    const OFSet<T> &operator=( const OFSet<T> &src )
      {
        if( this == &src )
          return( *this );

        unsigned int i;

        for( i=0 ; i<num ; i++ )
          delete items[i];
        delete[] items;

        num = src.num;
        size = src.size;
        items = new T*[size];
        for( i=0 ; i<size ; i++ )
        {
          if( i<num )
            items[i] = new T( *src.items[i] );
          else
            items[i] = NULL;
        }

        return( *this );
      }



      /** Returns a certain element which is contained in the set. Note that the
       *  original object which actually is contained in the set will be returned,
       *  and not a copy of the object. Further note that if the specified index
       *  is out of range, a newly default constructed object of class T will be
       *  returned.
       *  @return A certain element (original) which is contained in the set.
       */
    virtual T &operator[]( unsigned int i ) const
      {
        if( i<num )
          return( *items[i] );
        else
        {
          T *obj = new T();
          T &ret = *obj;
          return( ret );
        }
      }


      /** Resizes the set. If newSize is lower than the current number
       *  of elements in the set, this function doesn't do anything.
       *  @param newSize New size the set is supposed to have.
       */
    virtual void Resize( unsigned int newSize )
      {
        unsigned int i;

        if( newSize >= num )
        {
          T **tmp = new T*[newSize];

          for( i=0 ; i<newSize ; i++ )
          {
            if( i<num )
              tmp[i] = items[i];
            else
              tmp[i] = NULL;
          }

          delete[] items;
          items = tmp;

          size = newSize;
        }
      }


      /** Removes all items from the set.
       */
    virtual void Clear()
      {
        for( unsigned int i=0 ; i<num ; i++ )
        {
          delete items[i];
          items[i] = NULL;
        }

        num = 0;
      }


      /** Determines if the set is an empty set.
       *  @return OFTrue, if the set is an empty set, OFFalse otherwise.
       */
    virtual OFBool IsEmpty() const
      {
        if( num == 0 )
          return( OFTrue );
        else
          return( OFFalse );
      }


      /** Returns the number of elements in the set.
       *  @return Number of elements in the set.
       */
    virtual unsigned int NumberOfElements() const
      {
        return( num );
      }


      /** Inserts a new item into the set.
       *  @param item Item which shall be inserted into the set.
       */
    virtual void Insert( const T &item ) = 0;


      /** Removes one item from the set.
       *  @param item Item which shall be removed from the set.
       */
    virtual void Remove( const T &item ) = 0;


      /** Removes one item from the set.
       *  @param idx Index of the item which shall be removed from the set.
       */
    virtual void RemoveByIndex( unsigned int idx ) = 0;


      /** Tries to find a given object in the set. In case the specified object could
       *  be found, a pointer to the corresponding element within the set is returned;
       *  in case the specified object could not be found, NULL will be returned.
       *  @param item Search pattern.
       *  @return Pointer to the corresponding element within the set or NULL.
       */
    virtual T *Find( const T &item ) const = 0;


      /** Determines if a certain item is contained in the set.
       *  @param item - Item which shall be looked for.
       *  @return OFTrue, if item is contained in the set, OFFalse otherwise.
       */
    virtual OFBool Contains( const T &item ) const = 0;
};


#endif

/*
** CVS/RCS Log:
** $Log: ofset.h,v $
** Revision 1.13  2010-10-14 13:15:50  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2010-10-05 08:36:51  joergr
** Fixed various Doxygen API documentation issues.
**
** Revision 1.11  2010-03-01 09:08:51  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.10  2005-12-08 16:06:01  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2005/07/01 10:01:50  wilkens
** Modified a couple of "delete" statements to "delete[]" in order to get rid of
** valgrind's "Mismatched free() / delete / delete []" error messages.
**
** Revision 1.8  2002/12/18 09:06:41  wilkens
** Had forgotten to delete some superfluous code. Did it now.
**
** Revision 1.7  2002/12/17 17:01:34  wilkens
** Modified code again to keep Sun CC 2.0.1 happy on Solaris 2.5.1 (template
** errors).
**
** Revision 1.6  2002/12/16 10:40:25  wilkens
** Removed superfluous implementation files and modified header and make files.
**
** Revision 1.5  2002/12/13 12:26:51  wilkens
** Modified code to keep Sun CC 2.0.1 happy on Solaris 2.5.1 (template errors).
**
** Revision 1.4  2002/12/09 13:07:03  joergr
** Renamed parameter to avoid name clash with global function index().
** Initialize member variables in the member initialization list.
**
** Revision 1.3  2002/07/09 18:29:46  wilkens
** Added some more functionality.
**
**
*/
