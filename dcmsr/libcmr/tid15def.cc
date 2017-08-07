/*
 *
 *  Copyright (C) 2016-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for common error constants used in TID 14xx/15xx
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid15def.h"
#include "dcmtk/dcmdata/dcerror.h"


// conditions constants
makeOFConditionConst(CMR_EC_NoMeasurementReport,                OFM_dcmsr, 1500, OF_error, "No Measurement Report");
makeOFConditionConst(CMR_EC_NoMeasurementGroup,                 OFM_dcmsr, 1501, OF_error, "No Measurement Group");
makeOFConditionConst(CMR_EC_NoMeasurement,                      OFM_dcmsr, 1502, OF_error, "No Measurement");
makeOFConditionConst(CMR_EC_InvalidSegmentationObject,          OFM_dcmsr, 1503, OF_error, "Invalid Segmentation Object");
makeOFConditionConst(CMR_EC_InvalidRealWorldValueMappingObject, OFM_dcmsr, 1504, OF_error, "Invalid Real World Value Mapping Object");
