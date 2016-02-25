/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 *  Purpose: Class representing a stack in the context of Enhanced DICOM objects
 *
 */

#ifndef FGSTACK_H
#define FGSTACK_H

#include "dcmtk/config/osconfig.h"
#include <dcmtk/ofstd/ofmap.h>
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmfg/fgdefine.h"

/** Class representing a stack in the context of an Enhanced DICOM object.
 *  Therefore the class stores the ID of the stack (Stack ID) and all the frame
 *  numbers being part of that stack, as well as for each frame its position
 *  within the stack.
 */
class DCMTK_DCMFG_EXPORT FGStack
{
public:

  // Make sure the main interface class has easy access to internal members
  friend class FGStackInterface;

  /// Iterator type for iterating over the frames of a stack
  typedef OFMap<Uint32,Uint32>::iterator iterator;

  /// Const iterator for iterating over the frames of a stack
  typedef OFMap<Uint32,Uint32>::const_iterator const_iterator;

  /** Constructor, creates stack from Stack ID and assigned frame numbers
   *  @param  stackID The Stack ID of the stack
   *  @param  frameNumbers A map with frame numbers as the keys, and the
   *          position of the frame within the stack as the value for each
   *          frame. First position is 1. There may be frames having the same
   *          positions in the same stack. However, in that case the standard
   *          lists some elements which at least must have the same values, then:
   *          - Dimension Organization UID (0020,9164) to qualify the Stack ID
   *          - Image Position (Patient) (0020,0032)
   *          - Image Orientation (Patient) (0020,0037)
   *          - Rows (0028,0010) * first value of Pixel Spacing (0028,0030) (= field of view in the row direction)
   *          - Columns (0028,0011) * second value of Pixel Spacing (0028,0030) (= field of view in the column direction)
   *          - Slice Thickness (0018,0050)
   *         The values may change over time (last check was DICOM 2014a), so
   *         the latest edition of the standard should be consulted if it is
   *         planned to apply the same in-stack position to different frames.
   */
  FGStack(const OFString& stackID,
          const OFMap<Uint32, Uint32> frameNumbers);

  /** Constructor, create empty stack with given Stack ID (and fill in frames
   *  later)
   *  @param  stackID The Stack ID of the frame
   */
  FGStack(const OFString& stackID);

  /** Virtual destructor, frees memory
   */
  virtual ~FGStack();

  // TODO add sorted iterators using views, for example

  /** Return const iterator to first frame (not sorted after stack position)
   *  @return Const iterator to first frame
   */
  const_iterator begin() const;

  /** Return iterator to first frame (not sorted after stack position)
   *  @return Iterator to first frame
   */
  iterator begin();

  /** Return const iterator pointing behind last frame (not sorted after stack position)
   *  @return Const iterator pointing behind last frame
   */
  const_iterator end() const;

  /** Return iterator pointing behind last frame (not sorted after stack position)
   *  @return Iterator pointing behind last frame
   */
  iterator end();

  /** Add frame to stack and set its in-stack position. Any old position
   *  will be overwritten.
   *  @param  frameNumber The frame number that should be added to the stack
   *  @param  inStackPos  The position in the stack (starting from 1). More than
   *          one frame can have the same position in the stack, however there
   *          are rules (see documentation of constructor and the DICOM
   *          standard)
   */
  virtual OFBool addFrame(const Uint32 frameNumber,
                          const Uint32 inStackPos);

  /** Get Stack ID uniquely identifying this stack
   *  @return The Stack ID
   */
  virtual OFString getStackID() const;

  /** Get the stack position for a given frame
   *  @param  frameNumber The number of the frame
   *  @return The stack position for that frame
   */
  Uint32 getInStackPos(const Uint32 frameNumber) const;

  /** Get list of frames that are set to a specific In-Stack Position
   *  @param  inStackPos In-Stack Position Number to find frames for
   *  @param  resultFrameNumbers The frame numbers assigned to that stack
   *          position
   */
  void getFramesAtStackPos(const Uint32 inStackPos,
                           OFVector<Uint32>& resultFrameNumbers);

private:

  /// The Stack ID of this frame
  OFString m_StackID;

  /// Map with frames, key is the frame number, value is the in-stack position.
  /// More than one frame can have the same in-stack position
  OFMap<Uint32, Uint32> m_FrameNumbers;

  // TODO Maybe add a redundant data structures mapping in-stack positions
  // to frame numbers in order to enable fast access?
};

#endif // STACKREADER_H
