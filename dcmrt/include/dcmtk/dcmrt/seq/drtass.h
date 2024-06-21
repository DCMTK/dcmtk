/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTApplicationSetupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTASS_H
#define DRTASS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbads.h"   // for BrachyAccessoryDeviceSequence
#include "dcmtk/dcmrt/seq/drtchs.h"    // for ChannelSequence
#include "dcmtk/dcmrt/seq/drtrris3.h"  // for ReferencedReferenceImageSequence


/** Interface class for ApplicationSetupSequence (300a,0230) in RTBrachyApplicationSetupsModule
 */
class DCMTK_DCMRT_EXPORT DRTApplicationSetupSequence
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

        /** get ApplicationSetupManufacturer (300a,0238)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getApplicationSetupManufacturer(OFString &value, const signed long pos = 0) const;

        /** get ApplicationSetupName (300a,0236)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getApplicationSetupName(OFString &value, const signed long pos = 0) const;

        /** get ApplicationSetupNumber (300a,0234)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getApplicationSetupNumber(OFString &value, const signed long pos = 0) const;

        /** get ApplicationSetupNumber (300a,0234)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getApplicationSetupNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ApplicationSetupType (300a,0232)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getApplicationSetupType(OFString &value, const signed long pos = 0) const;

        /** get TemplateName (300a,0244)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTemplateName(OFString &value, const signed long pos = 0) const;

        /** get TemplateNumber (300a,0240)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTemplateNumber(OFString &value, const signed long pos = 0) const;

        /** get TemplateNumber (300a,0240)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTemplateNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get TemplateType (300a,0242)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTemplateType(OFString &value, const signed long pos = 0) const;

        /** get TotalReferenceAirKerma (300a,0250)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalReferenceAirKerma(OFString &value, const signed long pos = 0) const;

        /** get TotalReferenceAirKerma (300a,0250)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalReferenceAirKerma(Float64 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get BrachyAccessoryDeviceSequence (300a,0260)
         *  @return reference to sequence element
         */
        DRTBrachyAccessoryDeviceSequence &getBrachyAccessoryDeviceSequence()
            { return BrachyAccessoryDeviceSequence; }

        /** get BrachyAccessoryDeviceSequence (300a,0260)
         *  @return const reference to sequence element
         */
        const DRTBrachyAccessoryDeviceSequence &getBrachyAccessoryDeviceSequence() const
            { return BrachyAccessoryDeviceSequence; }

        /** get ChannelSequence (300a,0280)
         *  @return reference to sequence element
         */
        DRTChannelSequence &getChannelSequence()
            { return ChannelSequence; }

        /** get ChannelSequence (300a,0280)
         *  @return const reference to sequence element
         */
        const DRTChannelSequence &getChannelSequence() const
            { return ChannelSequence; }

        /** get ReferencedReferenceImageSequence (300c,0042)
         *  @return reference to sequence element
         */
        DRTReferencedReferenceImageSequenceInRTBrachyApplicationSetupsModule &getReferencedReferenceImageSequence()
            { return ReferencedReferenceImageSequence; }

        /** get ReferencedReferenceImageSequence (300c,0042)
         *  @return const reference to sequence element
         */
        const DRTReferencedReferenceImageSequenceInRTBrachyApplicationSetupsModule &getReferencedReferenceImageSequence() const
            { return ReferencedReferenceImageSequence; }

      // --- set DICOM attribute values ---

        /** set ApplicationSetupManufacturer (300a,0238)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setApplicationSetupManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set ApplicationSetupName (300a,0236)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setApplicationSetupName(const OFString &value, const OFBool check = OFTrue);

        /** set ApplicationSetupNumber (300a,0234)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setApplicationSetupNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ApplicationSetupType (300a,0232)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setApplicationSetupType(const OFString &value, const OFBool check = OFTrue);

        /** set TemplateName (300a,0244)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTemplateName(const OFString &value, const OFBool check = OFTrue);

        /** set TemplateNumber (300a,0240)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTemplateNumber(const OFString &value, const OFBool check = OFTrue);

        /** set TemplateType (300a,0242)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTemplateType(const OFString &value, const OFBool check = OFTrue);

        /** set TotalReferenceAirKerma (300a,0250)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalReferenceAirKerma(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ApplicationSetupManufacturer (300a,0238) vr=LO, vm=1, type=3
        DcmLongString ApplicationSetupManufacturer;
        /// ApplicationSetupName (300a,0236) vr=LO, vm=1, type=3
        DcmLongString ApplicationSetupName;
        /// ApplicationSetupNumber (300a,0234) vr=IS, vm=1, type=1
        DcmIntegerString ApplicationSetupNumber;
        /// ApplicationSetupType (300a,0232) vr=CS, vm=1, type=1
        DcmCodeString ApplicationSetupType;
        /// BrachyAccessoryDeviceSequence (300a,0260) vr=SQ, vm=1, type=3
        DRTBrachyAccessoryDeviceSequence BrachyAccessoryDeviceSequence;
        /// ChannelSequence (300a,0280) vr=SQ, vm=1, type=1
        DRTChannelSequence ChannelSequence;
        /// ReferencedReferenceImageSequence (300c,0042) vr=SQ, vm=1, type=3
        DRTReferencedReferenceImageSequenceInRTBrachyApplicationSetupsModule ReferencedReferenceImageSequence;
        /// TemplateName (300a,0244) vr=LO, vm=1, type=3
        DcmLongString TemplateName;
        /// TemplateNumber (300a,0240) vr=IS, vm=1, type=3
        DcmIntegerString TemplateNumber;
        /// TemplateType (300a,0242) vr=SH, vm=1, type=3
        DcmShortString TemplateType;
        /// TotalReferenceAirKerma (300a,0250) vr=DS, vm=1, type=1
        DcmDecimalString TotalReferenceAirKerma;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTApplicationSetupSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTApplicationSetupSequence(const DRTApplicationSetupSequence &copy);

    /** destructor
     */
    virtual ~DRTApplicationSetupSequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTApplicationSetupSequence &operator=(const DRTApplicationSetupSequence &copy);

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
