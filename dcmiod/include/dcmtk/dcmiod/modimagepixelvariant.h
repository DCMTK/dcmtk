/*
 *
 *  Copyright (C) 2016-2019, Open Connections GmbH
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
 *  Purpose: Base class for Image Pixel Module and related (e.g. Ploating Point)
 *
 */

#ifndef MODIMAGEPIXELVARIANT_H
#define MODIMAGEPIXELVARIANT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmiod/modimagepixelbase.h"
#include "dcmtk/ofstd/ofvriant.h"

/** Visitor for returning base class of given Image Pixel Module
 */
struct IODImagePixelVariantBaseVisitor
{

    /** Returns NULL since given argument is no pixel module at all
     *  @return OFnullptr
     */
    IODImagePixelBase* operator()(OFmonostate&)
    {
        return OFnullptr;
    }

    /** Returns the base class pointer of all pixel modules
     *  @param  base IODImagePixelBase pointer to given class instance
     *  @return OFnullptr
     */
    IODImagePixelBase* operator()(IODImagePixelBase& base)
    {
        return &base;
    }
};

/** Class representing different pixel modules from the DICOM standard:
 *  The class Image Pixel Module, the Floating Point Image Pixel module
 *  and the Double Floating Point Image Pixel module
 */
#ifdef HAVE_CXX11
template <typename... Types>
class IODImagePixelVariant : public OFvariant<OFmonostate, Types...>
#else
template <OFVARIADIC_DECLARE_TEMPLATE_PARAMETER_PACK_WITH_DEFAULTS(T)>
class IODImagePixelVariant : public OFvariant<OFmonostate, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>
#endif
{

public:
    /** Constructor
     */
    IODImagePixelVariant()
        : IODImagePixelVariant::variant()
    {
    }

    /** Constructor, constructs pixel module based on given type
     *  @param  t The Image Pixel Module to be used
     */
    template <typename T>
    IODImagePixelVariant(const T& t)
        : IODImagePixelVariant::variant(t)
    {
    }

    /** Destructor
     */
    ~IODImagePixelVariant()
    {
    }

    /** Get name of module
     *  @return Name of the actual module (e.g. "ImagePixelModule")
     */
    OFString getName() const
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getName();
        return OFString();
    }

    /** Get pixel data type
     *  @return The data type of the pixel data
     */
    IODImagePixelBase::DataType getDataType() const
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getDataType();
        return IODImagePixelBase::DATA_TYPE_UNKNOWN;
    }

    /** Clear data (base class version)
     */
    void clearData()
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            pBase->clearData();
    }

    // -------------------------------------------------------------------------
    // Getters for all attribute that appear in all Image Pixel Modules
    // -------------------------------------------------------------------------

    /** Get Samples per Pixel
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getSamplesPerPixel(Uint16& value, const unsigned long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getSamplesPerPixel(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    /** Get Photometric Interpretation
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getPhotometricInterpretation(OFString& value, const signed long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getPhotometricInterpretation(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    /** Get Rows
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getRows(Uint16& value, const unsigned long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getRows(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    /** Get Columns
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getColumns(Uint16& value, const unsigned long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getColumns(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    /** Get Bits Allocated
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getBitsAllocated(Uint16& value, const unsigned long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getBitsAllocated(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    /** Get Pixel Aspect Ratio
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getPixelAspectRatio(Uint16& value, const unsigned long pos = 0)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->getPixelAspectRatio(value, pos);
        return IOD_EC_InvalidPixelData;
    }

    // -------------------------------------------------------------------------
    // Setters for all attribute that appear in all Image Pixel Modules and which
    // do not have fixed values in sub classes.
    // -------------------------------------------------------------------------

    /** Set Rows
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition setRows(const Uint16 value, const OFBool checkValue = OFTrue)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->setRows(value, checkValue);
        return IOD_EC_InvalidPixelData;
    }

    /** Set Columns
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition setColumns(const Uint16 value, const OFBool checkValue = OFTrue)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->setColumns(value, checkValue);
        return IOD_EC_InvalidPixelData;
    }

    /** Set Pixel Aspect Ratio
     *  @param  verticalPixelSize The vertical pixel size (no unit)
     *  @param  horizontalPixelSize The horizontal pixel size (no unit)
     *  @param  checkValue Check 'value' for conformance with VR (IS)
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition setPixelAspectRatio(const OFString& verticalPixelSize,
                                    const OFString& horizontalPixelSize,
                                    const OFBool checkValue = OFTrue)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->setPixelAspectRatio(verticalPixelSize, horizontalPixelSize, checkValue);
        return IOD_EC_InvalidPixelData;
    }

    /** Write pixel module base class data to given item
     *  @param  dataset The item to write to (usually dataset level)
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition write(DcmItem& dataset)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->write(dataset);
        return IOD_EC_InvalidPixelData;
    }

    /** Read pixel module base class data from given item
     *  @param  dataset The item to read from (usually dataset level)
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition read(DcmItem& dataset)
    {
        if (IODImagePixelBase* pBase = OFvisit<IODImagePixelBase*>(IODImagePixelVariantBaseVisitor(), *this))
            return pBase->read(dataset);
        return IOD_EC_InvalidPixelData;
    }
};

#endif // MODIMAGEPIXELBASE_H
