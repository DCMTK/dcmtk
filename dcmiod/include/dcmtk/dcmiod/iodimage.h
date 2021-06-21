/*
 *
 *  Copyright (C) 2015-2021, Open Connections GmbH
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
 *  Purpose: Class repres. image IOD by exposing common image IOD attributes
 *
 */

#ifndef IODIMAGE_H
#define IODIMAGE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodcommn.h"
#include "dcmtk/dcmiod/modgeneralimage.h"
#include "dcmtk/dcmiod/modimagepixelvariant.h"
#include "dcmtk/ofstd/ofutil.h"
#include "dcmtk/ofstd/ofvriant.h"

class IODDoubleFloatingPointImagePixelModule;
class IODFloatingPointImagePixelModule;
template <typename>
class IODImagePixelModule;

/** Class for managing common image IOD attributes. At the moment support for
 *  the palette-related attributes and the smallest/largest image pixel value
 *  attributes is still missing. Also, pixel data must be handled separately.
 *  However, pixel-data related attributes are provided by the Image Pixel
 *  Module, the Floating Point Image Pixel Module or the Double Floating Point
 *  Image Pixel Module, based on the provided template parameter.
 *  Modules supported on top of DcmIODCommon: General Image Module and
 *  Image Pixel Module.
 */
#ifdef HAVE_CXX11
template <typename T, typename... Types>
struct DcmIODImageHasType : std::false_type
{
};

template <typename T, typename... Types>
struct DcmIODImageHasType<T, T, Types...> : std::true_type
{
};

template <typename T, typename T0, typename... Types>
struct DcmIODImageHasType<T, T0, Types...> : DcmIODImageHasType<T, Types...>::type
{
};

template <typename... Types>
#else
template <OFVARIADIC_DECLARE_TEMPLATE_PARAMETER_PACK_WITH_DEFAULTS(T)>
#endif
class DcmIODImage : public DcmIODCommon
{

public:
    /** typedef for old compilers that do not define the type 'DcmIODImage' in
     *  derived classes.
     */
    typedef DcmIODImage IODImage;

    /** A good comment would be nice, but I have nothing in mind
     */
#ifdef HAVE_CXX11
    using IODImagePixelModuleType = IODImagePixelVariant<Types...>;
#else
    typedef IODImagePixelVariant<OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)> IODImagePixelModuleType;
