/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 */

#ifndef DCMSMAP_H
#define DCMSMAP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"   /* for class OFList<> */
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */


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

  /** return key of key-value pair
   *  @return key as const reference
   */
  const OFString& key() const
  {
    return key_;
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
    OFLIST_TYPENAME OFListIterator(DcmKeyValuePair<T> *) first(list_.begin());
    OFLIST_TYPENAME OFListIterator(DcmKeyValuePair<T> *) last(list_.end());
    while (first != last)
    {
      delete (*first);
      first = list_.erase(first);
    }
  }

  /** inserts a new key-value pair into the map
   *  by means of copy construction.
   *  @param key key
   *  @param value value
   *  @return true if successful, false if failed because key already present
   */
  OFBool add(const OFString& key, const T& value)
  {
    OFBool result = OFFalse;
    if (! lookup(key))
    {
      list_.push_back(new DcmKeyValuePair<T>(key, value));
      result = OFTrue;
    }
    return result;
  }

  /** looks up the given key in the map.
   *  @param key key
   *  @return pointer to value if found, NULL otherwise.
   */
  const T *lookup(const OFString& key) const
  {
    OFLIST_TYPENAME OFListConstIterator(DcmKeyValuePair<T> *) first(list_.begin());
    OFLIST_TYPENAME OFListConstIterator(DcmKeyValuePair<T> *) last(list_.end());
    while (first != last)
    {
      if ((*first)->matches(key)) return &((*first)->value());
      ++first;
    }
    return NULL;
  }

  /** return iterator to first element in list
   */
  OFLIST_TYPENAME OFListIterator( DcmKeyValuePair<T> * ) begin()
  {
    return list_.begin();
  }

  /** return iterator to end of list
   */
  OFLIST_TYPENAME OFListIterator( DcmKeyValuePair<T> * ) end()
  {
    return list_.end();
  }

private:
  /// private undefined copy constructor
  DcmSimpleMap(const DcmSimpleMap& arg);

  /// private undefined copy assignment operator
  DcmSimpleMap& operator=(const DcmSimpleMap& arg);

  /// the list of key-value pairs
  OFList<DcmKeyValuePair<T> *> list_;

};

#endif
