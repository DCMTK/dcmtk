/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for class DSRNumericMeasurementValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/codes/dcm.h"


OFTEST(dcmsr_setNumericMeasurementValue)
{
    DSRNumericMeasurementValue numValue;
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(!numValue.isComplete());
    /* set valid coded entry */
    OFCHECK(numValue.setValue("", DSRCodedEntryValue(), OFTrue /*check*/).good());
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(!numValue.isComplete());
    OFCHECK(numValue.setValue(CODE_DCM_NotANumber, OFTrue /*check*/).good());
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK(numValue.setValue("1.5", DSRBasicCodedEntry("cm", "UCUM", "1.4", "centimeter"), OFTrue /*check*/).good());
    OFCHECK(!numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK(numValue.setValue("99999", DSRBasicCodedEntry("cm", "UCUM", "1.4", "centimeter"), CODE_DCM_ValueOutOfRange, OFTrue /*check*/).good());
    OFCHECK(!numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    /* set invalid coded entry (should fail) */
    OFCHECK(numValue.setValue("1.5", DSRCodedEntryValue(), CODE_DCM_NotANumber, OFFalse /*check*/).bad());
    OFCHECK(numValue.setValue("", DSRBasicCodedEntry("0815", "99TEST", "Some test code"), CODE_DCM_NotANumber, OFFalse /*check*/).bad());
    /* clear coded entry */
    numValue.clear();
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(!numValue.isComplete());
}
