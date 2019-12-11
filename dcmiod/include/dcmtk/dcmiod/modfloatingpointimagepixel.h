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
 *  Purpose: Floating Point and Double Floating Point Image Pixel Module
 *
 */

#ifndef MODFLOATINGPOINTIMAGEPIXEL_H
#define MODFLOATINGPOINTIMAGEPIXEL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/modimagepixelbase.h"

/** Class representing Floating Point Image Pixel Module:
 *
 *  Samples Per Pixel: (US, 1, 1)
 *  Photometric Interpretation (CS, 1, 1)
 *  Rows: (US, 1, 1)
 *  Columns: (US, 1, 1)
 *  Bits Allocated: (US, 1 1)
 *  Pixel Aspect Ratio: (IS,  2, 1C)
 *  Float Pixel Padding Value: (FL, 1, 3)
 *  Float Pixel Padding Range Limit: (FL, 1, 1C)
 */
class DCMTK_DCMIOD_EXPORT IODFloatingPointImagePixelModule : public IODImagePixelBase
{

public:
    /// Data type of pixels
    typedef Float32 value_type;

    static const DcmTagKey pixel_data_tag;

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODFloatingPointImagePixelModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODFloatingPointImagePixelModule();

    /** Destructor
     */
    virtual ~IODFloatingPointImagePixelModule();

    /** Read attributes from given item into this class
     *  @param source  The source to read from
     *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
     *         old data is overwritten (or amended)
     *  @result EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& source, const OFBool clearOldData = OFTrue);

    /** Write attributes from this class into given item
     *  @param  destination The item to write to
     *  @result EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition write(DcmItem& destination);

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("FloatingPointImagePixelModule")
     */
    virtual OFString getName() const;

    /** Get pixel data type, always returns DataType::FLOAT for this class.
     *  @return The data type of the pixel data
     */
    virtual DataType getDataType() const;

    /** Get Float Pixel Padding Value
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloatPixelPaddingValue(Float32& value, const unsigned long pos = 0);

    /** Get Float Pixel Padding Range Limit
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloatPixelPaddingRangeLimit(Float32& value, const unsigned long pos = 0);

    /** Set Float Pixel Pixel Padding Value
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value' for conformance with VR (FL) and consistency
     *    with other attributes if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFloatPixelPaddingValue(const Float32 value, const OFBool checkValue = OFTrue);

    /** Set Float Pixel Padding Range Limit
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value' for conformance with VR (FL) and consistency
     *    with other attributes if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFloatPixelPaddingRangeLimit(const Float32 value, const OFBool checkValue = OFTrue);

private:
    /// This module's name ("FloatingPointImagePixelModule")
    static const OFString m_ModuleName;
};

/** Class representing Double Floating Point Image Pixel Module:
 *
 *  Samples Per Pixel: (US, 1, 1)
 *  Photometric Interpretation (CS, 1, 1)
 *  Rows: (US, 1, 1)
 *  Columns: (US, 1, 1)
 *  Bits Allocated: (US, 1 1)
 *  Pixel Aspect Ratio: (IS,  2, 1C)
 *  Double Float Pixel Padding Value: (FD, 1, 3)
 *  Double Float Pixel Padding Range Limit: (FD, 1, 1C)
 */
class DCMTK_DCMIOD_EXPORT IODDoubleFloatingPointImagePixelModule : public IODImagePixelBase
{

public:
    /// Data type of pixels
    typedef Float64 value_type;

    static const DcmTagKey pixel_data_tag;

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODDoubleFloatingPointImagePixelModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODDoubleFloatingPointImagePixelModule();

    /** Destructor
     */
    virtual ~IODDoubleFloatingPointImagePixelModule();

    /** Read attributes from given item into this class
     *  @param source  The source to read from
     *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
     *         old data is overwritten (or amended)
     *  @result EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& source, const OFBool clearOldData = OFTrue);

    /** Write attributes from this class into given item
     *  @param  destination The item to write to
     *  @result EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition write(DcmItem& destination);

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("DoubleFloatingPointImagePixelModule")
     */
    virtual OFString getName() const;

    /** Get pixel data type, always returns DataType::DOUBLE for this class.
     *  @return The data type of the pixel data
     */
    virtual DataType getDataType() const;

    /** Get Double Float Pixel Padding Value
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoubleFloatPixelPaddingValue(Float64& value, const unsigned long pos = 0);

    /** Get Double Float Pixel Padding Range Limit
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoubleFloatPixelPaddingRangeLimit(Float64& value, const unsigned long pos = 0);

    /** Set Double Float Pixel Pixel Padding Value
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value' for conformance with VR (FD) and consistency
     *    with other attributes if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoubleFloatPixelPaddingValue(const Float64 value, const OFBool checkValue = OFTrue);

    /** Set Double Float Pixel Padding Range Limit
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value' for conformance with VR (FD) and consistency
     *    with other attributes if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoubleFloatPixelPaddingRangeLimit(const Float64 value, const OFBool checkValue = OFTrue);

private:
    /// This module's name ("DoubleFloatingPointImagePixelModule")
    static const OFString m_ModuleName;
};

#endif // MODFLOATINGPOINTIMAGEPIXEL_H
