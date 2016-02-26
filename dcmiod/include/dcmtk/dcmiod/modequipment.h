/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 */
class DCMTK_DCMIOD_EXPORT IODGeneralEquipmentModule : public IODModule
{

public:

    /** Convenient struct containing all information required for setting
     *  equipment information for use by external code)
     */
    struct EquipmentInfo
    {
      // Manufacturer (VM 1)
      OFString m_Manufacturer;

      // Manufacturer's Model Name (VM 1)
      OFString m_ManufacturerModelName;

      // Device Serial Number (VM 1)
      OFString m_DeviceSerialNumber;

      // Software Version(s) (VM 1-n)
      OFString m_SoftwareVersions;
    };

    /** Constructor
    *  @param  item The item to be used for data storage. If NULL, the
    *          class creates an empty data container.
    *  @param  rules The rule set for this class. If NULL, the class creates
    *          one from scratch and adds its values.
    */
    IODGeneralEquipmentModule(OFshared_ptr<DcmItem> item,
                              OFshared_ptr<IODRules> rules);

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
     *          depending on configuration)
     */
    virtual OFString getName() const;

    /** Get Manufacturer
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturer(OFString &value,
                                        const signed long pos = 0) const;

    /** Get Institution Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionName(OFString &value,
                                           const signed long pos = 0) const;

    /** Get Institution Address
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionAddress(OFString &value,
                                              const signed long pos = 0) const;

    /** Get Station Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStationName(OFString &value,
                                       const signed long pos = 0) const;

    /** Get Institutional Department Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionalDepartmentName(OFString &value,
                                                       const signed long pos = 0) const;
    /** Get Manufacturer's Model Name
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturerModelName(OFString &value,
                                                 const signed long pos = 0) const;

    /** Get Device Serial Number
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeviceSerialNumber(OFString &value,
                                              const signed long pos = 0) const;

    /** Get Software Version(s)
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSoftwareVersions(OFString &value,
                                            const signed long pos = 0) const;

    /** Set Manufacturer
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturer(const OFString &value,
                                        const OFBool checkValue = OFTrue);

    /** Set Institution Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionName(const OFString &value,
                                           const OFBool checkValue = OFTrue);

    /** Set Institution Address
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionAddress(const OFString &value,
                                              const OFBool checkValue = OFTrue);

    /** Set Station Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStationName(const OFString &value,
                                       const OFBool checkValue = OFTrue);

    /** Set Institutional Department Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstutionalDepartmentName(const OFString &value,
                                                     const OFBool checkValue = OFTrue);

    /** Set Manufacturer's Model Name
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturerModelName(const OFString &value,
                                                 const OFBool checkValue = OFTrue);

    /** Set Device Serial Number
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString &value,
                                              const OFBool checkValue = OFTrue);

    /** Set Software Version(s)
     *  @param  value Value to be set (possibly multi-valued) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1-n)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString &value,
                                            const OFBool checkValue = OFTrue);

private:

    /// Name of the modue ("GeneralEquipmentModule")
    OFString m_ModuleName;
};


#endif // MODEQUIPMENT_H
