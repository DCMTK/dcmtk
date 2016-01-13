/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CMR_ContentMappingResource
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_INIT_H
#define CMR_INIT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions used for the "Content Mapping Resource"
 *  (CMR) from the global namespace.
 *  All functions are static and can, therefore, be accessed without creating an instance
 *  of this class.
 */
class DCMTK_CMR_EXPORT CMR_ContentMappingResource
{

  public:

    /** initialize \e all supported context groups of the "Content Mapping Resource"
     *  explicitly.  Currently, the following context group classes are supported:
     *  - CID11_RouteOfAdministration
     *  - CID29_AcquisitionModality
     *  - CID42_NumericValueQualifier
     *  - CID100_QuantitativeDiagnosticImagingProcedures
     *  - CID244_Laterality
     *  - CID4020_PETRadionuclide
     *  - CID4021_PETRadiopharmaceutical
     *  - CID4031_CommonAnatomicRegions
     *  - CID5000_Languages
     *  - CID5001_Countries
     *  - CID6147_ResponseCriteria
     *  - CID7021_MeasurementReportDocumentTitles
     *  - CID7181_AbstractMultiDimensionalImageModelComponentUnits
     *  - CID7445_DeviceParticipatingRoles
     *  - CID7452_OrganizationalRoles
     *  - CID7453_PerformingRoles
     *  - CID7464_GeneralRegionOfInterestMeasurementModifiers
     *  - CID7469_GenericIntensityAndSizeMeasurements
     *  - CID10013_CTAcquisitionType
     *  - CID10033_CTReconstructionAlgorithm
     *
     ** @note In case of multi-threaded applications, this function could be called
     *        before any context group is actually used.  This might be easier than
     *        calling the initialize() function of the respective context group classes
     *        separately.  However, since the list of supported context groups is
     *        increasing over time, the additional resource requirements should be
     *        considered.  That means, if only a few context groups are used, it is
     *        recommended to call the individual initialize() functions instead.
     */
    static void initializeAllContextGroups();

    /** cleanup \e all supported context groups of the "Content Mapping Resource".
     *  Usually, there is no need to call this method, but it might be useful in order
     *  to explicitly free the associated memory, e.g. when checking for memory leaks.
     *  See initializeAllContextGroups() for a list of supported context groups.
     */
    static void cleanupAllContextGroups();
};

#endif
