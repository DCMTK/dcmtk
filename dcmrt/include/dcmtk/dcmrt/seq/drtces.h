/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTContributingEquipmentSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTCES_H
#define DRTCES_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtois.h"    // for OperatorIdentificationSequence
#include "dcmtk/dcmrt/seq/drtporcs.h"  // for PurposeOfReferenceCodeSequence


/** Interface class for ContributingEquipmentSequence (0018,a001)
 */
class DCMTK_DCMRT_EXPORT DRTContributingEquipmentSequence
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

        /** assigment operator
         *  @param copy item object to be copied
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

        /** get ContributionDateTime (0018,a002)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContributionDateTime(OFString &value, const signed long pos = 0) const;

        /** get ContributionDescription (0018,a003)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContributionDescription(OFString &value, const signed long pos = 0) const;

        /** get DateOfLastCalibration (0018,1200)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDateOfLastCalibration(OFString &value, const signed long pos = 0) const;

        /** get DeviceSerialNumber (0018,1000)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceSerialNumber(OFString &value, const signed long pos = 0) const;

        /** get InstitutionAddress (0008,0081)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionAddress(OFString &value, const signed long pos = 0) const;

        /** get InstitutionName (0008,0080)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionName(OFString &value, const signed long pos = 0) const;

        /** get InstitutionalDepartmentName (0008,1040)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionalDepartmentName(OFString &value, const signed long pos = 0) const;

        /** get Manufacturer (0008,0070)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturer(OFString &value, const signed long pos = 0) const;

        /** get ManufacturerModelName (0008,1090)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturerModelName(OFString &value, const signed long pos = 0) const;

        /** get OperatorsName (0008,1070)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOperatorsName(OFString &value, const signed long pos = 0) const;

        /** get SoftwareVersions (0018,1020)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSoftwareVersions(OFString &value, const signed long pos = 0) const;

        /** get SpatialResolution (0018,1050)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpatialResolution(OFString &value, const signed long pos = 0) const;

        /** get SpatialResolution (0018,1050)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpatialResolution(Float64 &value, const unsigned long pos = 0) const;

        /** get StationName (0008,1010)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getStationName(OFString &value, const signed long pos = 0) const;

        /** get TimeOfLastCalibration (0018,1201)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTimeOfLastCalibration(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get OperatorIdentificationSequence (0008,1072)
         *  @return reference to sequence element
         */
        DRTOperatorIdentificationSequence &getOperatorIdentificationSequence()
            { return OperatorIdentificationSequence; }

        /** get OperatorIdentificationSequence (0008,1072)
         *  @return const reference to sequence element
         */
        const DRTOperatorIdentificationSequence &getOperatorIdentificationSequence() const
            { return OperatorIdentificationSequence; }

        /** get PurposeOfReferenceCodeSequence (0040,a170)
         *  @return reference to sequence element
         */
        DRTPurposeOfReferenceCodeSequence &getPurposeOfReferenceCodeSequence()
            { return PurposeOfReferenceCodeSequence; }

        /** get PurposeOfReferenceCodeSequence (0040,a170)
         *  @return const reference to sequence element
         */
        const DRTPurposeOfReferenceCodeSequence &getPurposeOfReferenceCodeSequence() const
            { return PurposeOfReferenceCodeSequence; }

      // --- set DICOM attribute values ---

        /** set ContributionDateTime (0018,a002)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContributionDateTime(const OFString &value, const OFBool check = OFTrue);

        /** set ContributionDescription (0018,a003)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContributionDescription(const OFString &value, const OFBool check = OFTrue);

        /** set DateOfLastCalibration (0018,1200)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DA) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDateOfLastCalibration(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceSerialNumber (0018,1000)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceSerialNumber(const OFString &value, const OFBool check = OFTrue);

        /** set InstitutionAddress (0008,0081)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionAddress(const OFString &value, const OFBool check = OFTrue);

        /** set InstitutionName (0008,0080)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionName(const OFString &value, const OFBool check = OFTrue);

        /** set InstitutionalDepartmentName (0008,1040)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionalDepartmentName(const OFString &value, const OFBool check = OFTrue);

        /** set Manufacturer (0008,0070)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set ManufacturerModelName (0008,1090)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturerModelName(const OFString &value, const OFBool check = OFTrue);

        /** set OperatorsName (0008,1070)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOperatorsName(const OFString &value, const OFBool check = OFTrue);

        /** set SoftwareVersions (0018,1020)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSoftwareVersions(const OFString &value, const OFBool check = OFTrue);

        /** set SpatialResolution (0018,1050)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSpatialResolution(const OFString &value, const OFBool check = OFTrue);

        /** set StationName (0008,1010)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setStationName(const OFString &value, const OFBool check = OFTrue);

        /** set TimeOfLastCalibration (0018,1201)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (TM) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTimeOfLastCalibration(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ContributionDateTime (0018,a002) vr=DT, vm=1, type=3
        DcmDateTime ContributionDateTime;
        /// ContributionDescription (0018,a003) vr=ST, vm=1, type=3
        DcmShortText ContributionDescription;
        /// DateOfLastCalibration (0018,1200) vr=DA, vm=1-n, type=3
        DcmDate DateOfLastCalibration;
        /// DeviceSerialNumber (0018,1000) vr=LO, vm=1, type=3
        DcmLongString DeviceSerialNumber;
        /// InstitutionAddress (0008,0081) vr=ST, vm=1, type=3
        DcmShortText InstitutionAddress;
        /// InstitutionName (0008,0080) vr=LO, vm=1, type=3
        DcmLongString InstitutionName;
        /// InstitutionalDepartmentName (0008,1040) vr=LO, vm=1, type=3
        DcmLongString InstitutionalDepartmentName;
        /// Manufacturer (0008,0070) vr=LO, vm=1, type=1
        DcmLongString Manufacturer;
        /// ManufacturerModelName (0008,1090) vr=LO, vm=1, type=3
        DcmLongString ManufacturerModelName;
        /// OperatorIdentificationSequence (0008,1072) vr=SQ, vm=1, type=3
        DRTOperatorIdentificationSequence OperatorIdentificationSequence;
        /// OperatorsName (0008,1070) vr=PN, vm=1-n, type=3
        DcmPersonName OperatorsName;
        /// PurposeOfReferenceCodeSequence (0040,a170) vr=SQ, vm=1, type=1
        DRTPurposeOfReferenceCodeSequence PurposeOfReferenceCodeSequence;
        /// SoftwareVersions (0018,1020) vr=LO, vm=1-n, type=3
        DcmLongString SoftwareVersions;
        /// SpatialResolution (0018,1050) vr=DS, vm=1, type=3
        DcmDecimalString SpatialResolution;
        /// StationName (0008,1010) vr=SH, vm=1, type=3
        DcmShortString StationName;
        /// TimeOfLastCalibration (0018,1201) vr=TM, vm=1-n, type=3
        DcmTime TimeOfLastCalibration;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTContributingEquipmentSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTContributingEquipmentSequence(const DRTContributingEquipmentSequence &copy);

    /** destructor
     */
    virtual ~DRTContributingEquipmentSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTContributingEquipmentSequence &operator=(const DRTContributingEquipmentSequence &copy);

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
    unsigned long getNumberOfItems() const;

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
    OFCondition gotoItem(const unsigned long num);

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
    OFCondition getItem(const unsigned long num, Item *&item);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getItem(const unsigned long num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getItem(const unsigned long num) const;

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &operator[](const unsigned long num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &operator[](const unsigned long num) const;

    /** add new item to the end of this sequence
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(Item *&item);

    /** insert new item into the sequence
     *  @param  pos   position where the new item is to be inserted (0..num)
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition insertItem(const unsigned long pos, Item *&item);

    /** remove particular item from the sequence
     *  @param  pos  position of the item to be removed (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const unsigned long pos);

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
    OFCondition gotoItem(const unsigned long num,
                         OFListIterator(Item *) &iterator);

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const unsigned long num,
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
