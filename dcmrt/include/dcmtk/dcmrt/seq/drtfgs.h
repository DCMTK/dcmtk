/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTFractionGroupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTFGS_H
#define DRTFGS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtrbs8.h"   // for ReferencedBeamSequence
#include "dcmtk/dcmrt/seq/drtrbas8.h"  // for ReferencedBrachyApplicationSetupSequence
#include "dcmtk/dcmrt/seq/drtrdrs8.h"  // for ReferencedDoseReferenceSequence
#include "dcmtk/dcmrt/seq/drtrds.h"    // for ReferencedDoseSequence


/** Interface class for FractionGroupSequence (300a,0070) in RTFractionSchemeModule
 */
class DCMTK_DCMRT_EXPORT DRTFractionGroupSequence
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

        /** get BeamDoseMeaning (300a,008b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamDoseMeaning(OFString &value, const signed long pos = 0) const;

        /** get FractionGroupDescription (300a,0072)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFractionGroupDescription(OFString &value, const signed long pos = 0) const;

        /** get FractionGroupNumber (300a,0071)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFractionGroupNumber(OFString &value, const signed long pos = 0) const;

        /** get FractionGroupNumber (300a,0071)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFractionGroupNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get FractionPattern (300a,007b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFractionPattern(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBeams (300a,0080)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBeams(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBeams (300a,0080)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBeams(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfBrachyApplicationSetups (300a,00a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBrachyApplicationSetups(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBrachyApplicationSetups (300a,00a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBrachyApplicationSetups(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfFractionPatternDigitsPerDay (300a,0079)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfFractionPatternDigitsPerDay(OFString &value, const signed long pos = 0) const;

        /** get NumberOfFractionPatternDigitsPerDay (300a,0079)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfFractionPatternDigitsPerDay(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfFractionsPlanned (300a,0078)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfFractionsPlanned(OFString &value, const signed long pos = 0) const;

        /** get NumberOfFractionsPlanned (300a,0078)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfFractionsPlanned(Sint32 &value, const unsigned long pos = 0) const;

        /** get RepeatFractionCycleLength (300a,007a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRepeatFractionCycleLength(OFString &value, const signed long pos = 0) const;

        /** get RepeatFractionCycleLength (300a,007a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRepeatFractionCycleLength(Sint32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ReferencedBeamSequence (300c,0004)
         *  @return reference to sequence element
         */
        DRTReferencedBeamSequenceInRTFractionSchemeModule &getReferencedBeamSequence()
            { return ReferencedBeamSequence; }

        /** get ReferencedBeamSequence (300c,0004)
         *  @return const reference to sequence element
         */
        const DRTReferencedBeamSequenceInRTFractionSchemeModule &getReferencedBeamSequence() const
            { return ReferencedBeamSequence; }

        /** get ReferencedBrachyApplicationSetupSequence (300c,000a)
         *  @return reference to sequence element
         */
        DRTReferencedBrachyApplicationSetupSequenceInRTFractionSchemeModule &getReferencedBrachyApplicationSetupSequence()
            { return ReferencedBrachyApplicationSetupSequence; }

        /** get ReferencedBrachyApplicationSetupSequence (300c,000a)
         *  @return const reference to sequence element
         */
        const DRTReferencedBrachyApplicationSetupSequenceInRTFractionSchemeModule &getReferencedBrachyApplicationSetupSequence() const
            { return ReferencedBrachyApplicationSetupSequence; }

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return reference to sequence element
         */
        DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &getReferencedDoseReferenceSequence()
            { return ReferencedDoseReferenceSequence; }

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return const reference to sequence element
         */
        const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &getReferencedDoseReferenceSequence() const
            { return ReferencedDoseReferenceSequence; }

        /** get ReferencedDoseSequence (300c,0080)
         *  @return reference to sequence element
         */
        DRTReferencedDoseSequence &getReferencedDoseSequence()
            { return ReferencedDoseSequence; }

        /** get ReferencedDoseSequence (300c,0080)
         *  @return const reference to sequence element
         */
        const DRTReferencedDoseSequence &getReferencedDoseSequence() const
            { return ReferencedDoseSequence; }

      // --- set DICOM attribute values ---

        /** set BeamDoseMeaning (300a,008b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamDoseMeaning(const OFString &value, const OFBool check = OFTrue);

        /** set FractionGroupDescription (300a,0072)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFractionGroupDescription(const OFString &value, const OFBool check = OFTrue);

        /** set FractionGroupNumber (300a,0071)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFractionGroupNumber(const OFString &value, const OFBool check = OFTrue);

        /** set FractionPattern (300a,007b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFractionPattern(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfBeams (300a,0080)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBeams(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfBrachyApplicationSetups (300a,00a0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBrachyApplicationSetups(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfFractionPatternDigitsPerDay (300a,0079)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfFractionPatternDigitsPerDay(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfFractionsPlanned (300a,0078)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfFractionsPlanned(const OFString &value, const OFBool check = OFTrue);

        /** set RepeatFractionCycleLength (300a,007a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRepeatFractionCycleLength(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BeamDoseMeaning (300a,008b) vr=CS, vm=1, type=3
        DcmCodeString BeamDoseMeaning;
        /// FractionGroupDescription (300a,0072) vr=LO, vm=1, type=3
        DcmLongString FractionGroupDescription;
        /// FractionGroupNumber (300a,0071) vr=IS, vm=1, type=1
        DcmIntegerString FractionGroupNumber;
        /// FractionPattern (300a,007b) vr=LT, vm=1, type=3
        DcmLongText FractionPattern;
        /// NumberOfBeams (300a,0080) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfBeams;
        /// NumberOfBrachyApplicationSetups (300a,00a0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfBrachyApplicationSetups;
        /// NumberOfFractionPatternDigitsPerDay (300a,0079) vr=IS, vm=1, type=3
        DcmIntegerString NumberOfFractionPatternDigitsPerDay;
        /// NumberOfFractionsPlanned (300a,0078) vr=IS, vm=1, type=2
        DcmIntegerString NumberOfFractionsPlanned;
        /// ReferencedBeamSequence (300c,0004) vr=SQ, vm=1, type=1C
        DRTReferencedBeamSequenceInRTFractionSchemeModule ReferencedBeamSequence;
        /// ReferencedBrachyApplicationSetupSequence (300c,000a) vr=SQ, vm=1, type=1C
        DRTReferencedBrachyApplicationSetupSequenceInRTFractionSchemeModule ReferencedBrachyApplicationSetupSequence;
        /// ReferencedDoseReferenceSequence (300c,0050) vr=SQ, vm=1, type=3
        DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule ReferencedDoseReferenceSequence;
        /// ReferencedDoseSequence (300c,0080) vr=SQ, vm=1, type=3
        DRTReferencedDoseSequence ReferencedDoseSequence;
        /// RepeatFractionCycleLength (300a,007a) vr=IS, vm=1, type=3
        DcmIntegerString RepeatFractionCycleLength;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTFractionGroupSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTFractionGroupSequence(const DRTFractionGroupSequence &copy);

    /** destructor
     */
    virtual ~DRTFractionGroupSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTFractionGroupSequence &operator=(const DRTFractionGroupSequence &copy);

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
