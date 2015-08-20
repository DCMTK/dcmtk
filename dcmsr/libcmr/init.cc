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

#include "dcmtk/dcmsr/cmr/cid11.h"
#include "dcmtk/dcmsr/cmr/cid29.h"
#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid244.h"
#include "dcmtk/dcmsr/cmr/cid4020.h"
#include "dcmtk/dcmsr/cmr/cid4021.h"
#include "dcmtk/dcmsr/cmr/cid4031.h"
#include "dcmtk/dcmsr/cmr/cid5000.h"
#include "dcmtk/dcmsr/cmr/cid5001.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/cid7452.h"
#include "dcmtk/dcmsr/cmr/cid7453.h"
#include "dcmtk/dcmsr/cmr/cid10013.h"
#include "dcmtk/dcmsr/cmr/cid10033.h"


/*------------------*
 *  implementation  *
 *------------------*/

void ContentMappingResource::initializeAllContextGroups()
{
    /* initialize all context groups, one after the other */
    CID11_RouteOfAdministration::initialize();
    CID29_AcquisitionModality::initialize();
    CID42_NumericValueQualifier::initialize();
    CID244_Laterality::initialize();
    CID4020_PETRadionuclide::initialize();
    CID4021_PETRadiopharmaceutical::initialize();
    CID4031_CommonAnatomicRegions::initialize();
    CID5000_Languages::initialize();
    CID5001_Countries::initialize();
    CID7445_DeviceParticipatingRoles::initialize();
    CID7452_OrganizationalRoles::initialize();
    CID7453_PerformingRoles::initialize();
    CID10013_CTAcquisitionType::initialize();
    CID10033_CTReconstructionAlgorithm::initialize();
}


void ContentMappingResource::cleanupAllContextGroups()
{
    /* cleanup all context groups, one after the other */
    CID11_RouteOfAdministration::cleanup();
    CID29_AcquisitionModality::cleanup();
    CID42_NumericValueQualifier::cleanup();
    CID244_Laterality::cleanup();
    CID4020_PETRadionuclide::cleanup();
    CID4021_PETRadiopharmaceutical::cleanup();
    CID4031_CommonAnatomicRegions::cleanup();
    CID5000_Languages::cleanup();
    CID5001_Countries::cleanup();
    CID7445_DeviceParticipatingRoles::cleanup();
    CID7452_OrganizationalRoles::cleanup();
    CID7453_PerformingRoles::cleanup();
    CID10013_CTAcquisitionType::cleanup();
    CID10033_CTReconstructionAlgorithm::cleanup();
}
