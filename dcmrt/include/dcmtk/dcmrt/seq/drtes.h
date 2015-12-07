/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTExposureSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTES_H
#define DRTES_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtas5.h"    // for ApplicatorSequence
#include "dcmtk/dcmrt/seq/drtblds5.h"  // for BeamLimitingDeviceSequence
#include "dcmtk/dcmrt/seq/drtbl5.h"    // for BlockSequence
#include "dcmtk/dcmrt/seq/drtgas.h"    // for GeneralAccessorySequence
#include "dcmtk/dcmrt/seq/drtpfms.h"   // for PrimaryFluenceModeSequence


/** Interface class for ExposureSequence (3002,0030) in RTImageModule
 */
class DCMTK_DCMRT_EXPORT DRTExposureSequence
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

        /** get BeamLimitingDeviceAngle (300a,0120)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamLimitingDeviceAngle(OFString &value, const signed long pos = 0) const;

        /** get BeamLimitingDeviceAngle (300a,0120)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos = 0) const;

        /** get DiaphragmPosition (3002,0034)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDiaphragmPosition(OFString &value, const signed long pos = 0) const;

        /** get DiaphragmPosition (3002,0034)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDiaphragmPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get DiaphragmPosition (3002,0034)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDiaphragmPosition(OFVector<Float64> &value) const;

        /** get ExposureTime (0018,1150)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getExposureTime(OFString &value, const signed long pos = 0) const;

        /** get ExposureTime (0018,1150)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getExposureTime(Sint32 &value, const unsigned long pos = 0) const;

        /** get ExposureTimeInms (0018,9328)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getExposureTimeInms(Float64 &value, const unsigned long pos = 0) const;

        /** get GantryAngle (300a,011e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryAngle(OFString &value, const signed long pos = 0) const;

        /** get GantryAngle (300a,011e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryAngle(Float64 &value, const unsigned long pos = 0) const;

        /** get GantryPitchAngle (300a,014a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryPitchAngle(Float32 &value, const unsigned long pos = 0) const;

        /** get KVP (0018,0060)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getKVP(OFString &value, const signed long pos = 0) const;

        /** get KVP (0018,0060)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getKVP(Float64 &value, const unsigned long pos = 0) const;

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

        /** get NumberOfBlocks (300a,00f0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBlocks(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBlocks (300a,00f0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBlocks(Sint32 &value, const unsigned long pos = 0) const;

        /** get PatientSupportAngle (300a,0122)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportAngle(OFString &value, const signed long pos = 0) const;

        /** get PatientSupportAngle (300a,0122)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportAngle(Float64 &value, const unsigned long pos = 0) const;

        /** get ReferencedFrameNumber (0008,1160)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedFrameNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedFrameNumber (0008,1160)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedFrameNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get TableTopLateralPosition (300a,012a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralPosition(OFString &value, const signed long pos = 0) const;

        /** get TableTopLateralPosition (300a,012a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLateralPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopLongitudinalPosition (300a,0129)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalPosition(OFString &value, const signed long pos = 0) const;

        /** get TableTopLongitudinalPosition (300a,0129)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopPitchAngle (300a,0140)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopPitchAngle(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopRollAngle (300a,0144)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopRollAngle(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopVerticalPosition (300a,0128)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalPosition(OFString &value, const signed long pos = 0) const;

        /** get TableTopVerticalPosition (300a,0128)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopVerticalPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get XRayTubeCurrent (0018,1151)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getXRayTubeCurrent(OFString &value, const signed long pos = 0) const;

        /** get XRayTubeCurrent (0018,1151)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getXRayTubeCurrent(Sint32 &value, const unsigned long pos = 0) const;

        /** get XRayTubeCurrentInmA (0018,9330)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getXRayTubeCurrentInmA(Float64 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ApplicatorSequence (300a,0107)
         *  @return reference to sequence element
         */
        DRTApplicatorSequenceInRTImageModule &getApplicatorSequence()
            { return ApplicatorSequence; }

        /** get ApplicatorSequence (300a,0107)
         *  @return const reference to sequence element
         */
        const DRTApplicatorSequenceInRTImageModule &getApplicatorSequence() const
            { return ApplicatorSequence; }

        /** get BeamLimitingDeviceSequence (300a,00b6)
         *  @return reference to sequence element
         */
        DRTBeamLimitingDeviceSequenceInRTImageModule &getBeamLimitingDeviceSequence()
            { return BeamLimitingDeviceSequence; }

        /** get BeamLimitingDeviceSequence (300a,00b6)
         *  @return const reference to sequence element
         */
        const DRTBeamLimitingDeviceSequenceInRTImageModule &getBeamLimitingDeviceSequence() const
            { return BeamLimitingDeviceSequence; }

        /** get BlockSequence (300a,00f4)
         *  @return reference to sequence element
         */
        DRTBlockSequenceInRTImageModule &getBlockSequence()
            { return BlockSequence; }

        /** get BlockSequence (300a,00f4)
         *  @return const reference to sequence element
         */
        const DRTBlockSequenceInRTImageModule &getBlockSequence() const
            { return BlockSequence; }

        /** get GeneralAccessorySequence (300a,0420)
         *  @return reference to sequence element
         */
        DRTGeneralAccessorySequence &getGeneralAccessorySequence()
            { return GeneralAccessorySequence; }

        /** get GeneralAccessorySequence (300a,0420)
         *  @return const reference to sequence element
         */
        const DRTGeneralAccessorySequence &getGeneralAccessorySequence() const
            { return GeneralAccessorySequence; }

        /** get PrimaryFluenceModeSequence (3002,0050)
         *  @return reference to sequence element
         */
        DRTPrimaryFluenceModeSequence &getPrimaryFluenceModeSequence()
            { return PrimaryFluenceModeSequence; }

        /** get PrimaryFluenceModeSequence (3002,0050)
         *  @return const reference to sequence element
         */
        const DRTPrimaryFluenceModeSequence &getPrimaryFluenceModeSequence() const
            { return PrimaryFluenceModeSequence; }

      // --- set DICOM attribute values ---

        /** set BeamLimitingDeviceAngle (300a,0120)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamLimitingDeviceAngle(const OFString &value, const OFBool check = OFTrue);

        /** set DiaphragmPosition (3002,0034)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (4) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDiaphragmPosition(const OFString &value, const OFBool check = OFTrue);

        /** set ExposureTime (0018,1150)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setExposureTime(const OFString &value, const OFBool check = OFTrue);

        /** set ExposureTimeInms (0018,9328)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setExposureTimeInms(const Float64 value, const unsigned long pos = 0);

        /** set GantryAngle (300a,011e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGantryAngle(const OFString &value, const OFBool check = OFTrue);

        /** set GantryPitchAngle (300a,014a)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGantryPitchAngle(const Float32 value, const unsigned long pos = 0);

        /** set KVP (0018,0060)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setKVP(const OFString &value, const OFBool check = OFTrue);

        /** set MetersetExposure (3002,0032)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMetersetExposure(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfBlocks (300a,00f0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBlocks(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportAngle (300a,0122)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportAngle(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedFrameNumber (0008,1160)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedFrameNumber(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopLateralPosition (300a,012a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLateralPosition(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopLongitudinalPosition (300a,0129)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopLongitudinalPosition(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopPitchAngle (300a,0140)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopPitchAngle(const Float32 value, const unsigned long pos = 0);

        /** set TableTopRollAngle (300a,0144)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopRollAngle(const Float32 value, const unsigned long pos = 0);

        /** set TableTopVerticalPosition (300a,0128)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopVerticalPosition(const OFString &value, const OFBool check = OFTrue);

        /** set XRayTubeCurrent (0018,1151)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setXRayTubeCurrent(const OFString &value, const OFBool check = OFTrue);

        /** set XRayTubeCurrentInmA (0018,9330)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setXRayTubeCurrentInmA(const Float64 value, const unsigned long pos = 0);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ApplicatorSequence (300a,0107) vr=SQ, vm=1, type=3
        DRTApplicatorSequenceInRTImageModule ApplicatorSequence;
        /// BeamLimitingDeviceAngle (300a,0120) vr=DS, vm=1, type=3
        DcmDecimalString BeamLimitingDeviceAngle;
        /// BeamLimitingDeviceSequence (300a,00b6) vr=SQ, vm=1, type=3
        DRTBeamLimitingDeviceSequenceInRTImageModule BeamLimitingDeviceSequence;
        /// BlockSequence (300a,00f4) vr=SQ, vm=1, type=2C
        DRTBlockSequenceInRTImageModule BlockSequence;
        /// DiaphragmPosition (3002,0034) vr=DS, vm=4, type=3
        DcmDecimalString DiaphragmPosition;
        /// ExposureTime (0018,1150) vr=IS, vm=1, type=2C
        DcmIntegerString ExposureTime;
        /// ExposureTimeInms (0018,9328) vr=FD, vm=1, type=3
        DcmFloatingPointDouble ExposureTimeInms;
        /// GantryAngle (300a,011e) vr=DS, vm=1, type=3
        DcmDecimalString GantryAngle;
        /// GantryPitchAngle (300a,014a) vr=FL, vm=1, type=3
        DcmFloatingPointSingle GantryPitchAngle;
        /// GeneralAccessorySequence (300a,0420) vr=SQ, vm=1, type=3
        DRTGeneralAccessorySequence GeneralAccessorySequence;
        /// KVP (0018,0060) vr=DS, vm=1, type=2C
        DcmDecimalString KVP;
        /// MetersetExposure (3002,0032) vr=DS, vm=1, type=2C
        DcmDecimalString MetersetExposure;
        /// NumberOfBlocks (300a,00f0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfBlocks;
        /// PatientSupportAngle (300a,0122) vr=DS, vm=1, type=3
        DcmDecimalString PatientSupportAngle;
        /// PrimaryFluenceModeSequence (3002,0050) vr=SQ, vm=1, type=3
        DRTPrimaryFluenceModeSequence PrimaryFluenceModeSequence;
        /// ReferencedFrameNumber (0008,1160) vr=IS, vm=1-n, type=1C
        DcmIntegerString ReferencedFrameNumber;
        /// TableTopLateralPosition (300a,012a) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLateralPosition;
        /// TableTopLongitudinalPosition (300a,0129) vr=DS, vm=1, type=3
        DcmDecimalString TableTopLongitudinalPosition;
        /// TableTopPitchAngle (300a,0140) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TableTopPitchAngle;
        /// TableTopRollAngle (300a,0144) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TableTopRollAngle;
        /// TableTopVerticalPosition (300a,0128) vr=DS, vm=1, type=3
        DcmDecimalString TableTopVerticalPosition;
        /// XRayTubeCurrent (0018,1151) vr=IS, vm=1, type=2C
        DcmIntegerString XRayTubeCurrent;
        /// XRayTubeCurrentInmA (0018,9330) vr=FD, vm=1, type=3
        DcmFloatingPointDouble XRayTubeCurrentInmA;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTExposureSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTExposureSequence(const DRTExposureSequence &copy);

    /** destructor
     */
    virtual ~DRTExposureSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTExposureSequence &operator=(const DRTExposureSequence &copy);

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
