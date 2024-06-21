/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTPlanOverviewSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTPLOS_H
#define DRTPLOS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtpros.h"   // for PrescriptionOverviewSequence
#include "dcmtk/dcmrt/seq/drtrims.h"   // for ReferencedImageSequence
#include "dcmtk/dcmrt/seq/drtrsss.h"   // for ReferencedStructureSetSequence
#include "dcmtk/dcmrt/seq/drttscs.h"   // for TreatmentSiteCodeSequence


/** Interface class for PlanOverviewSequence (300c,0116)
 */
class DCMTK_DCMRT_EXPORT DRTPlanOverviewSequence
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

        /** get CurrentFractionNumber (3008,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCurrentFractionNumber(OFString &value, const signed long pos = 0) const;

        /** get CurrentFractionNumber (3008,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCurrentFractionNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfFractionsIncluded (300c,0119)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfFractionsIncluded(Uint16 &value, const unsigned long pos = 0) const;

        /** get PlanOverviewIndex (300c,0117)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPlanOverviewIndex(Uint16 &value, const unsigned long pos = 0) const;

        /** get RTPlanLabel (300a,0002)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTPlanLabel(OFString &value, const signed long pos = 0) const;

        /** get TreatmentSite (3010,0077)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentSite(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get PrescriptionOverviewSequence (300c,0114)
         *  @return reference to sequence element
         */
        DRTPrescriptionOverviewSequence &getPrescriptionOverviewSequence()
            { return PrescriptionOverviewSequence; }

        /** get PrescriptionOverviewSequence (300c,0114)
         *  @return const reference to sequence element
         */
        const DRTPrescriptionOverviewSequence &getPrescriptionOverviewSequence() const
            { return PrescriptionOverviewSequence; }

        /** get ReferencedImageSequence (0008,1140)
         *  @return reference to sequence element
         */
        DRTReferencedImageSequence &getReferencedImageSequence()
            { return ReferencedImageSequence; }

        /** get ReferencedImageSequence (0008,1140)
         *  @return const reference to sequence element
         */
        const DRTReferencedImageSequence &getReferencedImageSequence() const
            { return ReferencedImageSequence; }

        /** get ReferencedStructureSetSequence (300c,0060)
         *  @return reference to sequence element
         */
        DRTReferencedStructureSetSequence &getReferencedStructureSetSequence()
            { return ReferencedStructureSetSequence; }

        /** get ReferencedStructureSetSequence (300c,0060)
         *  @return const reference to sequence element
         */
        const DRTReferencedStructureSetSequence &getReferencedStructureSetSequence() const
            { return ReferencedStructureSetSequence; }

        /** get TreatmentSiteCodeSequence (3010,0078)
         *  @return reference to sequence element
         */
        DRTTreatmentSiteCodeSequence &getTreatmentSiteCodeSequence()
            { return TreatmentSiteCodeSequence; }

        /** get TreatmentSiteCodeSequence (3010,0078)
         *  @return const reference to sequence element
         */
        const DRTTreatmentSiteCodeSequence &getTreatmentSiteCodeSequence() const
            { return TreatmentSiteCodeSequence; }

      // --- set DICOM attribute values ---

        /** set CurrentFractionNumber (3008,0022)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCurrentFractionNumber(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfFractionsIncluded (300c,0119)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfFractionsIncluded(const Uint16 value, const unsigned long pos = 0);

        /** set PlanOverviewIndex (300c,0117)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPlanOverviewIndex(const Uint16 value, const unsigned long pos = 0);

        /** set RTPlanLabel (300a,0002)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTPlanLabel(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentSite (3010,0077)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentSite(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// CurrentFractionNumber (3008,0022) vr=IS, vm=1, type=1C
        DcmIntegerString CurrentFractionNumber;
        /// NumberOfFractionsIncluded (300c,0119) vr=US, vm=1, type=1C
        DcmUnsignedShort NumberOfFractionsIncluded;
        /// PlanOverviewIndex (300c,0117) vr=US, vm=1, type=1
        DcmUnsignedShort PlanOverviewIndex;
        /// PrescriptionOverviewSequence (300c,0114) vr=SQ, vm=1, type=2
        DRTPrescriptionOverviewSequence PrescriptionOverviewSequence;
        /// RTPlanLabel (300a,0002) vr=SH, vm=1, type=2
        DcmShortString RTPlanLabel;
        /// ReferencedImageSequence (0008,1140) vr=SQ, vm=1, type=1C
        DRTReferencedImageSequence ReferencedImageSequence;
        /// ReferencedStructureSetSequence (300c,0060) vr=SQ, vm=1, type=1C
        DRTReferencedStructureSetSequence ReferencedStructureSetSequence;
        /// TreatmentSite (3010,0077) vr=LO, vm=1, type=2
        DcmLongString TreatmentSite;
        /// TreatmentSiteCodeSequence (3010,0078) vr=SQ, vm=1, type=2
        DRTTreatmentSiteCodeSequence TreatmentSiteCodeSequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTPlanOverviewSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTPlanOverviewSequence(const DRTPlanOverviewSequence &copy);

    /** destructor
     */
    virtual ~DRTPlanOverviewSequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTPlanOverviewSequence &operator=(const DRTPlanOverviewSequence &copy);

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
