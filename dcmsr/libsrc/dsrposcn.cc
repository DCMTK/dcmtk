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


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrposcn.h"
#include "dcmtk/dcmsr/dsrtypes.h"


DSRPositionCounter::DSRPositionCounter()
  : Position(0),
    PositionList()
{
}


DSRPositionCounter::DSRPositionCounter(const DSRPositionCounter &counter)
  : Position(counter.Position),
    PositionList(counter.PositionList)
{
}


DSRPositionCounter::~DSRPositionCounter()
{
}


DSRPositionCounter &DSRPositionCounter::operator=(const DSRPositionCounter &counter)
{
    Position = counter.Position;
    PositionList = counter.PositionList;
    return *this;
}


void DSRPositionCounter::clear()
{
    /* clear and invalidate position counter */
    Position = 0;
    PositionList.clear();
}


void DSRPositionCounter::initialize(const OFBool valid)
{
    /* distinguish valid from non-valid position counter */
    Position = (valid ? 1 : 0);
    PositionList.clear();
}


OFBool DSRPositionCounter::goUp()
{
    const OFBool result = !PositionList.empty();
    if (result)
    {
        /* get back last position on next upper level (if any) */
        Position = PositionList.back();
        PositionList.pop_back();
    }
    return result;
}


OFBool DSRPositionCounter::goDown()
{
    const OFBool result = (Position > 0);
    if (result)
    {
        /* store current position and reset counter for next lower level */
        PositionList.push_back(Position);
        Position = 1;
    }
    return result;
}


const OFString &DSRPositionCounter::getString(OFString &position,
                                              const char separator) const
{
    position.clear();
    if (isValid())
    {
        char stringBuf[20];
        const OFListConstIterator(size_t) endPos = PositionList.end();
        OFListConstIterator(size_t) iterator = PositionList.begin();
        /* iterate over all levels and append current position in each case */
        while (iterator != endPos)
        {
            if (!position.empty())
                position += separator;
            position += DSRTypes::numberToString(*iterator, stringBuf);
            iterator++;
        }
        /* also append the current position on the current level */
        if (!position.empty())
            position += separator;
        position += DSRTypes::numberToString(Position, stringBuf);
    }
    return position;
}
