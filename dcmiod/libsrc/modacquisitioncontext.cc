/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modacquisitioncontext.h"
#include "dcmtk/dcmdata/dcdeftag.h"

const OFString IODAcquisitionContextModule::m_ModuleName = "AcquisitionContextModule";

IODAcquisitionContextModule::IODAcquisitionContextModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

IODAcquisitionContextModule::IODAcquisitionContextModule()
    : IODModule()
{
    // reset element rules
    resetRules();
}

OFString IODAcquisitionContextModule::getName() const
{
    return m_ModuleName;
}

void IODAcquisitionContextModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_AcquisitionContextSequence, "1-n", "2", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
}

IODAcquisitionContextModule::~IODAcquisitionContextModule()
{
}

// --- get attributes (C++ string) ---

// --- set attributes ---
