/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTDoseCalibrationConditionsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTDCCS_H
#define DRTDCCS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for DoseCalibrationConditionsSequence (300c,0120)
 */
class DCMTK_DCMRT_EXPORT DRTDoseCalibrationConditionsSequence
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

        /** get AbsorbedDoseToMetersetRatio (300c,0121)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getAbsorbedDoseToMetersetRatio(Float64 &value, const unsigned long pos = 0) const;

        /** get CalibrationDateTime (0018,1203)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCalibrationDateTime(OFString &value, const signed long pos = 0) const;

        /** get CalibrationReferencePointDepth (300c,0124)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCalibrationReferencePointDepth(Float64 &value, const unsigned long pos = 0) const;

        /** get DelineatedRadiationFieldSize (300c,0122)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDelineatedRadiationFieldSize(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceToSurfaceDistance (300a,0130)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToSurfaceDistance(OFString &value, const signed long pos = 0) const;

        /** get SourceToSurfaceDistance (300a,0130)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToSurfaceDistance(Float64 &value, const unsigned long pos = 0) const;

      // --- set DICOM attribute values ---

        /** set AbsorbedDoseToMetersetRatio (300c,0121)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setAbsorbedDoseToMetersetRatio(const Float64 value, const unsigned long pos = 0);

        /** set CalibrationDateTime (0018,1203)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCalibrationDateTime(const OFString &value, const OFBool check = OFTrue);

        /** set CalibrationReferencePointDepth (300c,0124)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCalibrationReferencePointDepth(const Float64 value, const unsigned long pos = 0);

        /** set DelineatedRadiationFieldSize (300c,0122)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=2
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDelineatedRadiationFieldSize(const Float64 value, const unsigned long pos = 0);

        /** set SourceToSurfaceDistance (300a,0130)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceToSurfaceDistance(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// AbsorbedDoseToMetersetRatio (300c,0121) vr=FD, vm=1, type=1
        DcmFloatingPointDouble AbsorbedDoseToMetersetRatio;
        /// CalibrationDateTime (0018,1203) vr=DT, vm=1, type=2
        DcmDateTime CalibrationDateTime;
        /// CalibrationReferencePointDepth (300c,0124) vr=FD, vm=1, type=1
        DcmFloatingPointDouble CalibrationReferencePointDepth;
        /// DelineatedRadiationFieldSize (300c,0122) vr=FD, vm=2, type=1
        DcmFloatingPointDouble DelineatedRadiationFieldSize;
        /// SourceToSurfaceDistance (300a,0130) vr=DS, vm=1, type=1
        DcmDecimalString SourceToSurfaceDistance;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTDoseCalibrationConditionsSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTDoseCalibrationConditionsSequence(const DRTDoseCalibrationConditionsSequence &copy);

    /** destructor
     */
    virtual ~DRTDoseCalibrationConditionsSequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTDoseCalibrationConditionsSequence &operator=(const DRTDoseCalibrationConditionsSequence &copy);

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
