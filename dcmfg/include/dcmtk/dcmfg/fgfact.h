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
 *  Purpose: Factory class for creating functional groups
 *
 */

#ifndef FGFACT_H
#define FGFACT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmfg/fgtypes.h"

class FGBase;


/** Singleton class that is used to create functional groups by knowing
 *  their type or other unique features, i.e.\ offering factory functionality.
 */
class DCMTK_DCMFG_EXPORT FGFactory
{
public:

  /** Return the single instance of the factory
   *  @return The instance of FGFactory
   */
  static FGFactory& instance();

  /** Create new functional group based on given type
   *  @param fgtype The type of functional group to create
   *  @return The functional group or NULL if error occurs
   */
  FGBase *create(const DcmFGTypes::E_FGType fgtype);

  /** Create new functional group based its unique sequence tag key
   *  @param fgSequenceKey Tag key of the functional groups sequence
   *  @return The functional group or NULL if error occurs
   */
  FGBase *create(const DcmTagKey& fgSequenceKey);

private:

  /** Private undefined constructor (singleton implementation)
   */
  FGFactory();

  /** Private undefined copy constructor (singleton implementation)
   */
  FGFactory(const FGFactory&);

  /** Private undefined assignment operator (singleton implementation)
   */
  FGFactory& operator=(const FGFactory&);

  /** Private undefined destructor (singleton implementation)
   */
  ~FGFactory() {}

  /// The instance of FGFactory handled by this singleton class
  static FGFactory* m_Instance;

};

#endif // FGFACT_H
