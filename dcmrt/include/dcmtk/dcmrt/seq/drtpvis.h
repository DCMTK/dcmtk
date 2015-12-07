/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTPlannedVerificationImageSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTPVIS_H
#define DRTPVIS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for PlannedVerificationImageSequence (300a,00ca)
 */
class DCMTK_DCMRT_EXPORT DRTPlannedVerificationImageSequence
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

        /** get EndCumulativeMetersetWeight (300c,0009)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getEndCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

        /** get EndCumulativeMetersetWeight (300c,0009)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getEndCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

        /** get ImagingDeviceSpecificAcquisitionParameters (300a,00cc)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getImagingDeviceSpecificAcquisitionParameters(OFString &value, const signed long pos = 0) const;

        /** get MetersetExposure (3002,0032)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMetersetExposure(OFString &value, const signed long pos = 0) const;

        /** get MetersetExposure (3002,0032)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMetersetExposure(Float64 &value, const unsigned long pos = 0) const;

        /** get RTImageOrientation (3002,0010)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImageOrientation(OFString &value, const signed long pos = 0) const;

        /** get RTImageOrientation (3002,0010)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImageOrientation(Float64 &value, const unsigned long pos = 0) const;

        /** get RTImageOrientation (3002,0010)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImageOrientation(OFVector<Float64> &value) const;

        /** get RTImagePlane (3002,000c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImagePlane(OFString &value, const signed long pos = 0) const;

        /** get RTImagePosition (3002,0012)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImagePosition(OFString &value, const signed long pos = 0) const;

        /** get RTImagePosition (3002,0012)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImagePosition(Float64 &value, const unsigned long pos = 0) const;

        /** get RTImagePosition (3002,0012)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImagePosition(OFVector<Float64> &value) const;

        /** get RTImageSID (3002,0026)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImageSID(OFString &value, const signed long pos = 0) const;

        /** get RTImageSID (3002,0026)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRTImageSID(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedReferenceImageNumber (300c,0007)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedReferenceImageNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedReferenceImageNumber (300c,0007)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedReferenceImageNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get StartCumulativeMetersetWeight (300c,0008)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getStartCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

        /** get StartCumulativeMetersetWeight (300c,0008)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getStartCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

        /** get XRayImageReceptorAngle (3002,000e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getXRayImageReceptorAngle(OFString &value, const signed long pos = 0) const;

        /** get XRayImageReceptorAngle (3002,000e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getXRayImageReceptorAngle(Float64 &value, const unsigned long pos = 0) const;

      // --- set DICOM attribute values ---

        /** set EndCumulativeMetersetWeight (300c,0009)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setEndCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

        /** set ImagingDeviceSpecificAcquisitionParameters (300a,00cc)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setImagingDeviceSpecificAcquisitionParameters(const OFString &value, const OFBool check = OFTrue);

        /** set MetersetExposure (3002,0032)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMetersetExposure(const OFString &value, const OFBool check = OFTrue);

        /** set RTImageOrientation (3002,0010)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (6) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTImageOrientation(const OFString &value, const OFBool check = OFTrue);

        /** set RTImagePlane (3002,000c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTImagePlane(const OFString &value, const OFBool check = OFTrue);

        /** set RTImagePosition (3002,0012)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTImagePosition(const OFString &value, const OFBool check = OFTrue);

        /** set RTImageSID (3002,0026)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRTImageSID(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedReferenceImageNumber (300c,0007)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedReferenceImageNumber(const OFString &value, const OFBool check = OFTrue);

        /** set StartCumulativeMetersetWeight (300c,0008)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setStartCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

        /** set XRayImageReceptorAngle (3002,000e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setXRayImageReceptorAngle(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// EndCumulativeMetersetWeight (300c,0009) vr=DS, vm=1, type=3
        DcmDecimalString EndCumulativeMetersetWeight;
        /// ImagingDeviceSpecificAcquisitionParameters (300a,00cc) vr=LO, vm=1-n, type=3
        DcmLongString ImagingDeviceSpecificAcquisitionParameters;
        /// MetersetExposure (3002,0032) vr=DS, vm=1, type=3
        DcmDecimalString MetersetExposure;
        /// RTImageOrientation (3002,0010) vr=DS, vm=6, type=3
        DcmDecimalString RTImageOrientation;
        /// RTImagePlane (3002,000c) vr=CS, vm=1, type=3
        DcmCodeString RTImagePlane;
        /// RTImagePosition (3002,0012) vr=DS, vm=2, type=3
        DcmDecimalString RTImagePosition;
        /// RTImageSID (3002,0026) vr=DS, vm=1, type=3
        DcmDecimalString RTImageSID;
        /// ReferencedReferenceImageNumber (300c,0007) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedReferenceImageNumber;
        /// StartCumulativeMetersetWeight (300c,0008) vr=DS, vm=1, type=3
        DcmDecimalString StartCumulativeMetersetWeight;
        /// XRayImageReceptorAngle (3002,000e) vr=DS, vm=1, type=3
        DcmDecimalString XRayImageReceptorAngle;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTPlannedVerificationImageSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTPlannedVerificationImageSequence(const DRTPlannedVerificationImageSequence &copy);

    /** destructor
     */
    virtual ~DRTPlannedVerificationImageSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTPlannedVerificationImageSequence &operator=(const DRTPlannedVerificationImageSequence &copy);

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
