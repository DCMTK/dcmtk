/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: ContentMappingResource
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/init.h"

#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid5000.h"
#include "dcmtk/dcmsr/cmr/cid5001.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/cid7452.h"
#include "dcmtk/dcmsr/cmr/cid7453.h"


/*------------------*
 *  implementation  *
 *------------------*/

void ContentMappingResource::initializeAllContextGroups()
{
    /* initialize all context groups, one after the other */
    CID42_NumericValueQualifier::initialize();
    CID5000_Languages::initialize();
    CID5001_Countries::initialize();
    CID7445_DeviceParticipatingRoles::initialize();
    CID7452_OrganizationalRoles::initialize();
    CID7453_PerformingRoles::initialize();
}
