/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTRTROIIdentificationCodeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRICS_H
#define DRTRICS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtecs.h"    // for EquivalentCodeSequence


/** Interface class for RTROIIdentificationCodeSequence (3006,0086)
 */
class DCMTK_DCMRT_EXPORT DRTRTROIIdentificationCodeSequence
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

        /** get CodeMeaning (0008,0104)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCodeMeaning(OFString &value, const signed long pos = 0) const;

        /** get CodeValue (0008,0100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCodeValue(OFString &value, const signed long pos = 0) const;

        /** get CodingSchemeDesignator (0008,0102)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCodingSchemeDesignator(OFString &value, const signed long pos = 0) const;

        /** get CodingSchemeVersion (0008,0103)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCodingSchemeVersion(OFString &value, const signed long pos = 0) const;

        /** get ContextGroupExtensionCreatorUID (0008,010d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextGroupExtensionCreatorUID(OFString &value, const signed long pos = 0) const;

        /** get ContextGroupExtensionFlag (0008,010b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextGroupExtensionFlag(OFString &value, const signed long pos = 0) const;

        /** get ContextGroupLocalVersion (0008,0107)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextGroupLocalVersion(OFString &value, const signed long pos = 0) const;

        /** get ContextGroupVersion (0008,0106)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextGroupVersion(OFString &value, const signed long pos = 0) const;

        /** get ContextIdentifier (0008,010f)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextIdentifier(OFString &value, const signed long pos = 0) const;

        /** get ContextUID (0008,0117)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getContextUID(OFString &value, const signed long pos = 0) const;

        /** get LongCodeValue (0008,0119)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getLongCodeValue(OFString &value, const signed long pos = 0) const;

        /** get MappingResource (0008,0105)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMappingResource(OFString &value, const signed long pos = 0) const;

        /** get MappingResourceUID (0008,0118)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMappingResourceUID(OFString &value, const signed long pos = 0) const;

        /** get URNCodeValue (0008,0120)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getURNCodeValue(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get EquivalentCodeSequence (0008,0121)
         *  @return reference to sequence element
         */
        DRTEquivalentCodeSequence &getEquivalentCodeSequence()
            { return EquivalentCodeSequence; }

        /** get EquivalentCodeSequence (0008,0121)
         *  @return const reference to sequence element
         */
        const DRTEquivalentCodeSequence &getEquivalentCodeSequence() const
            { return EquivalentCodeSequence; }

      // --- set DICOM attribute values ---

        /** set CodeMeaning (0008,0104)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCodeMeaning(const OFString &value, const OFBool check = OFTrue);

        /** set CodeValue (0008,0100)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCodeValue(const OFString &value, const OFBool check = OFTrue);

        /** set CodingSchemeDesignator (0008,0102)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCodingSchemeDesignator(const OFString &value, const OFBool check = OFTrue);

        /** set CodingSchemeVersion (0008,0103)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCodingSchemeVersion(const OFString &value, const OFBool check = OFTrue);

        /** set ContextGroupExtensionCreatorUID (0008,010d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextGroupExtensionCreatorUID(const OFString &value, const OFBool check = OFTrue);

        /** set ContextGroupExtensionFlag (0008,010b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextGroupExtensionFlag(const OFString &value, const OFBool check = OFTrue);

        /** set ContextGroupLocalVersion (0008,0107)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextGroupLocalVersion(const OFString &value, const OFBool check = OFTrue);

        /** set ContextGroupVersion (0008,0106)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextGroupVersion(const OFString &value, const OFBool check = OFTrue);

        /** set ContextIdentifier (0008,010f)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextIdentifier(const OFString &value, const OFBool check = OFTrue);

        /** set ContextUID (0008,0117)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setContextUID(const OFString &value, const OFBool check = OFTrue);

        /** set LongCodeValue (0008,0119)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UC) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setLongCodeValue(const OFString &value, const OFBool check = OFTrue);

        /** set MappingResource (0008,0105)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMappingResource(const OFString &value, const OFBool check = OFTrue);

        /** set MappingResourceUID (0008,0118)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMappingResourceUID(const OFString &value, const OFBool check = OFTrue);

        /** set URNCodeValue (0008,0120)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UR) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setURNCodeValue(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// CodeMeaning (0008,0104) vr=LO, vm=1, type=1
        DcmLongString CodeMeaning;
        /// CodeValue (0008,0100) vr=SH, vm=1, type=1C
        DcmShortString CodeValue;
        /// CodingSchemeDesignator (0008,0102) vr=SH, vm=1, type=1C
        DcmShortString CodingSchemeDesignator;
        /// CodingSchemeVersion (0008,0103) vr=SH, vm=1, type=1C
        DcmShortString CodingSchemeVersion;
        /// ContextGroupExtensionCreatorUID (0008,010d) vr=UI, vm=1, type=1C
        DcmUniqueIdentifier ContextGroupExtensionCreatorUID;
        /// ContextGroupExtensionFlag (0008,010b) vr=CS, vm=1, type=3
        DcmCodeString ContextGroupExtensionFlag;
        /// ContextGroupLocalVersion (0008,0107) vr=DT, vm=1, type=1C
        DcmDateTime ContextGroupLocalVersion;
        /// ContextGroupVersion (0008,0106) vr=DT, vm=1, type=1C
        DcmDateTime ContextGroupVersion;
        /// ContextIdentifier (0008,010f) vr=CS, vm=1, type=3
        DcmCodeString ContextIdentifier;
        /// ContextUID (0008,0117) vr=UI, vm=1, type=3
        DcmUniqueIdentifier ContextUID;
        /// EquivalentCodeSequence (0008,0121) vr=SQ, vm=1, type=3
        DRTEquivalentCodeSequence EquivalentCodeSequence;
        /// LongCodeValue (0008,0119) vr=UC, vm=1, type=1C
        DcmUnlimitedCharacters LongCodeValue;
        /// MappingResource (0008,0105) vr=CS, vm=1, type=1C
        DcmCodeString MappingResource;
        /// MappingResourceUID (0008,0118) vr=UI, vm=1, type=3
        DcmUniqueIdentifier MappingResourceUID;
        /// URNCodeValue (0008,0120) vr=UR, vm=1, type=1C
        DcmUniversalResourceIdentifierOrLocator URNCodeValue;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTRTROIIdentificationCodeSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTRTROIIdentificationCodeSequence(const DRTRTROIIdentificationCodeSequence &copy);

    /** destructor
     */
    virtual ~DRTRTROIIdentificationCodeSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTRTROIIdentificationCodeSequence &operator=(const DRTRTROIIdentificationCodeSequence &copy);

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
