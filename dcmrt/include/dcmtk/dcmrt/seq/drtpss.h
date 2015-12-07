/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTPatientSetupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTPSS_H
#define DRTPSS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtfds.h"    // for FixationDeviceSequence
#include "dcmtk/dcmrt/seq/drtmss.h"    // for MotionSynchronizationSequence
#include "dcmtk/dcmrt/seq/drtrsis.h"   // for ReferencedSetupImageSequence
#include "dcmtk/dcmrt/seq/drtsds.h"    // for SetupDeviceSequence
#include "dcmtk/dcmrt/seq/drtshds.h"   // for ShieldingDeviceSequence


/** Interface class for PatientSetupSequence (300a,0180)
 */
class DCMTK_DCMRT_EXPORT DRTPatientSetupSequence
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

        /** get PatientAdditionalPosition (300a,0184)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientAdditionalPosition(OFString &value, const signed long pos = 0) const;

        /** get PatientPosition (0018,5100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientPosition(OFString &value, const signed long pos = 0) const;

        /** get PatientSetupLabel (300a,0183)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSetupLabel(OFString &value, const signed long pos = 0) const;

        /** get PatientSetupNumber (300a,0182)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSetupNumber(OFString &value, const signed long pos = 0) const;

        /** get PatientSetupNumber (300a,0182)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSetupNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get SetupTechnique (300a,01b0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSetupTechnique(OFString &value, const signed long pos = 0) const;

        /** get SetupTechniqueDescription (300a,01b2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSetupTechniqueDescription(OFString &value, const signed long pos = 0) const;

        /** get TableTopLateralSetupDisplacement (300a,01d6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralSetupDisplacement(OFString &value, const signed long pos = 0) const;

        /** get TableTopLateralSetupDisplacement (300a,01d6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralSetupDisplacement(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopLongitudinalSetupDisplacement (300a,01d4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalSetupDisplacement(OFString &value, const signed long pos = 0) const;

        /** get TableTopLongitudinalSetupDisplacement (300a,01d4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalSetupDisplacement(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopVerticalSetupDisplacement (300a,01d2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalSetupDisplacement(OFString &value, const signed long pos = 0) const;

        /** get TableTopVerticalSetupDisplacement (300a,01d2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalSetupDisplacement(Float64 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get FixationDeviceSequence (300a,0190)
         *  @return reference to sequence element
         */
        DRTFixationDeviceSequence &getFixationDeviceSequence()
            { return FixationDeviceSequence; }

        /** get FixationDeviceSequence (300a,0190)
         *  @return const reference to sequence element
         */
        const DRTFixationDeviceSequence &getFixationDeviceSequence() const
            { return FixationDeviceSequence; }

        /** get MotionSynchronizationSequence (300a,0410)
         *  @return reference to sequence element
         */
        DRTMotionSynchronizationSequence &getMotionSynchronizationSequence()
            { return MotionSynchronizationSequence; }

        /** get MotionSynchronizationSequence (300a,0410)
         *  @return const reference to sequence element
         */
        const DRTMotionSynchronizationSequence &getMotionSynchronizationSequence() const
            { return MotionSynchronizationSequence; }

        /** get ReferencedSetupImageSequence (300a,0401)
         *  @return reference to sequence element
         */
        DRTReferencedSetupImageSequence &getReferencedSetupImageSequence()
            { return ReferencedSetupImageSequence; }

        /** get ReferencedSetupImageSequence (300a,0401)
         *  @return const reference to sequence element
         */
        const DRTReferencedSetupImageSequence &getReferencedSetupImageSequence() const
            { return ReferencedSetupImageSequence; }

        /** get SetupDeviceSequence (300a,01b4)
         *  @return reference to sequence element
         */
        DRTSetupDeviceSequence &getSetupDeviceSequence()
            { return SetupDeviceSequence; }

        /** get SetupDeviceSequence (300a,01b4)
         *  @return const reference to sequence element
         */
        const DRTSetupDeviceSequence &getSetupDeviceSequence() const
            { return SetupDeviceSequence; }

        /** get ShieldingDeviceSequence (300a,01a0)
         *  @return reference to sequence element
         */
        DRTShieldingDeviceSequence &getShieldingDeviceSequence()
            { return ShieldingDeviceSequence; }

        /** get ShieldingDeviceSequence (300a,01a0)
         *  @return const reference to sequence element
         */
        const DRTShieldingDeviceSequence &getShieldingDeviceSequence() const
            { return ShieldingDeviceSequence; }

      // --- set DICOM attribute values ---

        /** set PatientAdditionalPosition (300a,0184)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientAdditionalPosition(const OFString &value, const OFBool check = OFTrue);

        /** set PatientPosition (0018,5100)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientPosition(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSetupLabel (300a,0183)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSetupLabel(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSetupNumber (300a,0182)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSetupNumber(const OFString &value, const OFBool check = OFTrue);

        /** set SetupTechnique (300a,01b0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSetupTechnique(const OFString &value, const OFBool check = OFTrue);

        /** set SetupTechniqueDescription (300a,01b2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSetupTechniqueDescription(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopLateralSetupDisplacement (300a,01d6)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLateralSetupDisplacement(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopLongitudinalSetupDisplacement (300a,01d4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLongitudinalSetupDisplacement(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopVerticalSetupDisplacement (300a,01d2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopVerticalSetupDisplacement(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// FixationDeviceSequence (300a,0190) vr=SQ, vm=1, type=3
        DRTFixationDeviceSequence FixationDeviceSequence;
        /// MotionSynchronizationSequence (300a,0410) vr=SQ, vm=1, type=3
        DRTMotionSynchronizationSequence MotionSynchronizationSequence;
        /// PatientAdditionalPosition (300a,0184) vr=LO, vm=1, type=1C
        DcmLongString PatientAdditionalPosition;
        /// PatientPosition (0018,5100) vr=CS, vm=1, type=1C
        DcmCodeString PatientPosition;
        /// PatientSetupLabel (300a,0183) vr=LO, vm=1, type=3
        DcmLongString PatientSetupLabel;
        /// PatientSetupNumber (300a,0182) vr=IS, vm=1, type=1
        DcmIntegerString PatientSetupNumber;
        /// ReferencedSetupImageSequence (300a,0401) vr=SQ, vm=1, type=3
        DRTReferencedSetupImageSequence ReferencedSetupImageSequence;
        /// SetupDeviceSequence (300a,01b4) vr=SQ, vm=1, type=3
        DRTSetupDeviceSequence SetupDeviceSequence;
        /// SetupTechnique (300a,01b0) vr=CS, vm=1, type=3
        DcmCodeString SetupTechnique;
        /// SetupTechniqueDescription (300a,01b2) vr=ST, vm=1, type=3
        DcmShortText SetupTechniqueDescription;
        /// ShieldingDeviceSequence (300a,01a0) vr=SQ, vm=1, type=3
        DRTShieldingDeviceSequence ShieldingDeviceSequence;
        /// TableTopLateralSetupDisplacement (300a,01d6) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLateralSetupDisplacement;
        /// TableTopLongitudinalSetupDisplacement (300a,01d4) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLongitudinalSetupDisplacement;
        /// TableTopVerticalSetupDisplacement (300a,01d2) vr=DS, vm=1, type=3
        DcmDecimalString TableTopVerticalSetupDisplacement;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTPatientSetupSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTPatientSetupSequence(const DRTPatientSetupSequence &copy);

    /** destructor
     */
    virtual ~DRTPatientSetupSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTPatientSetupSequence &operator=(const DRTPatientSetupSequence &copy);

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
