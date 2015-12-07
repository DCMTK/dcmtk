/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTChannelSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTCHS_H
#define DRTCHS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbcps.h"   // for BrachyControlPointSequence
#include "dcmtk/dcmrt/seq/drtcshs.h"   // for ChannelShieldSequence


/** Interface class for ChannelSequence (300a,0280)
 */
class DCMTK_DCMRT_EXPORT DRTChannelSequence
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

        /** get ChannelLength (300a,0284)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelLength(OFString &value, const signed long pos = 0) const;

        /** get ChannelLength (300a,0284)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelLength(Float64 &value, const unsigned long pos = 0) const;

        /** get ChannelNumber (300a,0282)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelNumber(OFString &value, const signed long pos = 0) const;

        /** get ChannelNumber (300a,0282)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ChannelTotalTime (300a,0286)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelTotalTime(OFString &value, const signed long pos = 0) const;

        /** get ChannelTotalTime (300a,0286)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getChannelTotalTime(Float64 &value, const unsigned long pos = 0) const;

        /** get FinalCumulativeTimeWeight (300a,02c8)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFinalCumulativeTimeWeight(OFString &value, const signed long pos = 0) const;

        /** get FinalCumulativeTimeWeight (300a,02c8)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFinalCumulativeTimeWeight(Float64 &value, const unsigned long pos = 0) const;

        /** get MaterialID (300a,00e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMaterialID(OFString &value, const signed long pos = 0) const;

        /** get NumberOfControlPoints (300a,0110)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfControlPoints(OFString &value, const signed long pos = 0) const;

        /** get NumberOfControlPoints (300a,0110)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfControlPoints(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfPulses (300a,028a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfPulses(OFString &value, const signed long pos = 0) const;

        /** get NumberOfPulses (300a,028a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfPulses(Sint32 &value, const unsigned long pos = 0) const;

        /** get PulseRepetitionInterval (300a,028c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPulseRepetitionInterval(OFString &value, const signed long pos = 0) const;

        /** get PulseRepetitionInterval (300a,028c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPulseRepetitionInterval(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedROINumber (3006,0084)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedROINumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedROINumber (3006,0084)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedROINumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ReferencedSourceNumber (300c,000e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedSourceNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedSourceNumber (300c,000e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedSourceNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get SourceApplicatorID (300a,0291)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorID(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorLength (300a,0296)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorLength(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorLength (300a,0296)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorLength(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceApplicatorManufacturer (300a,0298)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorManufacturer(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorName (300a,0294)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorName(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorNumber (300a,0290)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorNumber(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorNumber (300a,0290)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get SourceApplicatorStepSize (300a,02a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorStepSize(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorStepSize (300a,02a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorStepSize(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceApplicatorType (300a,0292)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorType(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorWallNominalThickness (300a,029c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorWallNominalThickness(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorWallNominalThickness (300a,029c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorWallNominalThickness(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceApplicatorWallNominalTransmission (300a,029e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorWallNominalTransmission(OFString &value, const signed long pos = 0) const;

        /** get SourceApplicatorWallNominalTransmission (300a,029e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceApplicatorWallNominalTransmission(Float64 &value, const unsigned long pos = 0) const;

        /** get SourceMovementType (300a,0288)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceMovementType(OFString &value, const signed long pos = 0) const;

        /** get TransferTubeLength (300a,02a4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTransferTubeLength(OFString &value, const signed long pos = 0) const;

        /** get TransferTubeLength (300a,02a4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTransferTubeLength(Float64 &value, const unsigned long pos = 0) const;

        /** get TransferTubeNumber (300a,02a2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTransferTubeNumber(OFString &value, const signed long pos = 0) const;

        /** get TransferTubeNumber (300a,02a2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTransferTubeNumber(Sint32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get BrachyControlPointSequence (300a,02d0)
         *  @return reference to sequence element
         */
        DRTBrachyControlPointSequence &getBrachyControlPointSequence()
            { return BrachyControlPointSequence; }

        /** get BrachyControlPointSequence (300a,02d0)
         *  @return const reference to sequence element
         */
        const DRTBrachyControlPointSequence &getBrachyControlPointSequence() const
            { return BrachyControlPointSequence; }

        /** get ChannelShieldSequence (300a,02b0)
         *  @return reference to sequence element
         */
        DRTChannelShieldSequence &getChannelShieldSequence()
            { return ChannelShieldSequence; }

        /** get ChannelShieldSequence (300a,02b0)
         *  @return const reference to sequence element
         */
        const DRTChannelShieldSequence &getChannelShieldSequence() const
            { return ChannelShieldSequence; }

      // --- set DICOM attribute values ---

        /** set ChannelLength (300a,0284)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setChannelLength(const OFString &value, const OFBool check = OFTrue);

        /** set ChannelNumber (300a,0282)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setChannelNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ChannelTotalTime (300a,0286)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setChannelTotalTime(const OFString &value, const OFBool check = OFTrue);

        /** set FinalCumulativeTimeWeight (300a,02c8)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFinalCumulativeTimeWeight(const OFString &value, const OFBool check = OFTrue);

        /** set MaterialID (300a,00e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMaterialID(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfControlPoints (300a,0110)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfControlPoints(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfPulses (300a,028a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfPulses(const OFString &value, const OFBool check = OFTrue);

        /** set PulseRepetitionInterval (300a,028c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPulseRepetitionInterval(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedROINumber (3006,0084)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedROINumber(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedSourceNumber (300c,000e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedSourceNumber(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorID (300a,0291)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorID(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorLength (300a,0296)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorLength(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorManufacturer (300a,0298)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorName (300a,0294)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorName(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorNumber (300a,0290)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorNumber(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorStepSize (300a,02a0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorStepSize(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorType (300a,0292)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorType(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorWallNominalThickness (300a,029c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorWallNominalThickness(const OFString &value, const OFBool check = OFTrue);

        /** set SourceApplicatorWallNominalTransmission (300a,029e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceApplicatorWallNominalTransmission(const OFString &value, const OFBool check = OFTrue);

        /** set SourceMovementType (300a,0288)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceMovementType(const OFString &value, const OFBool check = OFTrue);

        /** set TransferTubeLength (300a,02a4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTransferTubeLength(const OFString &value, const OFBool check = OFTrue);

        /** set TransferTubeNumber (300a,02a2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTransferTubeNumber(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BrachyControlPointSequence (300a,02d0) vr=SQ, vm=1, type=1
        DRTBrachyControlPointSequence BrachyControlPointSequence;
        /// ChannelLength (300a,0284) vr=DS, vm=1, type=2
        DcmDecimalString ChannelLength;
        /// ChannelNumber (300a,0282) vr=IS, vm=1, type=1
        DcmIntegerString ChannelNumber;
        /// ChannelShieldSequence (300a,02b0) vr=SQ, vm=1, type=3
        DRTChannelShieldSequence ChannelShieldSequence;
        /// ChannelTotalTime (300a,0286) vr=DS, vm=1, type=1
        DcmDecimalString ChannelTotalTime;
        /// FinalCumulativeTimeWeight (300a,02c8) vr=DS, vm=1, type=1C
        DcmDecimalString FinalCumulativeTimeWeight;
        /// MaterialID (300a,00e1) vr=SH, vm=1, type=3
        DcmShortString MaterialID;
        /// NumberOfControlPoints (300a,0110) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfControlPoints;
        /// NumberOfPulses (300a,028a) vr=IS, vm=1, type=1C
        DcmIntegerString NumberOfPulses;
        /// PulseRepetitionInterval (300a,028c) vr=DS, vm=1, type=1C
        DcmDecimalString PulseRepetitionInterval;
        /// ReferencedROINumber (3006,0084) vr=IS, vm=1, type=2C
        DcmIntegerString ReferencedROINumber;
        /// ReferencedSourceNumber (300c,000e) vr=IS, vm=1, type=1
        DcmIntegerString ReferencedSourceNumber;
        /// SourceApplicatorID (300a,0291) vr=SH, vm=1, type=2C
        DcmShortString SourceApplicatorID;
        /// SourceApplicatorLength (300a,0296) vr=DS, vm=1, type=1C
        DcmDecimalString SourceApplicatorLength;
        /// SourceApplicatorManufacturer (300a,0298) vr=LO, vm=1, type=3
        DcmLongString SourceApplicatorManufacturer;
        /// SourceApplicatorName (300a,0294) vr=LO, vm=1, type=3
        DcmLongString SourceApplicatorName;
        /// SourceApplicatorNumber (300a,0290) vr=IS, vm=1, type=3
        DcmIntegerString SourceApplicatorNumber;
        /// SourceApplicatorStepSize (300a,02a0) vr=DS, vm=1, type=1C
        DcmDecimalString SourceApplicatorStepSize;
        /// SourceApplicatorType (300a,0292) vr=CS, vm=1, type=1C
        DcmCodeString SourceApplicatorType;
        /// SourceApplicatorWallNominalThickness (300a,029c) vr=DS, vm=1, type=3
        DcmDecimalString SourceApplicatorWallNominalThickness;
        /// SourceApplicatorWallNominalTransmission (300a,029e) vr=DS, vm=1, type=3
        DcmDecimalString SourceApplicatorWallNominalTransmission;
        /// SourceMovementType (300a,0288) vr=CS, vm=1, type=1
        DcmCodeString SourceMovementType;
        /// TransferTubeLength (300a,02a4) vr=DS, vm=1, type=2C
        DcmDecimalString TransferTubeLength;
        /// TransferTubeNumber (300a,02a2) vr=IS, vm=1, type=2
        DcmIntegerString TransferTubeNumber;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTChannelSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTChannelSequence(const DRTChannelSequence &copy);

    /** destructor
     */
    virtual ~DRTChannelSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTChannelSequence &operator=(const DRTChannelSequence &copy);

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
