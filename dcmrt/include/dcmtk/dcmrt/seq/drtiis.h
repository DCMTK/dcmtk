/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIconImageSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTIIS_H
#define DRTIIS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for IconImageSequence (0088,0200)
 */
class DCMTK_DCMRT_EXPORT DRTIconImageSequence
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

        /** get BitsAllocated (0028,0100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBitsAllocated(Uint16 &value, const unsigned long pos = 0) const;

        /** get BitsStored (0028,0101)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBitsStored(Uint16 &value, const unsigned long pos = 0) const;

        /** get BluePaletteColorLookupTableData (0028,1203)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

        /** get BluePaletteColorLookupTableDescriptor (0028,1103)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

        /** get Columns (0028,0011)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getColumns(Uint16 &value, const unsigned long pos = 0) const;

        /** get GreenPaletteColorLookupTableData (0028,1202)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

        /** get GreenPaletteColorLookupTableDescriptor (0028,1102)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

        /** get HighBit (0028,0102)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getHighBit(Uint16 &value, const unsigned long pos = 0) const;

        /** get ICCProfile (0028,2000)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getICCProfile(Uint8 *&value, unsigned long *count = NULL) const;

        /** get LargestImagePixelValue (0028,0107)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getLargestImagePixelValue(Uint16 &value, const unsigned long pos = 0) const;

        /** get PhotometricInterpretation (0028,0004)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPhotometricInterpretation(OFString &value, const signed long pos = 0) const;

        /** get PixelAspectRatio (0028,0034)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPixelAspectRatio(OFString &value, const signed long pos = 0) const;

        /** get PixelAspectRatio (0028,0034)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPixelAspectRatio(Sint32 &value, const unsigned long pos = 0) const;

        /** get PixelRepresentation (0028,0103)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPixelRepresentation(Uint16 &value, const unsigned long pos = 0) const;

        /** get PlanarConfiguration (0028,0006)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPlanarConfiguration(Uint16 &value, const unsigned long pos = 0) const;

        /** get RedPaletteColorLookupTableData (0028,1201)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

        /** get RedPaletteColorLookupTableDescriptor (0028,1101)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

        /** get Rows (0028,0010)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRows(Uint16 &value, const unsigned long pos = 0) const;

        /** get SamplesPerPixel (0028,0002)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSamplesPerPixel(Uint16 &value, const unsigned long pos = 0) const;

        /** get SmallestImagePixelValue (0028,0106)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSmallestImagePixelValue(Uint16 &value, const unsigned long pos = 0) const;

      // --- get pixel data element ---

        /** get PixelData (7fe0,0010)
         *  @return reference to pixel data element
         */
        DcmPixelData &getPixelData()
            { return PixelData; }

        /** get PixelData (7fe0,0010)
         *  @return const reference to pixel data element
         */
        const DcmPixelData &getPixelData() const
            { return PixelData; }

      // --- set DICOM attribute values ---

        /** set BitsAllocated (0028,0100)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBitsAllocated(const Uint16 value, const unsigned long pos = 0);

        /** set BitsStored (0028,0101)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBitsStored(const Uint16 value, const unsigned long pos = 0);

        /** set BluePaletteColorLookupTableData (0028,1203)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

        /** set BluePaletteColorLookupTableDescriptor (0028,1103)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=3
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

        /** set Columns (0028,0011)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setColumns(const Uint16 value, const unsigned long pos = 0);

        /** set GreenPaletteColorLookupTableData (0028,1202)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

        /** set GreenPaletteColorLookupTableDescriptor (0028,1102)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=3
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

        /** set HighBit (0028,0102)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setHighBit(const Uint16 value, const unsigned long pos = 0);

        /** set ICCProfile (0028,2000)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setICCProfile(const Uint8 *value, const unsigned long count);

        /** set LargestImagePixelValue (0028,0107)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setLargestImagePixelValue(const Uint16 value, const unsigned long pos = 0);

        /** set PhotometricInterpretation (0028,0004)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPhotometricInterpretation(const OFString &value, const OFBool check = OFTrue);

        /** set PixelAspectRatio (0028,0034)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (2) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPixelAspectRatio(const OFString &value, const OFBool check = OFTrue);

        /** set PixelRepresentation (0028,0103)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPixelRepresentation(const Uint16 value, const unsigned long pos = 0);

        /** set PlanarConfiguration (0028,0006)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPlanarConfiguration(const Uint16 value, const unsigned long pos = 0);

        /** set RedPaletteColorLookupTableData (0028,1201)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

        /** set RedPaletteColorLookupTableDescriptor (0028,1101)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=3
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

        /** set Rows (0028,0010)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRows(const Uint16 value, const unsigned long pos = 0);

        /** set SamplesPerPixel (0028,0002)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSamplesPerPixel(const Uint16 value, const unsigned long pos = 0);

        /** set SmallestImagePixelValue (0028,0106)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSmallestImagePixelValue(const Uint16 value, const unsigned long pos = 0);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BitsAllocated (0028,0100) vr=US, vm=1, type=1
        DcmUnsignedShort BitsAllocated;
        /// BitsStored (0028,0101) vr=US, vm=1, type=1
        DcmUnsignedShort BitsStored;
        /// BluePaletteColorLookupTableData (0028,1203) vr=OW, vm=1, type=1C
        DcmOtherByteOtherWord BluePaletteColorLookupTableData;
        /// BluePaletteColorLookupTableDescriptor (0028,1103) vr=US/SS, vm=3, type=1C
        DcmUnsignedShort BluePaletteColorLookupTableDescriptor;
        /// Columns (0028,0011) vr=US, vm=1, type=1
        DcmUnsignedShort Columns;
        /// GreenPaletteColorLookupTableData (0028,1202) vr=OW, vm=1, type=1C
        DcmOtherByteOtherWord GreenPaletteColorLookupTableData;
        /// GreenPaletteColorLookupTableDescriptor (0028,1102) vr=US/SS, vm=3, type=1C
        DcmUnsignedShort GreenPaletteColorLookupTableDescriptor;
        /// HighBit (0028,0102) vr=US, vm=1, type=1
        DcmUnsignedShort HighBit;
        /// ICCProfile (0028,2000) vr=OB, vm=1, type=3
        DcmOtherByteOtherWord ICCProfile;
        /// LargestImagePixelValue (0028,0107) vr=US/SS, vm=1, type=3
        DcmUnsignedShort LargestImagePixelValue;
        /// PhotometricInterpretation (0028,0004) vr=CS, vm=1, type=1
        DcmCodeString PhotometricInterpretation;
        /// PixelAspectRatio (0028,0034) vr=IS, vm=2, type=1C
        DcmIntegerString PixelAspectRatio;
        /// PixelData (7fe0,0010) vr=OB/OW, vm=1, type=1C
        DcmPixelData PixelData;
        /// PixelRepresentation (0028,0103) vr=US, vm=1, type=1
        DcmUnsignedShort PixelRepresentation;
        /// PlanarConfiguration (0028,0006) vr=US, vm=1, type=1C
        DcmUnsignedShort PlanarConfiguration;
        /// RedPaletteColorLookupTableData (0028,1201) vr=OW, vm=1, type=1C
        DcmOtherByteOtherWord RedPaletteColorLookupTableData;
        /// RedPaletteColorLookupTableDescriptor (0028,1101) vr=US/SS, vm=3, type=1C
        DcmUnsignedShort RedPaletteColorLookupTableDescriptor;
        /// Rows (0028,0010) vr=US, vm=1, type=1
        DcmUnsignedShort Rows;
        /// SamplesPerPixel (0028,0002) vr=US, vm=1, type=1
        DcmUnsignedShort SamplesPerPixel;
        /// SmallestImagePixelValue (0028,0106) vr=US/SS, vm=1, type=3
        DcmUnsignedShort SmallestImagePixelValue;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIconImageSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIconImageSequence(const DRTIconImageSequence &copy);

    /** destructor
     */
    virtual ~DRTIconImageSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTIconImageSequence &operator=(const DRTIconImageSequence &copy);

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
