/*
 *
 *  Copyright (C) 2015-2019, Open Connections GmbH
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing the Enhanced General Equipment Module
 *
 */

#ifndef MODENHEQUIPMENT_H
#define MODENHEQUIPMENT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Enhanced Equipment Module:
 *
 *  Manufacturer: (LO, 1, 1)
 *  Manufacturer's Model Name: (LO, 1, 1)
 *  Device Serial Number: (LO, 1, 1)
 *  Software Version(s): (LO, 1-n, 1)
 */
class DCMTK_DCMIOD_EXPORT IODEnhGeneralEquipmentModule : public IODModule
{

public:
    /** Convenient struct containing all information required for setting
     *  enhanced equipment information (for use by external code)
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

        /** Convenience Constructor setting all values
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

        /** Perform simple check whether all equipment data is filled in. Does not
         *  check VR or VM (will be checked in write() routine, though)
         *  @return OFTrue if data is complete, OFFalse otherwise
         */
        OFBool isDataComplete() const
        {
            if (m_Manufacturer.empty() || m_ManufacturerModelName.empty() || m_DeviceSerialNumber.empty()
                || m_SoftwareVersions.empty())
            {
                return OFFalse;
            }
            return OFTrue;
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
    IODEnhGeneralEquipmentModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODEnhGeneralEquipmentModule();

    /** Destructor
     */
    virtual ~IODEnhGeneralEquipmentModule();

    static OFCondition create(const EquipmentInfo& info, IODEnhGeneralEquipmentModule* equipment);

    /** Resets rules to their original values.
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("EnhancedEquipmentModule")
     */
    virtual OFString getName() const;

    /** Get Manufacturer
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturer(OFString& value, const signed long pos = 0) const;

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

    /** Set Manufacturer
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturer(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Manufacturer's Model Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturerModelName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Device Serial Number
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Software Version(s)
     *  @param  value Value to be set (possibly multi-valued) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set all equipment information at once
     *  @param  info The equipment information to be set.
     *  @return EC_Normal if values could be set, error otherwise
     */
    virtual OFCondition set(const EquipmentInfo& info);

private:
    /// Name of the module ("EnhancedEquipmentModule")
    OFString m_ModuleName;
};

#endif // MODENHEQUIPMENT_H
