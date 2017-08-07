/*
 *
 *  Copyright (C) 2016-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for common error constants used in TID 14xx/15xx
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID15DEF_H
#define CMR_TID15DEF_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmsr/cmr/define.h"


// include this file in doxygen documentation

/** @file tid15def.h
 *  @brief Error constants for TID 14xx/15xx in module dcmsr/cmr
 */


/*------------------------*
 *  constant definitions  *
 *------------------------*/

/** @name specific error conditions for TID 1500 (and included templates) in module dcmsr/cmr
 */
//@{

/// error: there is no measurement report to add content items to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementReport;
/// error: there is no measurement group to add entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementGroup;
/// error: there is no measurement to add entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurement;
/// error: the given segmentation object does not conform to the template constraints
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidSegmentationObject;
/// error: the given DICOM object is not a real world value mapping object
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidRealWorldValueMappingObject;

//@}

#endif
