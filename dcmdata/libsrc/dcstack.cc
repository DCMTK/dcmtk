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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    while ( num-- > 0 && node != NULL )
         node = node->link;

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


/*
 * CVS/RCS Log:
 * $Log: dcstack.cc,v $
 * Revision 1.20  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.18  2005-12-08 15:41:37  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2004/02/04 16:44:23  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.16  2003/06/02 16:52:26  meichel
 * Cleaned up implementation of DcmStack, added doc++ comments
 *
 * Revision 1.15  2002/11/27 12:06:52  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.14  2002/04/16 13:43:21  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.13  2001/06/01 15:49:09  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/03/08 16:26:41  meichel
 * Updated copyright header.
 *
 * Revision 1.11  2000/02/23 15:12:02  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.10  2000/02/01 10:12:10  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.9  1999/03/31 09:25:39  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
