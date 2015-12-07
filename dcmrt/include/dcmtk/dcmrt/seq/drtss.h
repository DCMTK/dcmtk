/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTSourceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTSS_H
#define DRTSS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class


/** Interface class for SourceSequence (300a,0210)
 */
class DCMTK_DCMRT_EXPORT DRTSourceSequence
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

        /** get ActiveSourceDiameter (300a,0218)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getActiveSourceDiameter(OFString &value, const signed long pos = 0) const;

        /** get ActiveSourceDiameter (300a,0218)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getActiveSourceDiameter(Float64 &value, const unsigned long pos = 0) const;

        /** get ActiveSourceLength (300a,021a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getActiveSourceLength(OFString &value, const signed long pos = 0) const;

        /** get ActiveSourceLength (300a,021a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getActiveSourceLength(Float64 &value, const unsigned long pos = 0) const;

        /** get MaterialID (300a,00e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMaterialID(OFString &value, const signed long pos = 0) const;

        /** get ReferenceAirKermaRate (300a,022a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferenceAirKermaRate(OFString &value, const signed long pos = 0) const;

        /** get ReferenceAirKermaRate (300a,022a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferenceAirKermaRate(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceDescription (300a,021c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceDescription(OFString &value, const signed long pos = 0) const;

        /** get SourceEncapsulationNominalThickness (300a,0222)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceEncapsulationNominalThickness(OFString &value, const signed long pos = 0) const;

        /** get SourceEncapsulationNominalThickness (300a,0222)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceEncapsulationNominalThickness(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceEncapsulationNominalTransmission (300a,0224)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceEncapsulationNominalTransmission(OFString &value, const signed long pos = 0) const;

        /** get SourceEncapsulationNominalTransmission (300a,0224)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceEncapsulationNominalTransmission(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceIsotopeHalfLife (300a,0228)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceIsotopeHalfLife(OFString &value, const signed long pos = 0) const;

        /** get SourceIsotopeHalfLife (300a,0228)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceIsotopeHalfLife(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceIsotopeName (300a,0226)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceIsotopeName(OFString &value, const signed long pos = 0) const;

        /** get SourceManufacturer (300a,0216)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceManufacturer(OFString &value, const signed long pos = 0) const;

        /** get SourceModelID (300a,021b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceModelID(OFString &value, const signed long pos = 0) const;

        /** get SourceNumber (300a,0212)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceNumber(OFString &value, const signed long pos = 0) const;

        /** get SourceNumber (300a,0212)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get SourceSerialNumber (3008,0105)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceSerialNumber(OFString &value, const signed long pos = 0) const;

        /** get SourceStrength (300a,022b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceStrength(OFString &value, const signed long pos = 0) const;

        /** get SourceStrength (300a,022b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceStrength(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceStrengthReferenceDate (300a,022c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceStrengthReferenceDate(OFString &value, const signed long pos = 0) const;

        /** get SourceStrengthReferenceTime (300a,022e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceStrengthReferenceTime(OFString &value, const signed long pos = 0) const;

        /** get SourceStrengthUnits (300a,0229)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceStrengthUnits(OFString &value, const signed long pos = 0) const;

        /** get SourceType (300a,0214)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceType(OFString &value, const signed long pos = 0) const;

      // --- set DICOM attribute values ---

        /** set ActiveSourceDiameter (300a,0218)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setActiveSourceDiameter(const OFString &value, const OFBool check = OFTrue);

        /** set ActiveSourceLength (300a,021a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setActiveSourceLength(const OFString &value, const OFBool check = OFTrue);

        /** set MaterialID (300a,00e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMaterialID(const OFString &value, const OFBool check = OFTrue);

        /** set ReferenceAirKermaRate (300a,022a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferenceAirKermaRate(const OFString &value, const OFBool check = OFTrue);

        /** set SourceDescription (300a,021c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceDescription(const OFString &value, const OFBool check = OFTrue);

        /** set SourceEncapsulationNominalThickness (300a,0222)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceEncapsulationNominalThickness(const OFString &value, const OFBool check = OFTrue);

        /** set SourceEncapsulationNominalTransmission (300a,0224)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceEncapsulationNominalTransmission(const OFString &value, const OFBool check = OFTrue);

        /** set SourceIsotopeHalfLife (300a,0228)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceIsotopeHalfLife(const OFString &value, const OFBool check = OFTrue);

        /** set SourceIsotopeName (300a,0226)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceIsotopeName(const OFString &value, const OFBool check = OFTrue);

        /** set SourceManufacturer (300a,0216)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set SourceModelID (300a,021b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceModelID(const OFString &value, const OFBool check = OFTrue);

        /** set SourceNumber (300a,0212)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceNumber(const OFString &value, const OFBool check = OFTrue);

        /** set SourceSerialNumber (3008,0105)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceSerialNumber(const OFString &value, const OFBool check = OFTrue);

        /** set SourceStrength (300a,022b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceStrength(const OFString &value, const OFBool check = OFTrue);

        /** set SourceStrengthReferenceDate (300a,022c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceStrengthReferenceDate(const OFString &value, const OFBool check = OFTrue);

        /** set SourceStrengthReferenceTime (300a,022e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceStrengthReferenceTime(const OFString &value, const OFBool check = OFTrue);

        /** set SourceStrengthUnits (300a,0229)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceStrengthUnits(const OFString &value, const OFBool check = OFTrue);

        /** set SourceType (300a,0214)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceType(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ActiveSourceDiameter (300a,0218) vr=DS, vm=1, type=3
        DcmDecimalString ActiveSourceDiameter;
        /// ActiveSourceLength (300a,021a) vr=DS, vm=1, type=3
        DcmDecimalString ActiveSourceLength;
        /// MaterialID (300a,00e1) vr=SH, vm=1, type=3
        DcmShortString MaterialID;
        /// ReferenceAirKermaRate (300a,022a) vr=DS, vm=1, type=1
        DcmDecimalString ReferenceAirKermaRate;
        /// SourceDescription (300a,021c) vr=LO, vm=1, type=3
        DcmLongString SourceDescription;
        /// SourceEncapsulationNominalThickness (300a,0222) vr=DS, vm=1, type=3
        DcmDecimalString SourceEncapsulationNominalThickness;
        /// SourceEncapsulationNominalTransmission (300a,0224) vr=DS, vm=1, type=3
        DcmDecimalString SourceEncapsulationNominalTransmission;
        /// SourceIsotopeHalfLife (300a,0228) vr=DS, vm=1, type=1
        DcmDecimalString SourceIsotopeHalfLife;
        /// SourceIsotopeName (300a,0226) vr=LO, vm=1, type=1
        DcmLongString SourceIsotopeName;
        /// SourceManufacturer (300a,0216) vr=LO, vm=1, type=3
        DcmLongString SourceManufacturer;
        /// SourceModelID (300a,021b) vr=SH, vm=1, type=3
        DcmShortString SourceModelID;
        /// SourceNumber (300a,0212) vr=IS, vm=1, type=1
        DcmIntegerString SourceNumber;
        /// SourceSerialNumber (3008,0105) vr=LO, vm=1, type=3
        DcmLongString SourceSerialNumber;
        /// SourceStrength (300a,022b) vr=DS, vm=1, type=1C
        DcmDecimalString SourceStrength;
        /// SourceStrengthReferenceDate (300a,022c) vr=DA, vm=1, type=1
        DcmDate SourceStrengthReferenceDate;
        /// SourceStrengthReferenceTime (300a,022e) vr=TM, vm=1, type=1
        DcmTime SourceStrengthReferenceTime;
        /// SourceStrengthUnits (300a,0229) vr=CS, vm=1, type=1C
        DcmCodeString SourceStrengthUnits;
        /// SourceType (300a,0214) vr=CS, vm=1, type=1
        DcmCodeString SourceType;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTSourceSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTSourceSequence(const DRTSourceSequence &copy);

    /** destructor
     */
    virtual ~DRTSourceSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTSourceSequence &operator=(const DRTSourceSequence &copy);

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
