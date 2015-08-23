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
 *  Purpose: Class for reading, accessing and writing stacks
 *
 */

#ifndef STACKINTERFACE_H
#define STACKINTERFACE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/dcmfg/stack.h"

// Forward declaration
class FGFrameContent;


/** Class for convenient access of stacks within an Enhanced DICOM object
 */
class DCMTK_DCMFG_EXPORT FGStackInterface
{
public:

  FGStackInterface();

  OFBool checkConsistency(FGInterface* fgContext = NULL);

  virtual void clear();

  virtual ~FGStackInterface();

  virtual OFBool addStack(FGStack* stack);

  virtual OFCondition read(FGInterface& fgSource);

  virtual OFCondition write(FGInterface& fgDestination);

  size_t numStacks() const;

protected:

  FGFrameContent* ensureFrameContentFG(const Uint32 frameNo,
                                       FGInterface& fg);

  // Returns number of errors
  virtual size_t checkContext(FGStack* stack,
                              FGInterface* context);

private:

  /// The stacks found
  OFMap<OFString, FGStack*> m_Stacks;
};

#endif // STACKINTERFACE_H