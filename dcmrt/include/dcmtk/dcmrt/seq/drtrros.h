/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTRTROIObservationsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRROS_H
#define DRTRROS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtarics.h"  // for AdditionalRTROIIdentificationCodeSequence
#include "dcmtk/dcmrt/seq/drtrpps.h"   // for ROIPhysicalPropertiesSequence
#include "dcmtk/dcmrt/seq/drtrics.h"   // for RTROIIdentificationCodeSequence
#include "dcmtk/dcmrt/seq/drtrrs.h"    // for RTRelatedROISequence
#include "dcmtk/dcmrt/seq/drtrrros.h"  // for RelatedRTROIObservationsSequence
#include "dcmtk/dcmrt/seq/drtspccs.h"  // for SegmentedPropertyCategoryCodeSequence


/** Interface class for RTROIObservationsSequence (3006,0080)
 */
class DCMTK_DCMRT_EXPORT DRTRTROIObservationsSequence
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

        /** get MaterialID (300a,00e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMaterialID(OFString &value, const signed long pos = 0) const;

        /** get ObservationNumber (3006,0082)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getObservationNumber(OFString &value, const signed long pos = 0) const;

        /** get ObservationNumber (3006,0082)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getObservationNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ROIInterpreter (3006,00a6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIInterpreter(OFString &value, const signed long pos = 0) const;

        /** get ROIObservationDescription (3006,0088)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIObservationDescription(OFString &value, const signed long pos = 0) const;

        /** get ROIObservationLabel (3006,0085)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIObservationLabel(OFString &value, const signed long pos = 0) const;

        /** get RTROIInterpretedType (3006,00a4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTROIInterpretedType(OFString &value, const signed long pos = 0) const;

        /** get ReferencedROINumber (3006,0084)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedROINumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedROINumber (3006,0084)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedROINumber(Sint32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get AdditionalRTROIIdentificationCodeSequence (3006,00b9)
         *  @return reference to sequence element
         */
        DRTAdditionalRTROIIdentificationCodeSequence &getAdditionalRTROIIdentificationCodeSequence()
            { return AdditionalRTROIIdentificationCodeSequence; }

        /** get AdditionalRTROIIdentificationCodeSequence (3006,00b9)
         *  @return const reference to sequence element
         */
        const DRTAdditionalRTROIIdentificationCodeSequence &getAdditionalRTROIIdentificationCodeSequence() const
            { return AdditionalRTROIIdentificationCodeSequence; }

        /** get ROIPhysicalPropertiesSequence (3006,00b0)
         *  @return reference to sequence element
         */
        DRTROIPhysicalPropertiesSequence &getROIPhysicalPropertiesSequence()
            { return ROIPhysicalPropertiesSequence; }

        /** get ROIPhysicalPropertiesSequence (3006,00b0)
         *  @return const reference to sequence element
         */
        const DRTROIPhysicalPropertiesSequence &getROIPhysicalPropertiesSequence() const
            { return ROIPhysicalPropertiesSequence; }

        /** get RTROIIdentificationCodeSequence (3006,0086)
         *  @return reference to sequence element
         */
        DRTRTROIIdentificationCodeSequence &getRTROIIdentificationCodeSequence()
            { return RTROIIdentificationCodeSequence; }

        /** get RTROIIdentificationCodeSequence (3006,0086)
         *  @return const reference to sequence element
         */
        const DRTRTROIIdentificationCodeSequence &getRTROIIdentificationCodeSequence() const
            { return RTROIIdentificationCodeSequence; }

        /** get RTRelatedROISequence (3006,0030)
         *  @return reference to sequence element
         */
        DRTRTRelatedROISequence &getRTRelatedROISequence()
            { return RTRelatedROISequence; }

        /** get RTRelatedROISequence (3006,0030)
         *  @return const reference to sequence element
         */
        const DRTRTRelatedROISequence &getRTRelatedROISequence() const
            { return RTRelatedROISequence; }

        /** get RelatedRTROIObservationsSequence (3006,00a0)
         *  @return reference to sequence element
         */
        DRTRelatedRTROIObservationsSequence &getRelatedRTROIObservationsSequence()
            { return RelatedRTROIObservationsSequence; }

        /** get RelatedRTROIObservationsSequence (3006,00a0)
         *  @return const reference to sequence element
         */
        const DRTRelatedRTROIObservationsSequence &getRelatedRTROIObservationsSequence() const
            { return RelatedRTROIObservationsSequence; }

        /** get SegmentedPropertyCategoryCodeSequence (0062,0003)
         *  @return reference to sequence element
         */
        DRTSegmentedPropertyCategoryCodeSequence &getSegmentedPropertyCategoryCodeSequence()
            { return SegmentedPropertyCategoryCodeSequence; }

        /** get SegmentedPropertyCategoryCodeSequence (0062,0003)
         *  @return const reference to sequence element
         */
        const DRTSegmentedPropertyCategoryCodeSequence &getSegmentedPropertyCategoryCodeSequence() const
            { return SegmentedPropertyCategoryCodeSequence; }

      // --- set DICOM attribute values ---

        /** set MaterialID (300a,00e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMaterialID(const OFString &value, const OFBool check = OFTrue);

        /** set ObservationNumber (3006,0082)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setObservationNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ROIInterpreter (3006,00a6)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIInterpreter(const OFString &value, const OFBool check = OFTrue);

        /** set ROIObservationDescription (3006,0088)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIObservationDescription(const OFString &value, const OFBool check = OFTrue);

        /** set ROIObservationLabel (3006,0085)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIObservationLabel(const OFString &value, const OFBool check = OFTrue);

        /** set RTROIInterpretedType (3006,00a4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTROIInterpretedType(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedROINumber (3006,0084)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedROINumber(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// AdditionalRTROIIdentificationCodeSequence (3006,00b9) vr=SQ, vm=1, type=3
        DRTAdditionalRTROIIdentificationCodeSequence AdditionalRTROIIdentificationCodeSequence;
        /// MaterialID (300a,00e1) vr=SH, vm=1, type=3
        DcmShortString MaterialID;
        /// ObservationNumber (3006,0082) vr=IS, vm=1, type=1
        DcmIntegerString ObservationNumber;
        /// ROIInterpreter (3006,00a6) vr=PN, vm=1, type=2
        DcmPersonName ROIInterpreter;
        /// ROIObservationDescription (3006,0088) vr=ST, vm=1, type=3
        DcmShortText ROIObservationDescription;
        /// ROIObservationLabel (3006,0085) vr=SH, vm=1, type=3
        DcmShortString ROIObservationLabel;
        /// ROIPhysicalPropertiesSequence (3006,00b0) vr=SQ, vm=1, type=3
        DRTROIPhysicalPropertiesSequence ROIPhysicalPropertiesSequence;
        /// RTROIIdentificationCodeSequence (3006,0086) vr=SQ, vm=1, type=3
        DRTRTROIIdentificationCodeSequence RTROIIdentificationCodeSequence;
        /// RTROIInterpretedType (3006,00a4) vr=CS, vm=1, type=2
        DcmCodeString RTROIInterpretedType;
        /// RTRelatedROISequence (3006,0030) vr=SQ, vm=1, type=3
        DRTRTRelatedROISequence RTRelatedROISequence;
        /// ReferencedROINumber (3006,0084) vr=IS, vm=1, type=1
        DcmIntegerString ReferencedROINumber;
        /// RelatedRTROIObservationsSequence (3006,00a0) vr=SQ, vm=1, type=3
        DRTRelatedRTROIObservationsSequence RelatedRTROIObservationsSequence;
        /// SegmentedPropertyCategoryCodeSequence (0062,0003) vr=SQ, vm=1, type=3
        DRTSegmentedPropertyCategoryCodeSequence SegmentedPropertyCategoryCodeSequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTRTROIObservationsSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTRTROIObservationsSequence(const DRTRTROIObservationsSequence &copy);

    /** destructor
     */
    virtual ~DRTRTROIObservationsSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTRTROIObservationsSequence &operator=(const DRTRTROIObservationsSequence &copy);

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
