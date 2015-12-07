/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTCompensatorSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTCOS_H
#define DRTCOS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for CompensatorSequence (300a,00e3)
 */
class DCMTK_DCMRT_EXPORT DRTCompensatorSequence
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

        /** get AccessoryCode (300a,00f9)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getAccessoryCode(OFString &value, const signed long pos = 0) const;

        /** get CompensatorColumns (300a,00e8)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorColumns(OFString &value, const signed long pos = 0) const;

        /** get CompensatorColumns (300a,00e8)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorColumns(Sint32 &value, const unsigned long pos = 0) const;

        /** get CompensatorDescription (300a,02eb)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorDescription(OFString &value, const signed long pos = 0) const;

        /** get CompensatorDivergence (300a,02e0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorDivergence(OFString &value, const signed long pos = 0) const;

        /** get CompensatorID (300a,00e5)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorID(OFString &value, const signed long pos = 0) const;

        /** get CompensatorMountingPosition (300a,02e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorMountingPosition(OFString &value, const signed long pos = 0) const;

        /** get CompensatorNumber (300a,00e4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorNumber(OFString &value, const signed long pos = 0) const;

        /** get CompensatorNumber (300a,00e4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get CompensatorPixelSpacing (300a,00e9)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPixelSpacing(OFString &value, const signed long pos = 0) const;

        /** get CompensatorPixelSpacing (300a,00e9)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPixelSpacing(Float64 &value, const unsigned long pos = 0) const;

        /** get CompensatorPixelSpacing (300a,00e9)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPixelSpacing(OFVector<Float64> &value) const;

        /** get CompensatorPosition (300a,00ea)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPosition(OFString &value, const signed long pos = 0) const;

        /** get CompensatorPosition (300a,00ea)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get CompensatorPosition (300a,00ea)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorPosition(OFVector<Float64> &value) const;

        /** get CompensatorRows (300a,00e7)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorRows(OFString &value, const signed long pos = 0) const;

        /** get CompensatorRows (300a,00e7)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorRows(Sint32 &value, const unsigned long pos = 0) const;

        /** get CompensatorThicknessData (300a,00ec)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorThicknessData(OFString &value, const signed long pos = 0) const;

        /** get CompensatorThicknessData (300a,00ec)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorThicknessData(Float64 &value, const unsigned long pos = 0) const;

        /** get CompensatorThicknessData (300a,00ec)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorThicknessData(OFVector<Float64> &value) const;

        /** get CompensatorTransmissionData (300a,00eb)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorTransmissionData(OFString &value, const signed long pos = 0) const;

        /** get CompensatorTransmissionData (300a,00eb)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorTransmissionData(Float64 &value, const unsigned long pos = 0) const;

        /** get CompensatorTransmissionData (300a,00eb)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorTransmissionData(OFVector<Float64> &value) const;

        /** get CompensatorTrayID (300a,00ef)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorTrayID(OFString &value, const signed long pos = 0) const;

        /** get CompensatorType (300a,00ee)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCompensatorType(OFString &value, const signed long pos = 0) const;

        /** get MaterialID (300a,00e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMaterialID(OFString &value, const signed long pos = 0) const;

        /** get SourceToCompensatorDistance (300a,02e2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToCompensatorDistance(OFString &value, const signed long pos = 0) const;

        /** get SourceToCompensatorDistance (300a,02e2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToCompensatorDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceToCompensatorDistance (300a,02e2)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToCompensatorDistance(OFVector<Float64> &value) const;

        /** get SourceToCompensatorTrayDistance (300a,00e6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToCompensatorTrayDistance(OFString &value, const signed long pos = 0) const;

        /** get SourceToCompensatorTrayDistance (300a,00e6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToCompensatorTrayDistance(Float64 &value, const unsigned long pos = 0) const;

      // --- set DICOM attribute values ---

        /** set AccessoryCode (300a,00f9)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setAccessoryCode(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorColumns (300a,00e8)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorColumns(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorDescription (300a,02eb)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorDescription(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorDivergence (300a,02e0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorDivergence(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorID (300a,00e5)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorID(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorMountingPosition (300a,02e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorMountingPosition(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorNumber (300a,00e4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorNumber(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorPixelSpacing (300a,00e9)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorPixelSpacing(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorPosition (300a,00ea)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorPosition(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorRows (300a,00e7)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorRows(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorThicknessData (300a,00ec)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorThicknessData(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorTransmissionData (300a,00eb)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorTransmissionData(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorTrayID (300a,00ef)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorTrayID(const OFString &value, const OFBool check = OFTrue);

        /** set CompensatorType (300a,00ee)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCompensatorType(const OFString &value, const OFBool check = OFTrue);

        /** set MaterialID (300a,00e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMaterialID(const OFString &value, const OFBool check = OFTrue);

        /** set SourceToCompensatorDistance (300a,02e2)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceToCompensatorDistance(const OFString &value, const OFBool check = OFTrue);

        /** set SourceToCompensatorTrayDistance (300a,00e6)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceToCompensatorTrayDistance(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// AccessoryCode (300a,00f9) vr=LO, vm=1, type=3
        DcmLongString AccessoryCode;
        /// CompensatorColumns (300a,00e8) vr=IS, vm=1, type=1
        DcmIntegerString CompensatorColumns;
        /// CompensatorDescription (300a,02eb) vr=LT, vm=1, type=3
        DcmLongText CompensatorDescription;
        /// CompensatorDivergence (300a,02e0) vr=CS, vm=1, type=3
        DcmCodeString CompensatorDivergence;
        /// CompensatorID (300a,00e5) vr=SH, vm=1, type=3
        DcmShortString CompensatorID;
        /// CompensatorMountingPosition (300a,02e1) vr=CS, vm=1, type=3
        DcmCodeString CompensatorMountingPosition;
        /// CompensatorNumber (300a,00e4) vr=IS, vm=1, type=1C
        DcmIntegerString CompensatorNumber;
        /// CompensatorPixelSpacing (300a,00e9) vr=DS, vm=2, type=1
        DcmDecimalString CompensatorPixelSpacing;
        /// CompensatorPosition (300a,00ea) vr=DS, vm=2, type=1
        DcmDecimalString CompensatorPosition;
        /// CompensatorRows (300a,00e7) vr=IS, vm=1, type=1
        DcmIntegerString CompensatorRows;
        /// CompensatorThicknessData (300a,00ec) vr=DS, vm=1-n, type=1C
        DcmDecimalString CompensatorThicknessData;
        /// CompensatorTransmissionData (300a,00eb) vr=DS, vm=1-n, type=1C
        DcmDecimalString CompensatorTransmissionData;
        /// CompensatorTrayID (300a,00ef) vr=SH, vm=1, type=3
        DcmShortString CompensatorTrayID;
        /// CompensatorType (300a,00ee) vr=CS, vm=1, type=3
        DcmCodeString CompensatorType;
        /// MaterialID (300a,00e1) vr=SH, vm=1, type=2C
        DcmShortString MaterialID;
        /// SourceToCompensatorDistance (300a,02e2) vr=DS, vm=1-n, type=1C
        DcmDecimalString SourceToCompensatorDistance;
        /// SourceToCompensatorTrayDistance (300a,00e6) vr=DS, vm=1, type=2
        DcmDecimalString SourceToCompensatorTrayDistance;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTCompensatorSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTCompensatorSequence(const DRTCompensatorSequence &copy);

    /** destructor
     */
    virtual ~DRTCompensatorSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTCompensatorSequence &operator=(const DRTCompensatorSequence &copy);

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
