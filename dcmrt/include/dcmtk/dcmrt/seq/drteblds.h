/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTEnhancedRTBeamLimitingDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTEBLDS_H
#define DRTEBLDS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtdtcs.h"   // for DeviceTypeCodeSequence
#include "dcmtk/dcmrt/seq/drtfbdds.h"  // for FixedRTBeamDelimiterDeviceSequence
#include "dcmtk/dcmrt/seq/drtpbdds.h"  // for ParallelRTBeamDelimiterDeviceSequence
#include "dcmtk/dcmrt/seq/drtudis.h"   // for UDISequence


/** Interface class for EnhancedRTBeamLimitingDeviceSequence (3008,00a1)
 */
class DCMTK_DCMRT_EXPORT DRTEnhancedRTBeamLimitingDeviceSequence
  : protected DRTTypes
{

  public:

    /** Item class
     */
    class DCMTK_DCMRT_EXPORT Item
      : protected DRTTypes
    {

      public:

      // --- constructors, destructor and operators ---

        /** (default) constructor
         *  @param emptyDefaultItem flag used to mark the empty default item
         */
        Item(const OFBool emptyDefaultItem = OFFalse);

        /** copy constructor
         *  @param copy item object to be copied
         */
        Item(const Item &copy);

        /** destructor
         */
        virtual ~Item();

        /** assignment operator
         *  @param copy item object to be copied
         *  @return reference to this object
         */
        Item &operator=(const Item &copy);

      // --- general methods ---

        /** clear all internal member variables
         */
        void clear();

        /** check if item is empty
         *  @return OFTrue if item is empty, OFFalse otherwise
         */
        OFBool isEmpty();

        /** check if item is valid, i.e.\ not the empty default item
         *  @return OFTrue if item is valid, OFFalse otherwise
         */
        OFBool isValid() const;

      // --- input/output methods ---

        /** read elements from sequence item
         *  @param  item    reference to DICOM sequence item from which the elements should be read
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &item);

        /** write elements to sequence item
         *  @param  item    reference to DICOM sequence item to which the elements should be written
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &item);

      // --- get DICOM attribute values ---

        /** get BeamModifierOrientationAngle (300a,0645)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamModifierOrientationAngle(Float64 &value, const unsigned long pos = 0) const;

        /** get DeviceAlternateIdentifier (3010,001b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceAlternateIdentifier(OFString &value, const signed long pos = 0) const;

        /** get DeviceAlternateIdentifierFormat (3010,001d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceAlternateIdentifierFormat(OFString &value, const signed long pos = 0) const;

        /** get DeviceAlternateIdentifierType (3010,001c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceAlternateIdentifierType(OFString &value, const signed long pos = 0) const;

        /** get DeviceIndex (3010,0039)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceIndex(Uint16 &value, const unsigned long pos = 0) const;

        /** get DeviceLabel (3010,002d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceLabel(OFString &value, const signed long pos = 0) const;

        /** get DeviceSerialNumber (0018,1000)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceSerialNumber(OFString &value, const signed long pos = 0) const;

        /** get LongDeviceDescription (0050,0021)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getLongDeviceDescription(OFString &value, const signed long pos = 0) const;

        /** get Manufacturer (0008,0070)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturer(OFString &value, const signed long pos = 0) const;

        /** get ManufacturerDeviceIdentifier (3010,0043)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturerDeviceIdentifier(OFString &value, const signed long pos = 0) const;

        /** get ManufacturerModelName (0008,1090)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturerModelName(OFString &value, const signed long pos = 0) const;

        /** get ManufacturerModelVersion (3010,001a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturerModelVersion(OFString &value, const signed long pos = 0) const;

        /** get RTAccessoryDeviceSlotID (300a,0615)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTAccessoryDeviceSlotID(OFString &value, const signed long pos = 0) const;

        /** get RTAccessoryHolderSlotID (300a,0611)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTAccessoryHolderSlotID(OFString &value, const signed long pos = 0) const;

        /** get RTAccessorySlotDistance (300a,0613)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTAccessorySlotDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get RTBeamLimitingDeviceDistalDistance (300a,0643)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTBeamLimitingDeviceDistalDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get RTBeamLimitingDeviceProximalDistance (300a,0642)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTBeamLimitingDeviceProximalDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedDefinedDeviceIndex (300a,0602)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedDefinedDeviceIndex(Uint16 &value, const unsigned long pos = 0) const;

        /** get ReferencedRTAccessoryHolderDeviceIndex (300a,060e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedRTAccessoryHolderDeviceIndex(Uint16 &value, const unsigned long pos = 0) const;

        /** get SoftwareVersions (0018,1020)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSoftwareVersions(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get DeviceTypeCodeSequence (3010,002e)
         *  @return reference to sequence element
         */
        DRTDeviceTypeCodeSequence &getDeviceTypeCodeSequence()
            { return DeviceTypeCodeSequence; }

        /** get DeviceTypeCodeSequence (3010,002e)
         *  @return const reference to sequence element
         */
        const DRTDeviceTypeCodeSequence &getDeviceTypeCodeSequence() const
            { return DeviceTypeCodeSequence; }

        /** get FixedRTBeamDelimiterDeviceSequence (300a,0646)
         *  @return reference to sequence element
         */
        DRTFixedRTBeamDelimiterDeviceSequence &getFixedRTBeamDelimiterDeviceSequence()
            { return FixedRTBeamDelimiterDeviceSequence; }

        /** get FixedRTBeamDelimiterDeviceSequence (300a,0646)
         *  @return const reference to sequence element
         */
        const DRTFixedRTBeamDelimiterDeviceSequence &getFixedRTBeamDelimiterDeviceSequence() const
            { return FixedRTBeamDelimiterDeviceSequence; }

        /** get ParallelRTBeamDelimiterDeviceSequence (300a,0647)
         *  @return reference to sequence element
         */
        DRTParallelRTBeamDelimiterDeviceSequence &getParallelRTBeamDelimiterDeviceSequence()
            { return ParallelRTBeamDelimiterDeviceSequence; }

        /** get ParallelRTBeamDelimiterDeviceSequence (300a,0647)
         *  @return const reference to sequence element
         */
        const DRTParallelRTBeamDelimiterDeviceSequence &getParallelRTBeamDelimiterDeviceSequence() const
            { return ParallelRTBeamDelimiterDeviceSequence; }

        /** get UDISequence (0018,100a)
         *  @return reference to sequence element
         */
        DRTUDISequence &getUDISequence()
            { return UDISequence; }

        /** get UDISequence (0018,100a)
         *  @return const reference to sequence element
         */
        const DRTUDISequence &getUDISequence() const
            { return UDISequence; }

      // --- set DICOM attribute values ---

        /** set BeamModifierOrientationAngle (300a,0645)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamModifierOrientationAngle(const Float64 value, const unsigned long pos = 0);

        /** set DeviceAlternateIdentifier (3010,001b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UC) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceAlternateIdentifier(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceAlternateIdentifierFormat (3010,001d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceAlternateIdentifierFormat(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceAlternateIdentifierType (3010,001c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceAlternateIdentifierType(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceIndex (3010,0039)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceIndex(const Uint16 value, const unsigned long pos = 0);

        /** set DeviceLabel (3010,002d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceLabel(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceSerialNumber (0018,1000)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceSerialNumber(const OFString &value, const OFBool check = OFTrue);

        /** set LongDeviceDescription (0050,0021)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setLongDeviceDescription(const OFString &value, const OFBool check = OFTrue);

        /** set Manufacturer (0008,0070)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set ManufacturerDeviceIdentifier (3010,0043)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturerDeviceIdentifier(const OFString &value, const OFBool check = OFTrue);

        /** set ManufacturerModelName (0008,1090)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturerModelName(const OFString &value, const OFBool check = OFTrue);

        /** set ManufacturerModelVersion (3010,001a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturerModelVersion(const OFString &value, const OFBool check = OFTrue);

        /** set RTAccessoryDeviceSlotID (300a,0615)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTAccessoryDeviceSlotID(const OFString &value, const OFBool check = OFTrue);

        /** set RTAccessoryHolderSlotID (300a,0611)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTAccessoryHolderSlotID(const OFString &value, const OFBool check = OFTrue);

        /** set RTAccessorySlotDistance (300a,0613)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTAccessorySlotDistance(const Float64 value, const unsigned long pos = 0);

        /** set RTBeamLimitingDeviceDistalDistance (300a,0643)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTBeamLimitingDeviceDistalDistance(const Float64 value, const unsigned long pos = 0);

        /** set RTBeamLimitingDeviceProximalDistance (300a,0642)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTBeamLimitingDeviceProximalDistance(const Float64 value, const unsigned long pos = 0);

        /** set ReferencedDefinedDeviceIndex (300a,0602)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedDefinedDeviceIndex(const Uint16 value, const unsigned long pos = 0);

        /** set ReferencedRTAccessoryHolderDeviceIndex (300a,060e)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedRTAccessoryHolderDeviceIndex(const Uint16 value, const unsigned long pos = 0);

        /** set SoftwareVersions (0018,1020)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSoftwareVersions(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BeamModifierOrientationAngle (300a,0645) vr=FD, vm=1, type=1
        DcmFloatingPointDouble BeamModifierOrientationAngle;
        /// DeviceAlternateIdentifier (3010,001b) vr=UC, vm=1, type=2
        DcmUnlimitedCharacters DeviceAlternateIdentifier;
        /// DeviceAlternateIdentifierFormat (3010,001d) vr=LT, vm=1, type=1C
        DcmLongText DeviceAlternateIdentifierFormat;
        /// DeviceAlternateIdentifierType (3010,001c) vr=CS, vm=1, type=1C
        DcmCodeString DeviceAlternateIdentifierType;
        /// DeviceIndex (3010,0039) vr=US, vm=1, type=1
        DcmUnsignedShort DeviceIndex;
        /// DeviceLabel (3010,002d) vr=LO, vm=1, type=1
        DcmLongString DeviceLabel;
        /// DeviceSerialNumber (0018,1000) vr=LO, vm=1, type=2
        DcmLongString DeviceSerialNumber;
        /// DeviceTypeCodeSequence (3010,002e) vr=SQ, vm=1, type=1
        DRTDeviceTypeCodeSequence DeviceTypeCodeSequence;
        /// FixedRTBeamDelimiterDeviceSequence (300a,0646) vr=SQ, vm=1, type=1C
        DRTFixedRTBeamDelimiterDeviceSequence FixedRTBeamDelimiterDeviceSequence;
        /// LongDeviceDescription (0050,0021) vr=ST, vm=1, type=3
        DcmShortText LongDeviceDescription;
        /// Manufacturer (0008,0070) vr=LO, vm=1, type=2
        DcmLongString Manufacturer;
        /// ManufacturerDeviceIdentifier (3010,0043) vr=ST, vm=1, type=2
        DcmShortText ManufacturerDeviceIdentifier;
        /// ManufacturerModelName (0008,1090) vr=LO, vm=1, type=2
        DcmLongString ManufacturerModelName;
        /// ManufacturerModelVersion (3010,001a) vr=LO, vm=1, type=2
        DcmLongString ManufacturerModelVersion;
        /// ParallelRTBeamDelimiterDeviceSequence (300a,0647) vr=SQ, vm=1, type=1C
        DRTParallelRTBeamDelimiterDeviceSequence ParallelRTBeamDelimiterDeviceSequence;
        /// RTAccessoryDeviceSlotID (300a,0615) vr=LO, vm=1, type=2C
        DcmLongString RTAccessoryDeviceSlotID;
        /// RTAccessoryHolderSlotID (300a,0611) vr=LO, vm=1, type=2C
        DcmLongString RTAccessoryHolderSlotID;
        /// RTAccessorySlotDistance (300a,0613) vr=FD, vm=1, type=2C
        DcmFloatingPointDouble RTAccessorySlotDistance;
        /// RTBeamLimitingDeviceDistalDistance (300a,0643) vr=FD, vm=1, type=2
        DcmFloatingPointDouble RTBeamLimitingDeviceDistalDistance;
        /// RTBeamLimitingDeviceProximalDistance (300a,0642) vr=FD, vm=1, type=2
        DcmFloatingPointDouble RTBeamLimitingDeviceProximalDistance;
        /// ReferencedDefinedDeviceIndex (300a,0602) vr=US, vm=1, type=1C
        DcmUnsignedShort ReferencedDefinedDeviceIndex;
        /// ReferencedRTAccessoryHolderDeviceIndex (300a,060e) vr=US, vm=1, type=2C
        DcmUnsignedShort ReferencedRTAccessoryHolderDeviceIndex;
        /// SoftwareVersions (0018,1020) vr=LO, vm=1-n, type=2
        DcmLongString SoftwareVersions;
        /// UDISequence (0018,100a) vr=SQ, vm=1, type=3
        DRTUDISequence UDISequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTEnhancedRTBeamLimitingDeviceSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTEnhancedRTBeamLimitingDeviceSequence(const DRTEnhancedRTBeamLimitingDeviceSequence &copy);

    /** destructor
     */
    virtual ~DRTEnhancedRTBeamLimitingDeviceSequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTEnhancedRTBeamLimitingDeviceSequence &operator=(const DRTEnhancedRTBeamLimitingDeviceSequence &copy);

  // --- general methods ---

    /** clear all internal member variables
     */
    void clear();

    /** check if sequence is empty
     *  @return OFTrue if sequence is empty, OFFalse otherwise
     */
    OFBool isEmpty();

    /** check if sequence is valid, i.e.\ not the empty default sequence
     *  @return OFTrue if sequence is valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** get number of items in the sequence
     *  @return number of items
     */
    size_t getNumberOfItems() const;

    /** goto first item in the sequence
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoFirstItem();

    /** goto next item in the sequence
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoNextItem();

    /** goto particular item in the sequence
     *  @param  num  number of the item to be selected (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const size_t num);

    /** get current item in the sequence
     *  @param  item  reference to item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getCurrentItem(Item *&item) const;

    /** get current item in the sequence
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getCurrentItem();

    /** get current item in the sequence
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getCurrentItem() const;

    /** get particular item in the sequence
     *  @param  num   number of the item to be retrieved (0..num-1)
     *  @param  item  reference to item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItem(const size_t num, Item *&item);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getItem(const size_t num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getItem(const size_t num) const;

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &operator[](const size_t num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &operator[](const size_t num) const;

    /** create and add new item to the end of this sequence
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(Item *&item);

    /** create and insert new item into the sequence
     *  @param  pos   position where the new item is to be inserted (0..num)
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition insertItem(const size_t pos, Item *&item);

    /** remove particular item from the sequence
     *  @param  pos  position of the item to be removed (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const size_t pos);

  // --- input/output methods ---

    /** read sequence of items from dataset
     *  @param  dataset     reference to DICOM dataset from which the sequence should be read
     *  @param  card        cardinality (valid range for number of items)
     *  @param  type        value type (valid value: "1", "1C", "2" or something else which is not checked)
     *  @param  moduleName  optional module/sequence name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const OFString &card,
                     const OFString &type,
                     const char *moduleName = NULL);

    /** write sequence of items to dataset
     *  @param  dataset     reference to DICOM dataset to which the sequence should be written
     *  @param  card        cardinality (valid range for number of items)
     *  @param  type        value type (valid value: "1", "2" or something else which is not checked)
     *  @param  moduleName  optional module/sequence name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      const OFString &card,
                      const OFString &type,
                      const char *moduleName = NULL);

  protected:

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const size_t num,
                         OFListIterator(Item *) &iterator);

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const size_t num,
                         OFListConstIterator(Item *) &iterator) const;

  private:

    /// internal flag used to mark the empty default sequence
    /*const*/ OFBool EmptyDefaultSequence;

    /// list of items in this sequence
    OFList<Item *>         SequenceOfItems;
    /// currently selected item
    OFListIterator(Item *) CurrentItem;
    /// empty default item
    Item                   EmptyItem;

};


#endif
