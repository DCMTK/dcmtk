/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRDRS8_H
#define DRTRDRS8_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for ReferencedDoseReferenceSequence (300c,0050) in RTFractionSchemeModule
 */
class DCMTK_DCMRT_EXPORT DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule
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

        /** get ConstraintWeight (300a,0021)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getConstraintWeight(OFString &value, const signed long pos = 0) const;

        /** get ConstraintWeight (300a,0021)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getConstraintWeight(Float64 &value, const unsigned long pos = 0) const;

        /** get DeliveryMaximumDose (300a,0023)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveryMaximumDose(OFString &value, const signed long pos = 0) const;

        /** get DeliveryMaximumDose (300a,0023)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveryMaximumDose(Float64 &value, const unsigned long pos = 0) const;

        /** get DeliveryWarningDose (300a,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveryWarningDose(OFString &value, const signed long pos = 0) const;

        /** get DeliveryWarningDose (300a,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveryWarningDose(Float64 &value, const unsigned long pos = 0) const;

        /** get OrganAtRiskFullVolumeDose (300a,002a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskFullVolumeDose(OFString &value, const signed long pos = 0) const;

        /** get OrganAtRiskFullVolumeDose (300a,002a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskFullVolumeDose(Float64 &value, const unsigned long pos = 0) const;

        /** get OrganAtRiskLimitDose (300a,002b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskLimitDose(OFString &value, const signed long pos = 0) const;

        /** get OrganAtRiskLimitDose (300a,002b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskLimitDose(Float64 &value, const unsigned long pos = 0) const;

        /** get OrganAtRiskMaximumDose (300a,002c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskMaximumDose(OFString &value, const signed long pos = 0) const;

        /** get OrganAtRiskMaximumDose (300a,002c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskMaximumDose(Float64 &value, const unsigned long pos = 0) const;

        /** get OrganAtRiskOverdoseVolumeFraction (300a,002d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskOverdoseVolumeFraction(OFString &value, const signed long pos = 0) const;

        /** get OrganAtRiskOverdoseVolumeFraction (300a,002d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOrganAtRiskOverdoseVolumeFraction(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedDoseReferenceNumber (300c,0051)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedDoseReferenceNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedDoseReferenceNumber (300c,0051)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedDoseReferenceNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get TargetMaximumDose (300a,0027)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetMaximumDose(OFString &value, const signed long pos = 0) const;

        /** get TargetMaximumDose (300a,0027)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetMaximumDose(Float64 &value, const unsigned long pos = 0) const;

        /** get TargetMinimumDose (300a,0025)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetMinimumDose(OFString &value, const signed long pos = 0) const;

        /** get TargetMinimumDose (300a,0025)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetMinimumDose(Float64 &value, const unsigned long pos = 0) const;

        /** get TargetPrescriptionDose (300a,0026)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetPrescriptionDose(OFString &value, const signed long pos = 0) const;

        /** get TargetPrescriptionDose (300a,0026)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetPrescriptionDose(Float64 &value, const unsigned long pos = 0) const;

        /** get TargetUnderdoseVolumeFraction (300a,0028)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetUnderdoseVolumeFraction(OFString &value, const signed long pos = 0) const;

        /** get TargetUnderdoseVolumeFraction (300a,0028)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTargetUnderdoseVolumeFraction(Float64 &value, const unsigned long pos = 0) const;

      // --- set DICOM attribute values ---

        /** set ConstraintWeight (300a,0021)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setConstraintWeight(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveryMaximumDose (300a,0023)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveryMaximumDose(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveryWarningDose (300a,0022)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveryWarningDose(const OFString &value, const OFBool check = OFTrue);

        /** set OrganAtRiskFullVolumeDose (300a,002a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOrganAtRiskFullVolumeDose(const OFString &value, const OFBool check = OFTrue);

        /** set OrganAtRiskLimitDose (300a,002b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOrganAtRiskLimitDose(const OFString &value, const OFBool check = OFTrue);

        /** set OrganAtRiskMaximumDose (300a,002c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOrganAtRiskMaximumDose(const OFString &value, const OFBool check = OFTrue);

        /** set OrganAtRiskOverdoseVolumeFraction (300a,002d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOrganAtRiskOverdoseVolumeFraction(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedDoseReferenceNumber (300c,0051)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedDoseReferenceNumber(const OFString &value, const OFBool check = OFTrue);

        /** set TargetMaximumDose (300a,0027)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTargetMaximumDose(const OFString &value, const OFBool check = OFTrue);

        /** set TargetMinimumDose (300a,0025)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTargetMinimumDose(const OFString &value, const OFBool check = OFTrue);

        /** set TargetPrescriptionDose (300a,0026)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTargetPrescriptionDose(const OFString &value, const OFBool check = OFTrue);

        /** set TargetUnderdoseVolumeFraction (300a,0028)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTargetUnderdoseVolumeFraction(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ConstraintWeight (300a,0021) vr=DS, vm=1, type=3
        DcmDecimalString ConstraintWeight;
        /// DeliveryMaximumDose (300a,0023) vr=DS, vm=1, type=3
        DcmDecimalString DeliveryMaximumDose;
        /// DeliveryWarningDose (300a,0022) vr=DS, vm=1, type=3
        DcmDecimalString DeliveryWarningDose;
        /// OrganAtRiskFullVolumeDose (300a,002a) vr=DS, vm=1, type=3
        DcmDecimalString OrganAtRiskFullVolumeDose;
        /// OrganAtRiskLimitDose (300a,002b) vr=DS, vm=1, type=3
        DcmDecimalString OrganAtRiskLimitDose;
        /// OrganAtRiskMaximumDose (300a,002c) vr=DS, vm=1, type=3
        DcmDecimalString OrganAtRiskMaximumDose;
        /// OrganAtRiskOverdoseVolumeFraction (300a,002d) vr=DS, vm=1, type=3
        DcmDecimalString OrganAtRiskOverdoseVolumeFraction;
        /// ReferencedDoseReferenceNumber (300c,0051) vr=IS, vm=1, type=1
        DcmIntegerString ReferencedDoseReferenceNumber;
        /// TargetMaximumDose (300a,0027) vr=DS, vm=1, type=3
        DcmDecimalString TargetMaximumDose;
        /// TargetMinimumDose (300a,0025) vr=DS, vm=1, type=3
        DcmDecimalString TargetMinimumDose;
        /// TargetPrescriptionDose (300a,0026) vr=DS, vm=1, type=3
        DcmDecimalString TargetPrescriptionDose;
        /// TargetUnderdoseVolumeFraction (300a,0028) vr=DS, vm=1, type=3
        DcmDecimalString TargetUnderdoseVolumeFraction;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule(const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &copy);

    /** destructor
     */
    virtual ~DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &operator=(const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &copy);

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
