/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTContentItemModifierSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTCIMS_H
#define DRTCIMS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtccs.h"    // for ConceptCodeSequence
#include "dcmtk/dcmrt/seq/drtcncs.h"   // for ConceptNameCodeSequence
#include "dcmtk/dcmrt/seq/drtmucs.h"   // for MeasurementUnitsCodeSequence
#include "dcmtk/dcmrt/seq/drtrsos.h"   // for ReferencedSOPSequence


/** Interface class for ContentItemModifierSequence (0040,0441)
 */
class DCMTK_DCMRT_EXPORT DRTContentItemModifierSequence
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

        /** get Date (0040,a121)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDate(OFString &value, const signed long pos = 0) const;

        /** get DateTime (0040,a120)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDateTime(OFString &value, const signed long pos = 0) const;

        /** get FloatingPointValue (0040,a161)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFloatingPointValue(Float64 &value, const unsigned long pos = 0) const;

        /** get NumericValue (0040,a30a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumericValue(OFString &value, const signed long pos = 0) const;

        /** get NumericValue (0040,a30a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumericValue(Float64 &value, const unsigned long pos = 0) const;

        /** get NumericValue (0040,a30a)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumericValue(OFVector<Float64> &value) const;

        /** get PersonName (0040,a123)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPersonName(OFString &value, const signed long pos = 0) const;

        /** get RationalDenominatorValue (0040,a163)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRationalDenominatorValue(Uint32 &value, const unsigned long pos = 0) const;

        /** get RationalNumeratorValue (0040,a162)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRationalNumeratorValue(Sint32 &value, const unsigned long pos = 0) const;

        /** get TextValue (0040,a160)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTextValue(OFString &value, const signed long pos = 0) const;

        /** get Time (0040,a122)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTime(OFString &value, const signed long pos = 0) const;

        /** get UID (0040,a124)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getUID(OFString &value, const signed long pos = 0) const;

        /** get ValueType (0040,a040)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getValueType(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ConceptCodeSequence (0040,a168)
         *  @return reference to sequence element
         */
        DRTConceptCodeSequence &getConceptCodeSequence()
            { return ConceptCodeSequence; }

        /** get ConceptCodeSequence (0040,a168)
         *  @return const reference to sequence element
         */
        const DRTConceptCodeSequence &getConceptCodeSequence() const
            { return ConceptCodeSequence; }

        /** get ConceptNameCodeSequence (0040,a043)
         *  @return reference to sequence element
         */
        DRTConceptNameCodeSequence &getConceptNameCodeSequence()
            { return ConceptNameCodeSequence; }

        /** get ConceptNameCodeSequence (0040,a043)
         *  @return const reference to sequence element
         */
        const DRTConceptNameCodeSequence &getConceptNameCodeSequence() const
            { return ConceptNameCodeSequence; }

        /** get MeasurementUnitsCodeSequence (0040,08ea)
         *  @return reference to sequence element
         */
        DRTMeasurementUnitsCodeSequence &getMeasurementUnitsCodeSequence()
            { return MeasurementUnitsCodeSequence; }

        /** get MeasurementUnitsCodeSequence (0040,08ea)
         *  @return const reference to sequence element
         */
        const DRTMeasurementUnitsCodeSequence &getMeasurementUnitsCodeSequence() const
            { return MeasurementUnitsCodeSequence; }

        /** get ReferencedSOPSequence (0008,1199)
         *  @return reference to sequence element
         */
        DRTReferencedSOPSequence &getReferencedSOPSequence()
            { return ReferencedSOPSequence; }

        /** get ReferencedSOPSequence (0008,1199)
         *  @return const reference to sequence element
         */
        const DRTReferencedSOPSequence &getReferencedSOPSequence() const
            { return ReferencedSOPSequence; }

      // --- set DICOM attribute values ---

        /** set Date (0040,a121)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDate(const OFString &value, const OFBool check = OFTrue);

        /** set DateTime (0040,a120)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDateTime(const OFString &value, const OFBool check = OFTrue);

        /** set FloatingPointValue (0040,a161)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFloatingPointValue(const Float64 value, const unsigned long pos = 0);

        /** set NumericValue (0040,a30a)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumericValue(const OFString &value, const OFBool check = OFTrue);

        /** set PersonName (0040,a123)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPersonName(const OFString &value, const OFBool check = OFTrue);

        /** set RationalDenominatorValue (0040,a163)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRationalDenominatorValue(const Uint32 value, const unsigned long pos = 0);

        /** set RationalNumeratorValue (0040,a162)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRationalNumeratorValue(const Sint32 value, const unsigned long pos = 0);

        /** set TextValue (0040,a160)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTextValue(const OFString &value, const OFBool check = OFTrue);

        /** set Time (0040,a122)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTime(const OFString &value, const OFBool check = OFTrue);

        /** set UID (0040,a124)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setUID(const OFString &value, const OFBool check = OFTrue);

        /** set ValueType (0040,a040)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setValueType(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ConceptCodeSequence (0040,a168) vr=SQ, vm=1, type=1C
        DRTConceptCodeSequence ConceptCodeSequence;
        /// ConceptNameCodeSequence (0040,a043) vr=SQ, vm=1, type=1
        DRTConceptNameCodeSequence ConceptNameCodeSequence;
        /// Date (0040,a121) vr=DA, vm=1, type=1C
        DcmDate Date;
        /// DateTime (0040,a120) vr=DT, vm=1, type=1C
        DcmDateTime DateTime;
        /// FloatingPointValue (0040,a161) vr=FD, vm=1-n, type=1C
        DcmFloatingPointDouble FloatingPointValue;
        /// MeasurementUnitsCodeSequence (0040,08ea) vr=SQ, vm=1, type=1C
        DRTMeasurementUnitsCodeSequence MeasurementUnitsCodeSequence;
        /// NumericValue (0040,a30a) vr=DS, vm=1-n, type=1C
        DcmDecimalString NumericValue;
        /// PersonName (0040,a123) vr=PN, vm=1, type=1C
        DcmPersonName PersonName;
        /// RationalDenominatorValue (0040,a163) vr=UL, vm=1-n, type=1C
        DcmUnsignedLong RationalDenominatorValue;
        /// RationalNumeratorValue (0040,a162) vr=SL, vm=1-n, type=1C
        DcmSignedLong RationalNumeratorValue;
        /// ReferencedSOPSequence (0008,1199) vr=SQ, vm=1, type=1C
        DRTReferencedSOPSequence ReferencedSOPSequence;
        /// TextValue (0040,a160) vr=UT, vm=1, type=1C
        DcmUnlimitedText TextValue;
        /// Time (0040,a122) vr=TM, vm=1, type=1C
        DcmTime Time;
        /// UID (0040,a124) vr=UI, vm=1, type=1C
        DcmUniqueIdentifier UID;
        /// ValueType (0040,a040) vr=CS, vm=1, type=1
        DcmCodeString ValueType;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTContentItemModifierSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTContentItemModifierSequence(const DRTContentItemModifierSequence &copy);

    /** destructor
     */
    virtual ~DRTContentItemModifierSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTContentItemModifierSequence &operator=(const DRTContentItemModifierSequence &copy);

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
