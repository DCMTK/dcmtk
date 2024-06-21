/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTStructureSetROISequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTSSRS_H
#define DRTSSRS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtcvis.h"   // for ConceptualVolumeIdentificationSequence
#include "dcmtk/dcmrt/seq/drtdfss.h"   // for DefinitionSourceSequence
#include "dcmtk/dcmrt/seq/drtdcs.h"    // for DerivationCodeSequence
#include "dcmtk/dcmrt/seq/drtrdais.h"  // for ROIDerivationAlgorithmIdentificationSequence


/** Interface class for StructureSetROISequence (3006,0020)
 */
class DCMTK_DCMRT_EXPORT DRTStructureSetROISequence
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

        /** get ROIDescription (3006,0028)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIDescription(OFString &value, const signed long pos = 0) const;

        /** get ROIGenerationAlgorithm (3006,0036)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIGenerationAlgorithm(OFString &value, const signed long pos = 0) const;

        /** get ROIGenerationDescription (3006,0038)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIGenerationDescription(OFString &value, const signed long pos = 0) const;

        /** get ROIName (3006,0026)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIName(OFString &value, const signed long pos = 0) const;

        /** get ROINumber (3006,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROINumber(OFString &value, const signed long pos = 0) const;

        /** get ROINumber (3006,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROINumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ROIVolume (3006,002c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIVolume(OFString &value, const signed long pos = 0) const;

        /** get ROIVolume (3006,002c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getROIVolume(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedFrameOfReferenceUID (3006,0024)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedFrameOfReferenceUID(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ConceptualVolumeIdentificationSequence (3010,00a0)
         *  @return reference to sequence element
         */
        DRTConceptualVolumeIdentificationSequence &getConceptualVolumeIdentificationSequence()
            { return ConceptualVolumeIdentificationSequence; }

        /** get ConceptualVolumeIdentificationSequence (3010,00a0)
         *  @return const reference to sequence element
         */
        const DRTConceptualVolumeIdentificationSequence &getConceptualVolumeIdentificationSequence() const
            { return ConceptualVolumeIdentificationSequence; }

        /** get DefinitionSourceSequence (0008,1156)
         *  @return reference to sequence element
         */
        DRTDefinitionSourceSequence &getDefinitionSourceSequence()
            { return DefinitionSourceSequence; }

        /** get DefinitionSourceSequence (0008,1156)
         *  @return const reference to sequence element
         */
        const DRTDefinitionSourceSequence &getDefinitionSourceSequence() const
            { return DefinitionSourceSequence; }

        /** get DerivationCodeSequence (0008,9215)
         *  @return reference to sequence element
         */
        DRTDerivationCodeSequence &getDerivationCodeSequence()
            { return DerivationCodeSequence; }

        /** get DerivationCodeSequence (0008,9215)
         *  @return const reference to sequence element
         */
        const DRTDerivationCodeSequence &getDerivationCodeSequence() const
            { return DerivationCodeSequence; }

        /** get ROIDerivationAlgorithmIdentificationSequence (3006,0037)
         *  @return reference to sequence element
         */
        DRTROIDerivationAlgorithmIdentificationSequence &getROIDerivationAlgorithmIdentificationSequence()
            { return ROIDerivationAlgorithmIdentificationSequence; }

        /** get ROIDerivationAlgorithmIdentificationSequence (3006,0037)
         *  @return const reference to sequence element
         */
        const DRTROIDerivationAlgorithmIdentificationSequence &getROIDerivationAlgorithmIdentificationSequence() const
            { return ROIDerivationAlgorithmIdentificationSequence; }

      // --- set DICOM attribute values ---

        /** set ROIDescription (3006,0028)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIDescription(const OFString &value, const OFBool check = OFTrue);

        /** set ROIGenerationAlgorithm (3006,0036)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIGenerationAlgorithm(const OFString &value, const OFBool check = OFTrue);

        /** set ROIGenerationDescription (3006,0038)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIGenerationDescription(const OFString &value, const OFBool check = OFTrue);

        /** set ROIName (3006,0026)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIName(const OFString &value, const OFBool check = OFTrue);

        /** set ROINumber (3006,0022)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROINumber(const OFString &value, const OFBool check = OFTrue);

        /** set ROIVolume (3006,002c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setROIVolume(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedFrameOfReferenceUID (3006,0024)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedFrameOfReferenceUID(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ConceptualVolumeIdentificationSequence (3010,00a0) vr=SQ, vm=1, type=3
        DRTConceptualVolumeIdentificationSequence ConceptualVolumeIdentificationSequence;
        /// DefinitionSourceSequence (0008,1156) vr=SQ, vm=1, type=3
        DRTDefinitionSourceSequence DefinitionSourceSequence;
        /// DerivationCodeSequence (0008,9215) vr=SQ, vm=1, type=3
        DRTDerivationCodeSequence DerivationCodeSequence;
        /// ROIDerivationAlgorithmIdentificationSequence (3006,0037) vr=SQ, vm=1, type=3
        DRTROIDerivationAlgorithmIdentificationSequence ROIDerivationAlgorithmIdentificationSequence;
        /// ROIDescription (3006,0028) vr=ST, vm=1, type=3
        DcmShortText ROIDescription;
        /// ROIGenerationAlgorithm (3006,0036) vr=CS, vm=1, type=2
        DcmCodeString ROIGenerationAlgorithm;
        /// ROIGenerationDescription (3006,0038) vr=LO, vm=1, type=3
        DcmLongString ROIGenerationDescription;
        /// ROIName (3006,0026) vr=LO, vm=1, type=2
        DcmLongString ROIName;
        /// ROINumber (3006,0022) vr=IS, vm=1, type=1
        DcmIntegerString ROINumber;
        /// ROIVolume (3006,002c) vr=DS, vm=1, type=3
        DcmDecimalString ROIVolume;
        /// ReferencedFrameOfReferenceUID (3006,0024) vr=UI, vm=1, type=1
        DcmUniqueIdentifier ReferencedFrameOfReferenceUID;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTStructureSetROISequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTStructureSetROISequence(const DRTStructureSetROISequence &copy);

    /** destructor
     */
    virtual ~DRTStructureSetROISequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTStructureSetROISequence &operator=(const DRTStructureSetROISequence &copy);

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
