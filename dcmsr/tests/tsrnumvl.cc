/*
 *
 *  Copyright (C) 2015-2021, J. Riesmeier, Oldenburg, Germany
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
    OFCHECK(numValue.setValue("", DSRCodedEntryValue(), CODE_DCM_ValueIndeterminate, OFTrue /*check*/).good());
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK(numValue.setValue(CODE_DCM_NotANumber, OFTrue /*check*/).good());
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK(numValue.setValue("1.5", DSRBasicCodedEntry("cm", "UCUM", "centimeter"), OFTrue /*check*/).good());
    OFCHECK(!numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK(numValue.setValue("99999", DSRBasicCodedEntry("cm", "UCUM", "centimeter"), CODE_DCM_ValueOutOfRange, OFTrue /*check*/).good());
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


OFTEST(dcmsr_emptyMeasurementValueSequence)
{
    DSRNumericMeasurementValue numValue(CODE_DCM_ValueUnknown);
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.isComplete());
    /* clear the value qualifier */
    OFCHECK(numValue.setNumericValueQualifier(DSRCodedEntryValue()).good());
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(!numValue.isComplete());
}


OFTEST(dcmsr_setAndGetFloatingPointRepresentation)
{
    Float64 value;
    /* set numeric value with additional representation */
    DSRNumericMeasurementValue numValue("1.5", DSRBasicCodedEntry("cm", "UCUM", "centimeter"));
    OFCHECK(numValue.setFloatingPointRepresentation(1.5).good());
    /* get and check the value */
    OFCHECK(numValue.getFloatingPointRepresentation(value).good());
    OFCHECK_EQUAL(value, 1.5);
}


OFTEST(dcmsr_setAndGetRationalRepresentation)
{
    Sint32 value1;
    Uint32 value2;
    /* set numeric value with additional representation */
    DSRNumericMeasurementValue numValue("1.5", DSRBasicCodedEntry("cm", "UCUM", "centimeter"));
    OFCHECK(numValue.setRationalRepresentation(3, 2).good());
    /* get and check the value */
    OFCHECK(numValue.getRationalRepresentation(value1, value2).good());
    OFCHECK_EQUAL(value1, 3);
    OFCHECK_EQUAL(value2, 2);
}
