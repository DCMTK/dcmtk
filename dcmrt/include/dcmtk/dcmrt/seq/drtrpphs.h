/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTReferencedPatientPhotoSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRPPHS_H
#define DRTRPPHS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtdimrs.h"  // for DICOMMediaRetrievalSequence
#include "dcmtk/dcmrt/seq/drtdirs.h"   // for DICOMRetrievalSequence
#include "dcmtk/dcmrt/seq/drtrsos.h"   // for ReferencedSOPSequence
#include "dcmtk/dcmrt/seq/drtwrsrs.h"  // for WADORSRetrievalSequence
#include "dcmtk/dcmrt/seq/drtwrs.h"    // for WADORetrievalSequence
#include "dcmtk/dcmrt/seq/drtxrs.h"    // for XDSRetrievalSequence


/** Interface class for ReferencedPatientPhotoSequence (0010,1100)
 */
class DCMTK_DCMRT_EXPORT DRTReferencedPatientPhotoSequence
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

        /** get SeriesInstanceUID (0020,000e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSeriesInstanceUID(OFString &value, const signed long pos = 0) const;

        /** get StudyInstanceUID (0020,000d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getStudyInstanceUID(OFString &value, const signed long pos = 0) const;

        /** get TypeOfInstances (0040,e020)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTypeOfInstances(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get DICOMMediaRetrievalSequence (0040,e022)
         *  @return reference to sequence element
         */
        DRTDICOMMediaRetrievalSequence &getDICOMMediaRetrievalSequence()
            { return DICOMMediaRetrievalSequence; }

        /** get DICOMMediaRetrievalSequence (0040,e022)
         *  @return const reference to sequence element
         */
        const DRTDICOMMediaRetrievalSequence &getDICOMMediaRetrievalSequence() const
            { return DICOMMediaRetrievalSequence; }

        /** get DICOMRetrievalSequence (0040,e021)
         *  @return reference to sequence element
         */
        DRTDICOMRetrievalSequence &getDICOMRetrievalSequence()
            { return DICOMRetrievalSequence; }

        /** get DICOMRetrievalSequence (0040,e021)
         *  @return const reference to sequence element
         */
        const DRTDICOMRetrievalSequence &getDICOMRetrievalSequence() const
            { return DICOMRetrievalSequence; }

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

        /** get WADORSRetrievalSequence (0040,e025)
         *  @return reference to sequence element
         */
        DRTWADORSRetrievalSequence &getWADORSRetrievalSequence()
            { return WADORSRetrievalSequence; }

        /** get WADORSRetrievalSequence (0040,e025)
         *  @return const reference to sequence element
         */
        const DRTWADORSRetrievalSequence &getWADORSRetrievalSequence() const
            { return WADORSRetrievalSequence; }

        /** get WADORetrievalSequence (0040,e023)
         *  @return reference to sequence element
         */
        DRTWADORetrievalSequence &getWADORetrievalSequence()
            { return WADORetrievalSequence; }

        /** get WADORetrievalSequence (0040,e023)
         *  @return const reference to sequence element
         */
        const DRTWADORetrievalSequence &getWADORetrievalSequence() const
            { return WADORetrievalSequence; }

        /** get XDSRetrievalSequence (0040,e024)
         *  @return reference to sequence element
         */
        DRTXDSRetrievalSequence &getXDSRetrievalSequence()
            { return XDSRetrievalSequence; }

        /** get XDSRetrievalSequence (0040,e024)
         *  @return const reference to sequence element
         */
        const DRTXDSRetrievalSequence &getXDSRetrievalSequence() const
            { return XDSRetrievalSequence; }

      // --- set DICOM attribute values ---

        /** set SeriesInstanceUID (0020,000e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSeriesInstanceUID(const OFString &value, const OFBool check = OFTrue);

        /** set StudyInstanceUID (0020,000d)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setStudyInstanceUID(const OFString &value, const OFBool check = OFTrue);

        /** set TypeOfInstances (0040,e020)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTypeOfInstances(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// DICOMMediaRetrievalSequence (0040,e022) vr=SQ, vm=1, type=1C
        DRTDICOMMediaRetrievalSequence DICOMMediaRetrievalSequence;
        /// DICOMRetrievalSequence (0040,e021) vr=SQ, vm=1, type=1C
        DRTDICOMRetrievalSequence DICOMRetrievalSequence;
        /// ReferencedSOPSequence (0008,1199) vr=SQ, vm=1, type=1
        DRTReferencedSOPSequence ReferencedSOPSequence;
        /// SeriesInstanceUID (0020,000e) vr=UI, vm=1, type=1C
        DcmUniqueIdentifier SeriesInstanceUID;
        /// StudyInstanceUID (0020,000d) vr=UI, vm=1, type=1C
        DcmUniqueIdentifier StudyInstanceUID;
        /// TypeOfInstances (0040,e020) vr=CS, vm=1, type=1
        DcmCodeString TypeOfInstances;
        /// WADORSRetrievalSequence (0040,e025) vr=SQ, vm=1, type=1C
        DRTWADORSRetrievalSequence WADORSRetrievalSequence;
        /// WADORetrievalSequence (0040,e023) vr=SQ, vm=1, type=1C
        DRTWADORetrievalSequence WADORetrievalSequence;
        /// XDSRetrievalSequence (0040,e024) vr=SQ, vm=1, type=1C
        DRTXDSRetrievalSequence XDSRetrievalSequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTReferencedPatientPhotoSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTReferencedPatientPhotoSequence(const DRTReferencedPatientPhotoSequence &copy);

    /** destructor
     */
    virtual ~DRTReferencedPatientPhotoSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTReferencedPatientPhotoSequence &operator=(const DRTReferencedPatientPhotoSequence &copy);

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
