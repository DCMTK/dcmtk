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
 *  Purpose: Base class for Image Pixel Module and related (e.g. Floating Point)
 *
 */

#ifndef MODIMAGEPIXELBASE_H
#define MODIMAGEPIXELBASE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"

/** Base class for deriving Image Pixel Module, Floating Point Image Pixel
 *  Module and Double Floating Point Image Pixel (and maybe more later)
 */
class DCMTK_DCMIOD_EXPORT IODImagePixelBase : public IODModule
{

public:
    enum DataType
    {
        DATA_TYPE_UNKNOWN,
        DATA_TYPE_FLOAT,
        DATA_TYPE_DOUBLE,
        DATA_TYPE_INTEGER
    };

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODImagePixelBase(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODImagePixelBase();

    /** Destructor
     */
    virtual ~IODImagePixelBase();

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("ImagePixelModule")
     */
    virtual OFString getName() const;

    /** Get pixel data type
     *  @return The data type of the pixel data
     */
    virtual DataType getDataType() const = 0;

    // -------------------------------------------------------------------------
    // Getters for all attribute that appear in all Image Pixel Modules
    // -------------------------------------------------------------------------

    /** Get Samples per Pixel
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSamplesPerPixel(Uint16& value, const unsigned long pos = 0);

    /** Get Photometric Interpretation
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPhotometricInterpretation(OFString& value, const signed long pos = 0);

    /** Get Rows
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRows(Uint16& value, const unsigned long pos = 0);

    /** Get Columns
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getColumns(Uint16& value, const unsigned long pos = 0);

    /** Get Bits Allocated
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBitsAllocated(Uint16& value, const unsigned long pos = 0);

    /** Get Pixel Aspect Ratio
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelAspectRatio(Uint16& value, const unsigned long pos = 0);

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
    virtual OFCondition setRows(const Uint16 value, const OFBool checkValue = OFTrue);

    /** Set Columns
     *  @param  value Reference to variable in which the value should be stored
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setColumns(const Uint16 value, const OFBool checkValue = OFTrue);

    /** Set Pixel Aspect Ratio
     *  @param  verticalPixelSize The vertical pixel size (no unit)
     *  @param  horizontalPixelSize The horizontal pixel size (no unit)
     *  @param  checkValue Check 'value' for conformance with VR (IS)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelAspectRatio(const OFString& verticalPixelSize,
                                            const OFString& horizontalPixelSize,
                                            const OFBool checkValue = OFTrue);

private:
    /// This module's name ("ImagePixelBase")
    static const OFString m_ModuleName;
};

#endif // MODIMAGEPIXELBASE_H
