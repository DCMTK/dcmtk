/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
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
 *  Module:  dcmrt
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Template helper functions for searching through sequences
 *
 */

#ifndef DRMSRCH_H
#define DRMSRCH_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */


namespace DRTSearch {
    /**
     * Given a function predicate, this iterates over an IOD and finds the
     * first item for which the predicate returns true. The result will be the
     * IOD's currentItem(). If nothing is found, currentItem() will return the
     * EmptyDefaultItem.
     * @tparam IOD the type of the IOD that should be checked
     * @tparam Predicate the type of the search predicate
     * @param iod the IOD to search through
     * @param pred the predicate to check
     * @return EC_IllegalCall if search failed, else EC_Normal
     */
    template<typename IOD, typename Predicate>
    static inline OFCondition findItem(IOD& iod, Predicate& pred)
    {
        OFCondition cond = iod.gotoFirstItem();

        while (cond.good()) {
            if (pred(iod.getCurrentItem()))
                break;
            cond = iod.gotoNextItem();
        }

        return cond;
    }

    /**
     * This implements a search predicate for findItem which looks for some
     * object attribute via the equality operator.
     * @tparam IOD the type of the IOD that should be checked
     * @tparam Item this should always be IOD::Item. Sadly, MSC6 can't figure
     * that out by itself and so we must explicitly tell it about this type.
     * @tparam type the type that is being searched for
     * @tparam arg2 the type of the second argument to the getter function
     * @tparam argument2 the value of the second argument to the getter function
     */
    template<typename IOD, typename Item, typename type, typename arg2, arg2 argument2>
    class searchPropertyEqual {
    public:
        /// Type of the getter-function that is used for getting the value.
        typedef OFCondition (Item::*funcType)(type&, arg2) const;

        /**
         * Constructor
         * @param expected the expected value that we are searching for
         * @param func member function that gets us the value from the object
         */
        searchPropertyEqual(const type& expected, funcType func) : m_expected(expected), m_func(func) { }

        /// @return true if the given item has the wanted property
        OFBool operator()(const Item& item) {
            type value;
            OFCondition cond = (item.*m_func)(value, argument2);
            return cond.good() && value == m_expected;
        }

    private:
        /// The expected value of an item
        type m_expected;

        /// Getter function which gets the actual value from an item
        funcType m_func;
    };

    /**
     * Helper class which wraps searchPropertyEqual and findItem to make it
     * easier to search through an IOD.
     * @tparam IOD the type of the IOD that should be checked
     * @tparam Item this should always be IOD::Item. Sadly, MSC6 can't figure
     * that out by itself and so we must explicitly tell it about this type.
     * @tparam type the type that is being searched for
     * @tparam arg2 the type of the second argument to the getter function
     * @tparam argument2 the value of the second argument to the getter function
     */
    template<typename IOD, typename Item, typename type, typename arg2, arg2 argument2>
    class searchPropertyEqualHelper {
    public:
        /// Typedef for simplifying usage of searchPropertyEqual
        typedef searchPropertyEqual<IOD, Item, type, arg2, argument2> searchType;

        /**
         * Find an item in a sequence
         * @param seq the sequence to search through
         * @param wanted the value that should be found
         * @param func function that gets a value from a sequence item
         * @return Reference to the first item that matches or to the empty
         *         default item.
         */
        static typename IOD::Item& search(IOD& seq, const type& wanted, typename searchType::funcType func)
        {
            searchType s(wanted, func);
            findItem<IOD, searchType>(seq, s);
            // If the search failed, this will be the empty default item
            return seq.getCurrentItem();
        }
    };
}

#endif
