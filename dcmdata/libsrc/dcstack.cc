/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: stack class
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcstack.h"


// ************************************************
// *** DcmStackNode() *****************************
// ************************************************


DcmStackNode::DcmStackNode( DcmObject *obj )
  : link(NULL),
    objNodeValue(obj)
{
}


// ********************************


DcmStackNode::~DcmStackNode()
{
}


// ********************************


DcmObject* DcmStackNode::value() const
{
    return objNodeValue;
}



// ************************************************
// *** DcmStack() *********************************
// ************************************************


DcmStack::DcmStack()
  : topNode_(NULL),
    cardinality_(0)
{
}


// ************************************************


DcmStack::DcmStack( const DcmStack & oldStack)
  : topNode_(NULL),
    cardinality_(oldStack.cardinality_)
{
    if (cardinality_)
    {
        topNode_ = new DcmStackNode(oldStack.topNode_->objNodeValue);
        DcmStackNode * oldPtr = oldStack.topNode_->link;
        DcmStackNode * newPtr = topNode_;
        while (oldPtr)
        {
            newPtr->link = new DcmStackNode(oldPtr->objNodeValue);
            oldPtr = oldPtr->link;
            newPtr = newPtr->link;
        }
    }
}


// ********************************


DcmStack::~DcmStack()
{
    clear();
}


// ********************************


void DcmStack::clear()
{
    DcmStackNode *node;
    while (topNode_ != NULL)
    {
        node = topNode_;
        topNode_ = topNode_->link;
        delete node;
    }
    cardinality_ = 0;
}


// ********************************


DcmObject* DcmStack::push( DcmObject *obj )
{
    if ( obj != NULL )
    {
        DcmStackNode *node = new DcmStackNode( obj );
        node->link = topNode_;
        topNode_ = node;
        cardinality_++;
    }
    return obj;
}


// ********************************


DcmObject* DcmStack::pop()
{
    DcmObject *obj;
    DcmStackNode *node;
    if ( topNode_ != NULL )
    {
        obj = topNode_->value();
        node = topNode_;
        topNode_ = topNode_->link;
        delete node;
        cardinality_--;
    }
    else
        obj = NULL;
    return obj;
}


// ********************************


DcmObject* DcmStack::elem(const unsigned long number) const
{
    unsigned long num = number;
    DcmObject *obj;
    DcmStackNode *node = topNode_;
    while ( num > 0 && node != NULL )
    {
         node = node->link;
         --num;
    }
    if ( node != NULL )
        obj = node->value();
    else
        obj = NULL;
    return obj;
}


// ********************************


DcmObject* DcmStack::top() const
{
    DcmObject *obj;
    if ( topNode_ != NULL )
        obj = topNode_->value();
    else
        obj = NULL;
    return obj;
}


// ********************************


unsigned long DcmStack::card() const
{
    return cardinality_;
}


// ********************************


OFBool DcmStack::empty() const
{
    return OFstatic_cast(OFBool, topNode_ == NULL );
}


DcmStack& DcmStack::operator=(const DcmStack& arg)
{
  if (this != &arg)
  {
    clear();
    cardinality_ = arg.cardinality_;
    if (cardinality_)
    {
        topNode_ = new DcmStackNode(arg.topNode_->objNodeValue);
        DcmStackNode * oldPtr = arg.topNode_->link;
        DcmStackNode * newPtr = topNode_;
        while (oldPtr)
        {
            newPtr->link = new DcmStackNode(oldPtr->objNodeValue);
            oldPtr = oldPtr->link;
            newPtr = newPtr->link;
        }
    }
  }
  return *this;
}


OFBool DcmStack::operator<(const DcmStack& arg) const
{
  if (cardinality_ < arg.cardinality_) return OFTrue;
  if (cardinality_ > arg.cardinality_) return OFFalse;

  // cardinality_ is equal. Now walk through stack and do pointer arithmetics
  DcmStackNode * thisPtr = topNode_;
  DcmStackNode * argPtr = arg.topNode_;
  while (thisPtr)
  {
      if (thisPtr->objNodeValue < argPtr->objNodeValue) return OFTrue;
      if (thisPtr->objNodeValue > argPtr->objNodeValue) return OFFalse;
      thisPtr = thisPtr->link;
      argPtr = argPtr->link;
  }
  return OFFalse; // stacks are equal
}


OFBool DcmStack::operator==(const DcmStack& arg) const
{
  if (cardinality_ != arg.cardinality_) return OFFalse;

  DcmStackNode * thisPtr = topNode_;
  DcmStackNode * argPtr = arg.topNode_;
  while (thisPtr)
  {
      if (thisPtr->objNodeValue != argPtr->objNodeValue) return OFFalse;
      thisPtr = thisPtr->link;
      argPtr = argPtr->link;
  }
  return OFTrue;
}
