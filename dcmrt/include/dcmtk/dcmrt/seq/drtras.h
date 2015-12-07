/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTRequestAttributesSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRAS_H
#define DRTRAS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtians.h"   // for IssuerOfAccessionNumberSequence
#include "dcmtk/dcmrt/seq/drtrrpcs.h"  // for ReasonForRequestedProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrss.h"    // for ReferencedStudySequence
#include "dcmtk/dcmrt/seq/drtrpcs.h"   // for RequestedProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtspcs.h"   // for ScheduledProtocolCodeSequence


/** Interface class for RequestAttributesSequence (0040,0275)
 */
class DCMTK_DCMRT_EXPORT DRTRequestAttributesSequence
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

        /** get AccessionNumber (0008,0050)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getAccessionNumber(OFString &value, const signed long pos = 0) const;

        /** get ReasonForTheRequestedProcedure (0040,1002)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReasonForTheRequestedProcedure(OFString &value, const signed long pos = 0) const;

        /** get RequestedProcedureDescription (0032,1060)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRequestedProcedureDescription(OFString &value, const signed long pos = 0) const;

        /** get RequestedProcedureID (0040,1001)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRequestedProcedureID(OFString &value, const signed long pos = 0) const;

        /** get ScheduledProcedureStepDescription (0040,0007)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScheduledProcedureStepDescription(OFString &value, const signed long pos = 0) const;

        /** get ScheduledProcedureStepID (0040,0009)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScheduledProcedureStepID(OFString &value, const signed long pos = 0) const;

        /** get StudyInstanceUID (0020,000d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getStudyInstanceUID(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get IssuerOfAccessionNumberSequence (0008,0051)
         *  @return reference to sequence element
         */
        DRTIssuerOfAccessionNumberSequence &getIssuerOfAccessionNumberSequence()
            { return IssuerOfAccessionNumberSequence; }

        /** get IssuerOfAccessionNumberSequence (0008,0051)
         *  @return const reference to sequence element
         */
        const DRTIssuerOfAccessionNumberSequence &getIssuerOfAccessionNumberSequence() const
            { return IssuerOfAccessionNumberSequence; }

        /** get ReasonForRequestedProcedureCodeSequence (0040,100a)
         *  @return reference to sequence element
         */
        DRTReasonForRequestedProcedureCodeSequence &getReasonForRequestedProcedureCodeSequence()
            { return ReasonForRequestedProcedureCodeSequence; }

        /** get ReasonForRequestedProcedureCodeSequence (0040,100a)
         *  @return const reference to sequence element
         */
        const DRTReasonForRequestedProcedureCodeSequence &getReasonForRequestedProcedureCodeSequence() const
            { return ReasonForRequestedProcedureCodeSequence; }

        /** get ReferencedStudySequence (0008,1110)
         *  @return reference to sequence element
         */
        DRTReferencedStudySequence &getReferencedStudySequence()
            { return ReferencedStudySequence; }

        /** get ReferencedStudySequence (0008,1110)
         *  @return const reference to sequence element
         */
        const DRTReferencedStudySequence &getReferencedStudySequence() const
            { return ReferencedStudySequence; }

        /** get RequestedProcedureCodeSequence (0032,1064)
         *  @return reference to sequence element
         */
        DRTRequestedProcedureCodeSequence &getRequestedProcedureCodeSequence()
            { return RequestedProcedureCodeSequence; }

        /** get RequestedProcedureCodeSequence (0032,1064)
         *  @return const reference to sequence element
         */
        const DRTRequestedProcedureCodeSequence &getRequestedProcedureCodeSequence() const
            { return RequestedProcedureCodeSequence; }

        /** get ScheduledProtocolCodeSequence (0040,0008)
         *  @return reference to sequence element
         */
        DRTScheduledProtocolCodeSequence &getScheduledProtocolCodeSequence()
            { return ScheduledProtocolCodeSequence; }

        /** get ScheduledProtocolCodeSequence (0040,0008)
         *  @return const reference to sequence element
         */
        const DRTScheduledProtocolCodeSequence &getScheduledProtocolCodeSequence() const
            { return ScheduledProtocolCodeSequence; }

      // --- set DICOM attribute values ---

        /** set AccessionNumber (0008,0050)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setAccessionNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ReasonForTheRequestedProcedure (0040,1002)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReasonForTheRequestedProcedure(const OFString &value, const OFBool check = OFTrue);

        /** set RequestedProcedureDescription (0032,1060)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRequestedProcedureDescription(const OFString &value, const OFBool check = OFTrue);

        /** set RequestedProcedureID (0040,1001)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRequestedProcedureID(const OFString &value, const OFBool check = OFTrue);

        /** set ScheduledProcedureStepDescription (0040,0007)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScheduledProcedureStepDescription(const OFString &value, const OFBool check = OFTrue);

        /** set ScheduledProcedureStepID (0040,0009)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScheduledProcedureStepID(const OFString &value, const OFBool check = OFTrue);

        /** set StudyInstanceUID (0020,000d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setStudyInstanceUID(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// AccessionNumber (0008,0050) vr=SH, vm=1, type=3
        DcmShortString AccessionNumber;
        /// IssuerOfAccessionNumberSequence (0008,0051) vr=SQ, vm=1, type=3
        DRTIssuerOfAccessionNumberSequence IssuerOfAccessionNumberSequence;
        /// ReasonForRequestedProcedureCodeSequence (0040,100a) vr=SQ, vm=1, type=3
        DRTReasonForRequestedProcedureCodeSequence ReasonForRequestedProcedureCodeSequence;
        /// ReasonForTheRequestedProcedure (0040,1002) vr=LO, vm=1, type=3
        DcmLongString ReasonForTheRequestedProcedure;
        /// ReferencedStudySequence (0008,1110) vr=SQ, vm=1, type=3
        DRTReferencedStudySequence ReferencedStudySequence;
        /// RequestedProcedureCodeSequence (0032,1064) vr=SQ, vm=1, type=3
        DRTRequestedProcedureCodeSequence RequestedProcedureCodeSequence;
        /// RequestedProcedureDescription (0032,1060) vr=LO, vm=1, type=3
        DcmLongString RequestedProcedureDescription;
        /// RequestedProcedureID (0040,1001) vr=SH, vm=1, type=1C
        DcmShortString RequestedProcedureID;
        /// ScheduledProcedureStepDescription (0040,0007) vr=LO, vm=1, type=3
        DcmLongString ScheduledProcedureStepDescription;
        /// ScheduledProcedureStepID (0040,0009) vr=SH, vm=1, type=1C
        DcmShortString ScheduledProcedureStepID;
        /// ScheduledProtocolCodeSequence (0040,0008) vr=SQ, vm=1, type=3
        DRTScheduledProtocolCodeSequence ScheduledProtocolCodeSequence;
        /// StudyInstanceUID (0020,000d) vr=UI, vm=1, type=3
        DcmUniqueIdentifier StudyInstanceUID;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTRequestAttributesSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTRequestAttributesSequence(const DRTRequestAttributesSequence &copy);

    /** destructor
     */
    virtual ~DRTRequestAttributesSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTRequestAttributesSequence &operator=(const DRTRequestAttributesSequence &copy);

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
