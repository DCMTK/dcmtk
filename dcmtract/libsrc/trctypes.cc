/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class and definitions managing various tractography-typical types
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmtract/trctypes.h"

OFLogger DCM_dcmtractLogger = OFLog::getLogger("dcmtk.dcmtract");

makeOFConditionConst(TRC_EC_InvalidPointCoordinatesData, OFM_dcmtract, 1, OF_error, "Invalid data in Track's Point Coordinates Data element");
makeOFConditionConst(TRC_EC_InvalidColorInformation,     OFM_dcmtract, 2, OF_error, "Invalid color information for Track or Track Set");
makeOFConditionConst(TRC_EC_NoSuchTrack,                 OFM_dcmtract, 3, OF_error, "No such Track in Track Set");
makeOFConditionConst(TRC_EC_NoSuchMeasurement,           OFM_dcmtract, 4, OF_error, "No such Measurement in Track Set");
makeOFConditionConst(TRC_EC_MeasurementDataMissing,      OFM_dcmtract, 5, OF_error, "Measurement misses data for one or more tracks");
makeOFConditionConst(TRC_EC_InvalidStatisticData,        OFM_dcmtract, 6, OF_error, "Statistic data is invalid");
makeOFConditionConst(TRC_EC_InvalidTrackData,            OFM_dcmtract, 7, OF_error, "Track data is invalid");
