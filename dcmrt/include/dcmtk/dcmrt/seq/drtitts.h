/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonToleranceTableSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTITTS_H
#define DRTITTS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbldts.h"  // for BeamLimitingDeviceToleranceSequence


/** Interface class for IonToleranceTableSequence (300a,03a0)
 */
class DCMTK_DCMRT_EXPORT DRTIonToleranceTableSequence
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

        /** get BeamLimitingDeviceAngleTolerance (300a,0046)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamLimitingDeviceAngleTolerance(OFString &value, const signed long pos = 0) const;

        /** get BeamLimitingDeviceAngleTolerance (300a,0046)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamLimitingDeviceAngleTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get GantryAngleTolerance (300a,0044)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryAngleTolerance(OFString &value, const signed long pos = 0) const;

        /** get GantryAngleTolerance (300a,0044)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryAngleTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get PatientSupportAngleTolerance (300a,004c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportAngleTolerance(OFString &value, const signed long pos = 0) const;

        /** get PatientSupportAngleTolerance (300a,004c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportAngleTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get SnoutPositionTolerance (300a,004b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSnoutPositionTolerance(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopLateralPositionTolerance (300a,0053)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralPositionTolerance(OFString &value, const signed long pos = 0) const;

        /** get TableTopLateralPositionTolerance (300a,0053)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralPositionTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopLongitudinalPositionTolerance (300a,0052)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalPositionTolerance(OFString &value, const signed long pos = 0) const;

        /** get TableTopLongitudinalPositionTolerance (300a,0052)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalPositionTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopPitchAngleTolerance (300a,004f)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopPitchAngleTolerance(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopRollAngleTolerance (300a,0050)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopRollAngleTolerance(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopVerticalPositionTolerance (300a,0051)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalPositionTolerance(OFString &value, const signed long pos = 0) const;

        /** get TableTopVerticalPositionTolerance (300a,0051)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalPositionTolerance(Float64 &value, const unsigned long pos = 0) const;

        /** get ToleranceTableLabel (300a,0043)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getToleranceTableLabel(OFString &value, const signed long pos = 0) const;

        /** get ToleranceTableNumber (300a,0042)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getToleranceTableNumber(OFString &value, const signed long pos = 0) const;

        /** get ToleranceTableNumber (300a,0042)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getToleranceTableNumber(Sint32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get BeamLimitingDeviceToleranceSequence (300a,0048)
         *  @return reference to sequence element
         */
        DRTBeamLimitingDeviceToleranceSequence &getBeamLimitingDeviceToleranceSequence()
            { return BeamLimitingDeviceToleranceSequence; }

        /** get BeamLimitingDeviceToleranceSequence (300a,0048)
         *  @return const reference to sequence element
         */
        const DRTBeamLimitingDeviceToleranceSequence &getBeamLimitingDeviceToleranceSequence() const
            { return BeamLimitingDeviceToleranceSequence; }

      // --- set DICOM attribute values ---

        /** set BeamLimitingDeviceAngleTolerance (300a,0046)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamLimitingDeviceAngleTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set GantryAngleTolerance (300a,0044)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGantryAngleTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportAngleTolerance (300a,004c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportAngleTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set SnoutPositionTolerance (300a,004b)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSnoutPositionTolerance(const Float32 value, const unsigned long pos = 0);

        /** set TableTopLateralPositionTolerance (300a,0053)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLateralPositionTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopLongitudinalPositionTolerance (300a,0052)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLongitudinalPositionTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopPitchAngleTolerance (300a,004f)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopPitchAngleTolerance(const Float32 value, const unsigned long pos = 0);

        /** set TableTopRollAngleTolerance (300a,0050)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopRollAngleTolerance(const Float32 value, const unsigned long pos = 0);

        /** set TableTopVerticalPositionTolerance (300a,0051)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopVerticalPositionTolerance(const OFString &value, const OFBool check = OFTrue);

        /** set ToleranceTableLabel (300a,0043)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setToleranceTableLabel(const OFString &value, const OFBool check = OFTrue);

        /** set ToleranceTableNumber (300a,0042)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setToleranceTableNumber(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BeamLimitingDeviceAngleTolerance (300a,0046) vr=DS, vm=1, type=3
        DcmDecimalString BeamLimitingDeviceAngleTolerance;
        /// BeamLimitingDeviceToleranceSequence (300a,0048) vr=SQ, vm=1, type=3
        DRTBeamLimitingDeviceToleranceSequence BeamLimitingDeviceToleranceSequence;
        /// GantryAngleTolerance (300a,0044) vr=DS, vm=1, type=3
        DcmDecimalString GantryAngleTolerance;
        /// PatientSupportAngleTolerance (300a,004c) vr=DS, vm=1, type=3
        DcmDecimalString PatientSupportAngleTolerance;
        /// SnoutPositionTolerance (300a,004b) vr=FL, vm=1, type=3
        DcmFloatingPointSingle SnoutPositionTolerance;
        /// TableTopLateralPositionTolerance (300a,0053) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLateralPositionTolerance;
        /// TableTopLongitudinalPositionTolerance (300a,0052) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLongitudinalPositionTolerance;
        /// TableTopPitchAngleTolerance (300a,004f) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TableTopPitchAngleTolerance;
        /// TableTopRollAngleTolerance (300a,0050) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TableTopRollAngleTolerance;
        /// TableTopVerticalPositionTolerance (300a,0051) vr=DS, vm=1, type=3
        DcmDecimalString TableTopVerticalPositionTolerance;
        /// ToleranceTableLabel (300a,0043) vr=SH, vm=1, type=3
        DcmShortString ToleranceTableLabel;
        /// ToleranceTableNumber (300a,0042) vr=IS, vm=1, type=1
        DcmIntegerString ToleranceTableNumber;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIonToleranceTableSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIonToleranceTableSequence(const DRTIonToleranceTableSequence &copy);

    /** destructor
     */
    virtual ~DRTIonToleranceTableSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTIonToleranceTableSequence &operator=(const DRTIonToleranceTableSequence &copy);

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
