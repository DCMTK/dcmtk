/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Defines a template map class with interfaces similar to the C++
 *           Standard
 *
 */

#ifndef OFMAP_H
#define OFMAP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if defined(HAVE_STL) || defined(HAVE_STL_MAP)
// it is possible to use the standard template library map class since the
// interface is nearly identical.
#include <map>
#define OFMap std::map
#define OFPair std::pair
#define OFMake_pair std::make_pair
#else

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/oflist.h"

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif


/** a pair - this implements parts of std::pair's interface.
 */
template<typename K, typename V> class OFPair
{
public:

    /** this is the first value of the pair */
    K first;

    /** this is the second value of the pair */
    V second;

    /** default constructor */
    OFPair() : first(), second() { }

    /** construct a OFPair for the two given values
     *  @param f the value for first.
     *  @param s the value for second.
     */
    OFPair(const K& f, const V& s) : first(f), second(s) { }

    /** copy constructor
     *  @param p Other OFPair to copy from.
     */
    template<class OK, class OV>
    OFPair(const OFPair<OK, OV>& p) : first(p.first), second(p.second) { }

    /** assignment operator */
    OFPair<K, V>& operator=(const OFPair<K, V>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }
};

/** helper function to create a pair. This is similar to std::make_pair()
 *  @param first the first part of the pair
 *  @param second the second art of the pair
 *  @return the pair (first, second)
 */
template<typename K, typename V>
OFPair<K, V> OFMake_pair(const K& first, const V& second)
{
    return OFPair<K, V>(first, second);
}

/** associative container class. This class is a subset of std::map.
 */
template<typename K, typename V> class OFMap
{
public:

    /** the type of values saved in this map */
    typedef OFPair<const K, V> value_type;

protected:

    /// @todo using a list as base is slooooow
    OFList<value_type> values_;

public:

    /** iterator class for OFMap. You can modify elements through this
     *  iterator's ->first and ->second properties.
     */
    typedef OFListIterator(value_type) iterator;

    /** constant iterator class for OFMap. You can read the elements, but you may
     *  not modify them.
     */
    typedef OFListConstIterator(value_type) const_iterator;

    /** default constructor */
    OFMap() : values_() { }

    /** assignment operator */
    OFMap& operator=(const OFMap& other)
    {
        if (this == &other)
            return *this;

        clear();

        for (const_iterator it = other.begin();
                it != other.end(); it++)
            insert(*it);

        return *this;
    }

    /** index operator. You can use this to add new elements to the map.
     *  Beware: Don't use this for checking if a given key is already used in
     *  the map, use find() != end() for this job!
     *  @param key the key you want to access
     *  @return reference to the value saved under the given key.
     */
    V& operator[](const K& key)
    {
        iterator it = find(key);
        if (it == end())
        {
            it = insert(value_type(key, V())).first;
        }

        return it->second;
    }

    /** returns iterator pointing to the first element of this map
     *  @return iterator pointing to the first element of this map
     */
    iterator begin() { return values_.begin(); }

    /** returns iterator pointer after the last element of this map
     *  @return iterator pointer after the last element of this map
     */
    iterator end() { return values_.end(); }

    /** returns constant iterator pointing to the first element of this map
     *  @return constant iterator pointing to the first element of this map
     */
    const_iterator begin() const { return values_.begin(); }

    /** returns constant iterator pointer after the last element of this map
     *  @return constant iterator pointer after the last element of this map
     */
    const_iterator end() const { return values_.end(); }

    /** looks up the given key in this map
     *  @param key the key to look for
     *  @return iterator for that key to end()
     */
    iterator find(const K& key)
    {
        iterator it = begin();
        while (it != end())
        {
            if (it->first == key)
                break;
            it++;
        }

        return it;
    }

    /** looks up the given key in this map
     *  @param key the key to look for
     *  @return constant iterator for that key to end()
     */
    const_iterator find(const K& key) const
    {
        const_iterator it = begin();
        while (it != end())
        {
            if (it->first == key)
                break;
            it++;
        }

        return it;
    }

    /** returns the number of elements saved in this map
     *  @return the number of elements saved in this map
     */
    size_t size() const { return values_.size(); }

    /** removes all elements from this map */
    void clear() { values_.clear(); }

    /** removes all elements in the given range from this map
     *  @param first the first element to remove
     *  @param last the first element NOT to remove
     */
    void erase(const iterator& first, const iterator& last)
    {
        values_.erase(first, last);
    }

    /** removes the element to which the given iterator points to
     *  @param elem the element to remove
     */
    void erase(const iterator& elem)
    {
        values_.erase(elem);
    }

    /** removes the element with the given key from this map
     *  @return the number of elements removed (0 or 1)
     */
    int erase(const K& key)
    {
        iterator it = find(key);
        if (it == end())
            return 0;

        values_.erase(it);
        return 1;
    }

    /** inserts a new element into the map, but does not overwrite existing
     *  elements
     *  @param val the value to insert
     *  @return a pair of an iterator and a boolean. The iterator always points
     *  to the element which got val's key. The boolean is true if val was
     *  inserted, false otherwise.
     */
    OFPair<iterator, bool> insert(const value_type& val)
    {
        OFListIterator(value_type) it = find(val.first);
        if (it != end())
            return OFMake_pair(it, false);

        it = values_.insert(values_.end(), val);
        return OFMake_pair(it, true);
    }

    /** swaps the contents of the two maps. The time complexity of this
     *  function should be constant.
     *  @param s map to swap with
     */
    void swap(OFMap<K, V>& s)
    {
        OFList<value_type> own_values = values_;
        values_ = s.values_;
        s.values_ = own_values;
    }
};

#endif
#endif
