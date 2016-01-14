/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CMR_ContentMappingResource
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/init.h"

#include "dcmtk/dcmsr/cmr/cid11.h"
#include "dcmtk/dcmsr/cmr/cid29.h"
#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid100.h"
#include "dcmtk/dcmsr/cmr/cid244.h"
#include "dcmtk/dcmsr/cmr/cid4020.h"
#include "dcmtk/dcmsr/cmr/cid4021.h"
#include "dcmtk/dcmsr/cmr/cid4031.h"
#include "dcmtk/dcmsr/cmr/cid5000.h"
#include "dcmtk/dcmsr/cmr/cid5001.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7021.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/cid7452.h"
#include "dcmtk/dcmsr/cmr/cid7453.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"
#include "dcmtk/dcmsr/cmr/cid10013.h"
#include "dcmtk/dcmsr/cmr/cid10033.h"


/*------------------*
 *  implementation  *
 *------------------*/

void CMR_ContentMappingResource::initializeAllContextGroups()
{
    /* initialize all context groups, one after the other */
    CID11_RouteOfAdministration::initialize();
    CID29_AcquisitionModality::initialize();
    CID42_NumericValueQualifier::initialize();
    CID100_QuantitativeDiagnosticImagingProcedures::initialize();
    CID244_Laterality::initialize();
    CID4020_PETRadionuclide::initialize();
    CID4021_PETRadiopharmaceutical::initialize();
    CID4031_CommonAnatomicRegions::initialize();
    CID5000_Languages::initialize();
    CID5001_Countries::initialize();
    CID6147_ResponseCriteria::initialize();
    CID7021_MeasurementReportDocumentTitles::initialize();
    CID7181_AbstractMultiDimensionalImageModelComponentUnits::initialize();
    CID7445_DeviceParticipatingRoles::initialize();
    CID7452_OrganizationalRoles::initialize();
    CID7453_PerformingRoles::initialize();
    CID7464_GeneralRegionOfInterestMeasurementModifiers::initialize();
    CID7469_GenericIntensityAndSizeMeasurements::initialize();
    CID10013_CTAcquisitionType::initialize();
    CID10033_CTReconstructionAlgorithm::initialize();
}


void CMR_ContentMappingResource::cleanupAllContextGroups()
{
    /* cleanup all context groups, one after the other */
    CID11_RouteOfAdministration::cleanup();
    CID29_AcquisitionModality::cleanup();
    CID42_NumericValueQualifier::cleanup();
    CID100_QuantitativeDiagnosticImagingProcedures::cleanup();
    CID244_Laterality::cleanup();
    CID4020_PETRadionuclide::cleanup();
    CID4021_PETRadiopharmaceutical::cleanup();
    CID4031_CommonAnatomicRegions::cleanup();
    CID5000_Languages::cleanup();
    CID5001_Countries::cleanup();
    CID6147_ResponseCriteria::cleanup();
    CID7021_MeasurementReportDocumentTitles::cleanup();
    CID7181_AbstractMultiDimensionalImageModelComponentUnits::cleanup();
    CID7445_DeviceParticipatingRoles::cleanup();
    CID7452_OrganizationalRoles::cleanup();
    CID7453_PerformingRoles::cleanup();
    CID7464_GeneralRegionOfInterestMeasurementModifiers::cleanup();
    CID7469_GenericIntensityAndSizeMeasurements::cleanup();
    CID10013_CTAcquisitionType::cleanup();
    CID10033_CTReconstructionAlgorithm::cleanup();
}
