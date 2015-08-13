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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Acquisition Context Module
 *
 */

#ifndef MODACQUISITIONCONTEXT_H
#define MODACQUISITIONCONTEXT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Acquisition Context Module. At the moment
  * only an empty Acquisition Context Sequence is supported, so this
  * class servers more as a placeholder at the moment.
  *
  * Acquisition Context Sequence: (SQ, 1-n, 2)
  *
  */
class DCMTK_DCMIOD_EXPORT IODAcquisitionContextModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set where this classes rules are added to. If NULL, the
   *          class creates an empty rule set.
   */
  IODAcquisitionContextModule(OFshared_ptr<DcmItem> item,
                              OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODAcquisitionContextModule();

  /** Destructor
   */
  virtual ~IODAcquisitionContextModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("AcquisitionContextModule")
   *  @return Name of the module ("AcquisitionContextModule")
   */
  virtual OFString getName() const;

private:

  /// The name of this module ("AcquisitionContextModule")
  static const OFString m_ModuleName;
};

#endif // MODACQUISITIONCONTEXT_H
