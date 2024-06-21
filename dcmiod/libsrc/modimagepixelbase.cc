/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Base class for Image Pixel Module and related (e.g. Floating Point)
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modimagepixelbase.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODImagePixelBase::m_ModuleName = "ImagePixelBase";

IODImagePixelBase::IODImagePixelBase(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

OFString IODImagePixelBase::getName() const
{
    return m_ModuleName;
}

IODImagePixelBase::IODImagePixelBase()
    : IODModule()
{
    resetRules();
}

IODImagePixelBase::~IODImagePixelBase()
{
}

void IODImagePixelBase::resetRules()
{
    // all rules in sub classes
}

OFCondition IODImagePixelBase::getSamplesPerPixel(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_SamplesPerPixel, value, pos);
}

OFCondition IODImagePixelBase::getPhotometricInterpretation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PhotometricInterpretation, *m_Item, value, pos);
}

OFCondition IODImagePixelBase::getRows(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_Rows, value, pos);
}

OFCondition IODImagePixelBase::getColumns(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_Columns, value, pos);
}

OFCondition IODImagePixelBase::getBitsAllocated(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_BitsAllocated, value, pos);
}

OFCondition IODImagePixelBase::getPixelAspectRatio(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_PixelAspectRatio, value, pos);
}

OFCondition IODImagePixelBase::setRows(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_Rows, value);
}

OFCondition IODImagePixelBase::setColumns(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_Columns, value);
}

OFCondition IODImagePixelBase::setPixelAspectRatio(const OFString& verticalPixelSize,
                                                   const OFString& horizontalPixelSize,
                                                   const OFBool checkValue)
{
    OFString concat = verticalPixelSize;
    concat += "\\";
    concat += horizontalPixelSize;
    OFCondition cond;
    if (checkValue)
    {
        cond = DcmIntegerString::checkStringValue(concat, "2");
        // check for unsignedness, too?
    }
    if (cond.good())
        m_Item->putAndInsertOFStringArray(DCM_PixelAspectRatio, concat);
    return cond;
}
