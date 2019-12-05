/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a stack in the context of Enhanced multiframe
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/stack.h"

FGStack::FGStack(const OFString& stackID, const OFMap<Uint32, Uint32> frameNumbers)
    : m_StackID(stackID)
    , m_FrameNumbers(frameNumbers)
{
}

FGStack::FGStack(const OFString& stackID)
    : m_StackID(stackID)
    , m_FrameNumbers()
{
}

OFBool FGStack::addFrame(const Uint32 frameNumber, const Uint32 inStackPos)
{
    m_FrameNumbers.insert(OFMake_pair(frameNumber, inStackPos));
    return OFTrue;
}

FGStack::~FGStack()
{
}

FGStack::const_iterator FGStack::begin() const
{
    return m_FrameNumbers.begin();
}

FGStack::iterator FGStack::begin()
{
    return m_FrameNumbers.begin();
}

FGStack::const_iterator FGStack::end() const
{
    return m_FrameNumbers.end();
}

FGStack::iterator FGStack::end()
{
    return m_FrameNumbers.end();
}

OFString FGStack::getStackID() const
{
    return m_StackID;
}

Uint32 FGStack::getInStackPos(const Uint32 frameNumber) const
{
    FGStack::const_iterator it = m_FrameNumbers.find(frameNumber);
    if (it == m_FrameNumbers.end())
        return 0;
    else
        return (*it).second;
}

void FGStack::getFramesAtStackPos(const Uint32 inStackPos, OFVector<Uint32>& resultFrameNumbers)
{
    FGStack::iterator it = m_FrameNumbers.begin();
    while (it != m_FrameNumbers.end())
    {
        if ((*it).second == inStackPos)
        {
            resultFrameNumbers.push_back((*it).second);
        }
        it++;
    }
}
