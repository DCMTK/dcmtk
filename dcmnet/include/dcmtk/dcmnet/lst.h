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
 *  Purpose: List class with procedural API compatible to MIR CTN
 *
 */

#ifndef LST_H
#define LST_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/dcmnet/dndefine.h"


/** general purpose list class for use with dcmnet module.
 */
class DCMTK_DCMNET_EXPORT LST_HEAD
{
public:
  /// default constructor
  LST_HEAD();

  /// destructor, deletes list but not elements pointed to by list entries.
  ~LST_HEAD();
  
  /** inserts after the last element of the list.
   *  @param node value inserted into the list
   */
  void push_back(void *node);

  /** removes first element from list and returns it.
   *  Returns NULL if list is empty.
   */
  void *dequeue();

  /** returns number of elements in the list.
   *  @return number of elements
   */
  size_t size() const;

 /** returns the first element in the list.
  *  @return first element in list, NULL if list empty
  */
  void *front();

  /** Make a node current and return the argument.
   *  @param node pointer to element which must be contained in the list
   *  @return pointer to node if successful, NULL otherwise
   */
  void *position(void *node);

  /** Advances the current element to the next element in the list
   *  and returns a pointer to the next element (if any), NULL otherwise.
   *  A valid current element must exist (e.g. position() called with an
   *  existing element), otherwise the behaviour is undefined.
   */
  void *next();

  /** Returns pointer to current element in list.
   *  A valid current element must exist (e.g. position() called with an
   *  existing element), otherwise the behaviour is undefined.
   */
  void *current() const;


private:
  /// the list
  OFList<void *> theList;
  
  /// list iterator, points to current element
  OFListIterator(void *) theIterator;
};


// --------------------------------------------------------------------
// THE FOLLOWING PROCEDURAL API IS COMPATIBLE TO THE MIR CTN LST MODULE
// --------------------------------------------------------------------

/// LST_NODE pseudo-type for MIR List API
typedef void LST_NODE;

/** creates a new list head and returns your handle to it.
 */
DCMTK_DCMNET_EXPORT LST_HEAD *LST_Create();

/** destroys list. The list must be empty.
 *  The list handle is set to NULL as a side-effect.
 */
DCMTK_DCMNET_EXPORT void LST_Destroy(LST_HEAD **lst);

/** Adds a new node to the tail of the list and returns status.
 */
DCMTK_DCMNET_EXPORT void LST_Enqueue(LST_HEAD **lst, void *node);

/** Removes a node from the head of the list and returns
 *  a pointer to it.
 */
DCMTK_DCMNET_EXPORT void *LST_Dequeue(LST_HEAD **lst);

/** alias for LST_Dequeue()
 */
DCMTK_DCMNET_EXPORT void *LST_Pop(LST_HEAD **lst);

/** Returns the number of nodes in the list.
 */
DCMTK_DCMNET_EXPORT unsigned long LST_Count(LST_HEAD **lst);

/** Returns a pointer to the node at the head of the list.
 *  It does NOT remove the node from the list.
 */
DCMTK_DCMNET_EXPORT void *LST_Head(LST_HEAD **lst);

/** Returns a pointer to the current node.
 */
DCMTK_DCMNET_EXPORT void *LST_Current(LST_HEAD **lst);

/** Returns a pointer to the next node in the list and
 *  makes it current.
 */
DCMTK_DCMNET_EXPORT void *LST_Next(LST_HEAD **lst);

/** Make a node current and return the argument.
 *  Note:  node = lst_position(list, lst_head(list));
 *         makes the node at the head of the list current
 *         and returns a pointer to it.
 */
DCMTK_DCMNET_EXPORT void *LST_Position(LST_HEAD **lst, void *node);

#endif