#endif

    /** Constructor, creates new DcmIODImage instance with integer-based pixel data.
     */
    DcmIODImage()
        : DcmIODCommon()
        , m_GeneralImageModuleEnabled(OFTrue)
        , m_GeneralImage(getData(), getRules())
        , m_ImagePixel()
    {
    }

    /** Constructor, constructs new DcmIODImage instance with integer, float or
     *  double based Image Pixel Module (i. e. Image Pixel Module, Floating Point
     *  Image Pixel Module or the Double Floating Point Image Pixel Module), based
     *  on the provided Image Pixel module type.
     */
    template <typename ImagePixel>
    DcmIODImage(OFin_place_type_t(ImagePixel))
        : DcmIODCommon()
        , m_GeneralImageModuleEnabled(OFTrue)
        , m_GeneralImage(getData(), getRules())
        , m_ImagePixel(ImagePixel(getData(), getRules()))
    {
    }

    /** Virtual Destructor
     */
    virtual ~DcmIODImage()
    {
    }

    virtual void setGeneralImageModuleEnabled(const OFBool enabled)
    {
        m_GeneralImageModuleEnabled = enabled;
    }

    virtual OFBool getGeneralImageModuleEnabled() const
    {
        return m_GeneralImageModuleEnabled;
    }

    /** Get General Image Module
     *  @return Reference to General Image Module
     */
    IODGeneralImageModule& getGeneralImage()
    {
        return m_GeneralImage;
    }

    /** Get Image Pixel Module (variant)
     *  @return Reference to Image Pixel Module
     */
    IODImagePixelModuleType& getImagePixel()
    {
        return m_ImagePixel;
    }

    /** Clear (removes) all attributes handled by the modules of this IOD.
     *  IOD Rules are not reset.
     */
    virtual void clearData()
    {
        DcmIODCommon::clearData();
        m_GeneralImage.clearData();
        m_ImagePixel.clearData();
    }

    /** Read common image module attributes (all those handled by this class)
     *  from given item. Reads attributes from base class DcmIODCommon before.
     *  The current content is deleted even if the reading process fails.
     *  If the log stream is set and valid the reason for any error might be
     *  obtained from the error/warning output.
     *  @param  dataset  Reference to DICOM dataset from which the document
     *          should be read
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem& dataset)
    {
        // re-initialize object
        clearData();

        // read from base classes
        DcmIODCommon::read(dataset);
        if (m_GeneralImageModuleEnabled)
        {
            m_GeneralImage.read(dataset, OFTrue /* clear old data */);
        }

        return readFloatingPointDoubleImagePixel<IODDoubleFloatingPointImagePixelModule>(dataset);
    }

    /** Write current common image module's attributes to DICOM dataset. Also
     *  writes attributes of base class DcmIODCommon afterwards.
     *  @param  dataset Reference to DICOM dataset to which the current data
     *          should be written. The dataset is not cleared before writing
     *          to it!
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem& dataset)
    {
        OFCondition result = EC_Normal;

        // Write base class attributes to dataset
        result = DcmIODCommon::write(dataset);

        // Write extra modules to dataset
        if (result.good() && m_GeneralImageModuleEnabled)
            result = m_GeneralImage.write(dataset);
        if (result.good())
            result = m_ImagePixel.write(dataset);

        return result;
    }

private:
    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<DcmIODImageHasType<T, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<T, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value != ~OFstatic_cast(size_t,0)), OFCondition>::type
#endif
    readFloatingPointDoubleImagePixel(DcmItem& dataset)
    {
        if (dataset.tagExists(DCM_DoubleFloatPixelData))
            return OFget<T>(&(m_ImagePixel = T(getData(), getRules())))->read(dataset);
        return readFloatingPointImagePixel<IODFloatingPointImagePixelModule>(dataset);
    }

    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<!DcmIODImageHasType<T, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<T, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value == ~OFstatic_cast(size_t,0)), OFCondition>::type
#endif
    readFloatingPointDoubleImagePixel(DcmItem& dataset)
    {
        return readFloatingPointImagePixel<IODFloatingPointImagePixelModule>(dataset);
    }

    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<DcmIODImageHasType<T, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<T, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value != ~OFstatic_cast(size_t,0)), OFCondition>::type
#endif
    readFloatingPointImagePixel(DcmItem& dataset)
    {
        if (dataset.tagExists(DCM_FloatPixelData))
            return OFget<T>(&(m_ImagePixel = T(getData(), getRules())))->read(dataset);
        return readIntegerImagePixel(dataset);
    }

    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<!DcmIODImageHasType<T, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<T, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value == ~OFstatic_cast(size_t,0)), OFCondition>::type
#endif
    readFloatingPointImagePixel(DcmItem& dataset)
    {
        return readIntegerImagePixel(dataset);
    }

    OFCondition readIntegerImagePixel(DcmItem& dataset)
    {
        Uint16 allocated = 0;
        Uint16 representation = 0;
        // Pixel data is not managed right now by Image Pixel Module class.
        if (/*  dataset.tagExists(DCM_PixelData) && */ dataset.findAndGetUint16(DCM_BitsAllocated, allocated).good()
            && dataset.findAndGetUint16(DCM_PixelRepresentation, representation).good())
            switch ((allocated > 8 ? 2 : 0) | (representation ? 1 : 0))
            {
                case 0:
                    return readImagePixel<Uint8>(dataset);
                case 1:
                    return readImagePixel<Sint8>(dataset);
                case 2:
                    return readImagePixel<Uint16>(dataset);
                case 3:
                    return readImagePixel<Sint16>(dataset);
                default:
                    break;
            }
        return IOD_EC_InvalidPixelData;
    }

    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<DcmIODImageHasType<IODImagePixelModule<T>, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<IODImagePixelModule<T>, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value != ~OFstatic_cast(size_t,0)),
                    OFCondition>::type
#endif
    readImagePixel(DcmItem& dataset)
    {
        return OFget<IODImagePixelModule<T> >(&(m_ImagePixel = IODImagePixelModule<T>(getData(), getRules())))
            ->read(dataset);
    }

    template <typename T>
#ifdef HAVE_CXX11
    typename std::enable_if<!DcmIODImageHasType<IODImagePixelModule<T>, Types...>::value, OFCondition>::type
#else
    OFTypename
        OFenable_if<(OFvariadic_find_type<IODImagePixelModule<T>, OFVARIADIC_TEMPLATE_PARAMETER_PACK(T)>::value == ~OFstatic_cast(size_t,0)),
                    OFCondition>::type
#endif
    readImagePixel(DcmItem& dataset)
    {
        // Avoid compiler warning about unused parameter
        OFstatic_cast(void, dataset);
        return IOD_EC_InvalidPixelData;
    }

    /// Denotes whether General Image Module is used in this class (OFTrue) or not (OFFalse).
    /// This switch can be configured using a call to setGeneralImageModuleEnabled(). The
    /// default is OFTrue.
    OFBool m_GeneralImageModuleEnabled;

    /// General Image Module
    IODGeneralImageModule m_GeneralImage;

    /// Image Pixel Module
    IODImagePixelModuleType m_ImagePixel;
};

#endif // IODIMAGE_H
