/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
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
#include "dcmtk/dcmsr/cmr/cid218.h"
#include "dcmtk/dcmsr/cmr/cid244.h"
#include "dcmtk/dcmsr/cmr/cid247.h"
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
#include "dcmtk/dcmsr/cmr/cid7551.h"
#include "dcmtk/dcmsr/cmr/cid10013.h"
#include "dcmtk/dcmsr/cmr/cid10033.h"


/*------------------*
 *  implementation  *
 *------------------*/

void CMR_ContentMappingResource::initializeAllContextGroups()
{
    /* initialize all context groups, one after the other */
    CID11_AdministrationRoute::initialize();
    CID29_AcquisitionModality::initialize();
    CID42_NumericValueQualifier::initialize();
    CID100_QuantitativeDiagnosticImagingProcedure::initialize();
    CID218_QuantitativeImageFeature::initialize();
    CID244_Laterality::initialize();
    CID247_LateralityLeftRightOnly::initialize();
    CID4020_PETRadionuclide::initialize();
    CID4021_PETRadiopharmaceutical::initialize();
    CID4031_CommonAnatomicRegion::initialize();
    CID5000_Language::initialize();
    CID5001_Country::initialize();
    CID6147_ResponseCriteria::initialize();
    CID7021_MeasurementReportDocumentTitle::initialize();
    CID7181_AbstractMultiDimensionalImageModelComponentUnit::initialize();
    CID7445_DeviceParticipatingRole::initialize();
    CID7452_OrganizationalRole::initialize();
    CID7453_PerformingRole::initialize();
    CID7464_GeneralRegionOfInterestMeasurementModifier::initialize();
    CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurement::initialize();
    CID7469_GenericIntensityAndSizeMeasurement::initialize();
    CID10013_CTAcquisitionType::initialize();
    CID10033_CTReconstructionAlgorithm::initialize();
}


void CMR_ContentMappingResource::cleanupAllContextGroups()
{
    /* cleanup all context groups, one after the other */
    CID11_AdministrationRoute::cleanup();
    CID29_AcquisitionModality::cleanup();
    CID42_NumericValueQualifier::cleanup();
    CID100_QuantitativeDiagnosticImagingProcedure::cleanup();
    CID218_QuantitativeImageFeature::cleanup();
    CID244_Laterality::cleanup();
    CID247_LateralityLeftRightOnly::cleanup();
    CID4020_PETRadionuclide::cleanup();
    CID4021_PETRadiopharmaceutical::cleanup();
    CID4031_CommonAnatomicRegion::cleanup();
    CID5000_Language::cleanup();
    CID5001_Country::cleanup();
    CID6147_ResponseCriteria::cleanup();
    CID7021_MeasurementReportDocumentTitle::cleanup();
    CID7181_AbstractMultiDimensionalImageModelComponentUnit::cleanup();
    CID7445_DeviceParticipatingRole::cleanup();
    CID7452_OrganizationalRole::cleanup();
    CID7453_PerformingRole::cleanup();
    CID7464_GeneralRegionOfInterestMeasurementModifier::cleanup();
    CID7469_GenericIntensityAndSizeMeasurement::cleanup();
    CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurement::cleanup();
    CID10013_CTAcquisitionType::cleanup();
    CID10033_CTReconstructionAlgorithm::cleanup();
}
