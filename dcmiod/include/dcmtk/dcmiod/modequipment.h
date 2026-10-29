/*
 *
 *  Copyright (C) 2015-2026, Open Connections GmbH
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
 *  Purpose: Class for managing the General Equipment Module
 *
 */

#ifndef MODEQUIPMENT_H
#define MODEQUIPMENT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/ofstd/ofdeprec.h"

/** Class representing the General Equipment Module:
 *
 *  Manufacturer: (LO, 1, 2)
 *  Institution Name: (LO, 1, 3)
 *  Institution Address: (ST, 1, 3)
 *  Station Name: (SH, 1, 3)
 *  Institutional Department Name: (LO, 1, 3)
 *  Manufacturer's Model Name: (LO, 1, 3)
 *  Device Serial Number: (LO, 1, 3)
 *  Software Version(s): (LO, 1-n, 3)
 *  Pixel Padding Value: (US or SS, 1, 3)
 */
class DCMTK_DCMIOD_EXPORT IODGeneralEquipmentModule : public IODModule
{

public:
    /** Convenient struct containing commonly used equipment information
     *  (for use by external code)
     */
    struct DCMTK_DCMIOD_EXPORT EquipmentInfo
    {

        /** Default Constructor
         */
        EquipmentInfo()
            : m_Manufacturer()
            , m_ManufacturerModelName()
            , m_DeviceSerialNumber()
            , m_SoftwareVersions()
        {
        }

        /** Convenience Constructor setting commonly used values
         *  @param manufacturer Manufacturer
         *  @param manufacturerModelName Manufacturer's model name
         *  @param deviceSerialNumber Serial number
         *  @param softwareVersions Software versions
         */
        EquipmentInfo(const OFString& manufacturer,
                      const OFString& manufacturerModelName,
                      const OFString& deviceSerialNumber,
                      const OFString& softwareVersions)
            : m_Manufacturer(manufacturer)
            , m_ManufacturerModelName(manufacturerModelName)
            , m_DeviceSerialNumber(deviceSerialNumber)
            , m_SoftwareVersions(softwareVersions)
        {
        }

        /// Manufacturer (VM 1)
        OFString m_Manufacturer;

        /// Manufacturer's Model Name (VM 1)
        OFString m_ManufacturerModelName;

        /// Device Serial Number (VM 1)
        OFString m_DeviceSerialNumber;

        /// Software Version(s) (VM 1-n)
        OFString m_SoftwareVersions;
    };

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the class
     *          creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODGeneralEquipmentModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODGeneralEquipmentModule();

    /** Destructor
     */
    virtual ~IODGeneralEquipmentModule();

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("GeneralEquipmentModule")
     */
    virtual OFString getName() const;

    /** Get Manufacturer
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturer(OFString& value, const signed long pos = 0) const;

    /** Get Institution Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionName(OFString& value, const signed long pos = 0) const;

    /** Get Institution Address
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionAddress(OFString& value, const signed long pos = 0) const;

    /** Get Station Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStationName(OFString& value, const signed long pos = 0) const;

    /** Get Institutional Department Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionalDepartmentName(OFString& value, const signed long pos = 0) const;
    /** Get Manufacturer's Model Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturerModelName(OFString& value, const signed long pos = 0) const;

    /** Get Device Serial Number
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeviceSerialNumber(OFString& value, const signed long pos = 0) const;

    /** Get Software Version(s)
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSoftwareVersions(OFString& value, const signed long pos = 0) const;

    /** Get Pixel Padding Value.
     *  @note Pixel Padding Value (0028,0120) has VR "US or SS"; the value
     *        representation depends on Pixel Representation (0028,0103). This
     *        method returns the unsigned (US) interpretation, which is the
     *        correct one whenever Pixel Representation is 0 (e.g.\ for
     *        Segmentation objects). For signed data, retrieve the element
     *        directly instead.
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelPaddingValue(Uint16& value, const unsigned long pos = 0) const;

    /** Get Pixel Padding Value, signed (SS) interpretation.
     *  @note Use this overload when Pixel Representation (0028,0103) is 1, i.e.\
     *        the pixel data is signed. For unsigned data use the Uint16 overload.
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelPaddingValue(Sint16& value, const unsigned long pos = 0) const;

    /** Get the Value Representation actually used to store Pixel Padding Value
     *  (0028,0120), which has VR "US or SS". This lets a caller decide which
     *  getPixelPaddingValue() overload applies to a value read from a dataset,
     *  rather than probing both.
     *  @param  vr Returns the VR of the stored element (EVR_US or EVR_SS) if
     *          Pixel Padding Value is present
     *  @return EC_Normal if Pixel Padding Value is present, an error code (e.g.\
     *          EC_TagNotFound) otherwise
     */
    virtual OFCondition getPixelPaddingValueVR(DcmEVR& vr) const;

    /** Check whether Pixel Padding Value (0028,0120) is present and stored with a
     *  signed (SS) Value Representation. Convenience wrapper around
     *  getPixelPaddingValueVR().
     *  @return OFTrue if Pixel Padding Value is present and signed (SS); OFFalse
     *          if it is absent or unsigned (US)
     */
    virtual OFBool isPixelPaddingValueSigned() const;

    /** Get a copy altogether as EquipmentInfo
     *  @return EquipmentInfo object containing all relevant information
     *    If some data is not available, it will contain an empty string
     */
    virtual IODGeneralEquipmentModule::EquipmentInfo getEquipmentInfo() const;

    /** Set Manufacturer
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturer(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Institution Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Institution Address
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionAddress(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Station Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStationName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Institutional Department Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionalDepartmentName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Institutional Department Name
     *  @deprecated This method is deprecated and will be removed in the future.
     *    Please use setInstitutionalDepartmentName() instead.
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFdeprecated_msg("use setInstitutionalDepartmentName() instead")
    virtual OFCondition setInstutionalDepartmentName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Manufacturer's Model Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturerModelName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Device Serial Number
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Software Version(s)
     *  @param  value Value to be set (possibly multi-valued) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1-n)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Pixel Padding Value.
     *  @note Pixel Padding Value (0028,0120) has VR "US or SS". This method sets
     *        the value as US (unsigned), which is correct whenever Pixel
     *        Representation (0028,0103) is 0 (e.g.\ for Labelmap Segmentation objects).
     *  @param  value Value to be set (single value only)
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelPaddingValue(const Uint16 value, const OFBool checkValue = OFTrue);

    /** Set Pixel Padding Value, signed (SS).
     *  @note Use this overload when Pixel Representation (0028,0103) is 1, i.e.\
     *        the pixel data is signed; the value is stored with VR SS.
     *  @param  value Value to be set (single value only)
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelPaddingValue(const Sint16 value, const OFBool checkValue = OFTrue);

private:
    /// Name of the module ("GeneralEquipmentModule")
    OFString m_ModuleName;
};

#endif // MODEQUIPMENT_H
