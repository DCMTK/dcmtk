/*
 *
 *  Copyright (C) 1994-2003, OFFIS
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    template <class T> class DcmKeyValuePair
 *    template <class T> class DcmSimpleMap
 *    these template classes implement a simple map of key-value pairs.
 *    The template type must be copy constructable.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-07-03 15:56:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dcmsmap.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMSMAP_H
#define DCMSMAP_H

#include "osconfig.h"
#include "oflist.h"   /* for class OFList<> */
#include "ofstring.h" /* for class OFString */


/** this template class implements a simple key - value pair map
 *  that is used in template class DcmSimpleMap. Template type must be
 *  copy constructable.
 */
template <class T> class DcmKeyValuePair
{
public:
  /** constructor
   *  @param k key
   *  @param v value
   */
  DcmKeyValuePair(const OFString& k, const T& v)
  : key_(k)
  , value_(v)
  {
  }

  /// copy constructor
  DcmKeyValuePair(const DcmKeyValuePair& arg)
  : key_(arg.key_)
  , value_(arg.value_)
  {
  }

  /// destructor
  ~DcmKeyValuePair()
  {
  }

  /** return value of key-value pair
   *  @return value as const reference
   */
  const T& value() const
  {
    return value_;
  }

  /** return value of key-value pair
   *  @return value as non-const reference
   */
  T& value()
  {
    return value_;
  }

  /** checks if this entry matches the given key
   *  @return true if key matches, false otherwise
   */
  OFBool matches(const OFString &key) const
  {
    return (key_ == key);
  }

  /** comparison operator.
   *  @param arg object to compare with
   *  @return true if equal
   */
  OFBool operator==(const DcmKeyValuePair& arg) const
  {
    return (key_ == arg.key_) && (value_ == arg.value_); 
  }

private:
  /// private undefined copy assignment operator
  DcmKeyValuePair& operator=(const DcmKeyValuePair& arg);

  /// the key
  OFString key_;

  /// the value
  T value_;
};


/** this class implements a simple map of key-value pairs.
 *  No double keys are permitted, the second attempt to insert
 *  an element with a key already present will fail.
 *  Template type must be copy constructable.
 */
template <class T> class DcmSimpleMap
{
public:
  /// default constructor
  DcmSimpleMap()
  : list_()
  {
  }

  /// destructor
  ~DcmSimpleMap()
  {
  }

  /** inserts a new key-value pair into the map
   *  by means of copy construction.
   *  @param key key
   *  @param value value
   *  @return true if successful, false if failed because key already present
   */
  OFBool add(const OFString& key, const T& value)
  {
    if (lookup(key)) return OFFalse;
    list_.push_back(DcmKeyValuePair<T>(key, value));
    return OFTrue;
  }

  /** looks up the given key in the map.
   *  @param key key
   *  @return pointer to value if found, NULL otherwise.
   */
  const T *lookup(const OFString& key) const
  {
#if defined(HAVE_TYPENAME) && (defined(HAVE_STL) || defined(HAVE_STL_LIST))
    typename 
#endif
    OFListConstIterator(DcmKeyValuePair<T>) first(list_.begin());
#if defined(HAVE_TYPENAME) && (defined(HAVE_STL) || defined(HAVE_STL_LIST))
    typename 
#endif
    OFListConstIterator(DcmKeyValuePair<T>) last(list_.end());
    while (first != last)
    {
      if ((*first).matches(key)) return &((*first).value());
      ++first;
    }
    return NULL;
  }

  /** return iterator to first element in list
   */
#if defined(HAVE_TYPENAME) && (defined(HAVE_STL) || defined(HAVE_STL_LIST))
  typename 
#endif
  OFListIterator( DcmKeyValuePair<T> ) begin()
  {
    return list_.begin();
  }

  /** return iterator to end of list
   */
#if defined(HAVE_TYPENAME) && (defined(HAVE_STL) || defined(HAVE_STL_LIST))
  typename 
#endif
  OFListIterator( DcmKeyValuePair<T> ) end()
  {
    return list_.end();
  }

private:
  /// private undefined copy constructor
  DcmSimpleMap(const DcmSimpleMap& arg);

  /// private undefined copy assignment operator
  DcmSimpleMap& operator=(const DcmSimpleMap& arg);

  /// the list of key-value pairs
  OFList<DcmKeyValuePair<T> > list_;

};

#endif

/*
 * CVS/RCS Log
 * $Log: dcmsmap.h,v $
 * Revision 1.3  2003-07-03 15:56:19  meichel
 * Introduced workaround for "implicit typename" warning on gcc 3.x when
 *   compiling with HAVE_STL.
 *
 * Revision 1.2  2003/06/18 08:16:17  meichel
 * Added comparison operators to keep MSVC5 compiler happy
 *
 * Revision 1.1  2003/06/10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
