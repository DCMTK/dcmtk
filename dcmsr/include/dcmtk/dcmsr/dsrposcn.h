/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRPositionCounter
 *
 */


#ifndef DSRPOSCN_H
#define DSRPOSCN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oflist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a position counter to be used for iterating a tree of nodes
 */
class DCMTK_DCMSR_EXPORT DSRPositionCounter
{
  public:

    /** (default) constructor
     ** @param  flags  optional flags used to customize the processing of the counter.
     *                 These flags are only stored but not evaluated by this class.
     */
    DSRPositionCounter(const size_t flags = 0);

    /** copy constructor
     ** @param  counter  object to be copied
     */
    DSRPositionCounter(const DSRPositionCounter &counter);

    /** destructor
     */
    virtual ~DSRPositionCounter();

    /** assignment operator
     ** @param  counter  object to be copied
     ** @return reference to modified cursor (this object)
     */
    DSRPositionCounter &operator=(const DSRPositionCounter &counter);

    /** conversion operator that returns the position on the current level
     *  (0 if the position counter is invalid)
     */
    inline operator size_t () const
    {
        return Position;
    }

    /** pre-increment operator.  Increases the position on the current level by 1.
     *  Also makes the position counter valid if it was invalid before.
     */
    inline DSRPositionCounter &operator++()
    {
        ++Position;
        return *this;
    }

    /** pre-decrement operator.  Decreases the position on the current level by 1.
     *  This makes the position counter invalid if the position was 1 before.
     */
    inline DSRPositionCounter &operator--()
    {
        /* avoid underflow */
        if (Position > 0) --Position;
        return *this;
    }

    /** clear all member variables.
     *  The position counter becomes invalid afterwards (same state as after default
     *  construction).  Also see initialize().
     */
    void clear();

    /** initialize the position counter
     ** @param  valid  flag specifying whether the counter should be initialized as
     *                 valid (default) or invalid
     *  @param  flags  optional flags used to customize the processing of the counter.
     *                 These flags are only stored but not evaluated by this class.
     */
    void initialize(const OFBool valid = OFTrue,
                    const size_t flags = 0);

    /** check whether position counter is valid (i.e.\ has been initialized)
     ** @return OFTrue if valid, OFFalse otherwise
     */
    inline OFBool isValid() const
    {
        return (Position > 0);
    }

    /** go one level up, i.e.\ restore the position on the next upper level and forget
     *  about the current level
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool goUp();

    /** go one level down, i.e.\ store the position on the current level and start with
     *  1 on the new level
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool goDown();

    /** get specified flags that can be used to customize the counter
     ** @return flags used to customize the counter
     */
    inline size_t getFlags() const
    {
        return Flags;
    }

    /** get current level of the position counter.
     *  The top most (root) level is 1, the next lower level is 2, etc.
     ** @return number of the current level if valid, 0 otherwise
     */
    inline size_t getLevel() const
    {
        /* check for valid counter */
        return isValid() ? PositionList.size() + 1 : 0;
    }

    /** get string representing the current state of the position counter.
     *  See DSRTreeNodeCursor::getPosition() for details.
     ** @param  position   variable where the position string should be stored
     *  @param  separator  character used to separate the figures (default: '.')
     ** @return reference to the resulting position string (empty if invalid)
     */
    const OFString &getString(OFString &position,
                              const char separator = '.') const;


  private:

    /// current position within the current level
    size_t Position;
    /// list of position counters in upper levels
    OFList<size_t> PositionList;
    /// flags used to customize the counter
    size_t Flags;
};


#endif
