/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonControlPointDeliverySequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTICPDS_H
#define DRTICPDS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbldps.h"  // for BeamLimitingDevicePositionSequence
#include "dcmtk/dcmrt/seq/drtcpas.h"   // for CorrectedParameterSequence
#include "dcmtk/dcmrt/seq/drtiwps.h"   // for IonWedgePositionSequence
#include "dcmtk/dcmrt/seq/drtlsds7.h"  // for LateralSpreadingDeviceSettingsSequence
#include "dcmtk/dcmrt/seq/drtos.h"     // for OverrideSequence
#include "dcmtk/dcmrt/seq/drtrmss7.h"  // for RangeModulatorSettingsSequence
#include "dcmtk/dcmrt/seq/drtrshs7.h"  // for RangeShifterSettingsSequence


/** Interface class for IonControlPointDeliverySequence (3008,0041) in RTIonBeamsSessionRecordModule
 */
class DCMTK_DCMRT_EXPORT DRTIonControlPointDeliverySequence
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

        /** get BeamLimitingDeviceRotationDirection (300a,0121)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamLimitingDeviceRotationDirection(OFString &value, const signed long pos = 0) const;

        /** get DeliveredMeterset (3008,0044)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredMeterset(OFString &value, const signed long pos = 0) const;

        /** get DeliveredMeterset (3008,0044)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredMeterset(Float64 &value, const unsigned long pos = 0) const;

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

        /** get GantryPitchRotationDirection (300a,014c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryPitchRotationDirection(OFString &value, const signed long pos = 0) const;

        /** get GantryRotationDirection (300a,011f)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getGantryRotationDirection(OFString &value, const signed long pos = 0) const;

        /** get HeadFixationAngle (300a,0148)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getHeadFixationAngle(Float32 &value, const unsigned long pos = 0) const;

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

        /** get MetersetRateDelivered (3008,0046)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMetersetRateDelivered(Float32 &value, const unsigned long pos = 0) const;

        /** get MetersetRateSet (3008,0045)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMetersetRateSet(Float32 &value, const unsigned long pos = 0) const;

        /** get NominalBeamEnergy (300a,0114)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNominalBeamEnergy(OFString &value, const signed long pos = 0) const;

        /** get NominalBeamEnergy (300a,0114)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNominalBeamEnergy(Float64 &value, const unsigned long pos = 0) const;

        /** get NumberOfPaintings (300a,039a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfPaintings(OFString &value, const signed long pos = 0) const;

        /** get NumberOfPaintings (300a,039a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfPaintings(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfScanSpotPositions (300a,0392)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfScanSpotPositions(OFString &value, const signed long pos = 0) const;

        /** get NumberOfScanSpotPositions (300a,0392)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfScanSpotPositions(Sint32 &value, const unsigned long pos = 0) const;

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

        /** get PatientSupportRotationDirection (300a,0123)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportRotationDirection(OFString &value, const signed long pos = 0) const;

        /** get ReferencedControlPointIndex (300c,00f0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedControlPointIndex(OFString &value, const signed long pos = 0) const;

        /** get ReferencedControlPointIndex (300c,00f0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedControlPointIndex(Sint32 &value, const unsigned long pos = 0) const;

        /** get ScanSpotMetersetsDelivered (3008,0047)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanSpotMetersetsDelivered(Float32 &value, const unsigned long pos = 0) const;

        /** get ScanSpotPositionMap (300a,0394)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanSpotPositionMap(Float32 &value, const unsigned long pos = 0) const;

        /** get ScanSpotTuneID (300a,0390)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanSpotTuneID(OFString &value, const signed long pos = 0) const;

        /** get ScanningSpotSize (300a,0398)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanningSpotSize(Float32 &value, const unsigned long pos = 0) const;

        /** get SnoutPosition (300a,030d)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSnoutPosition(Float32 &value, const unsigned long pos = 0) const;

        /** get SpecifiedMeterset (3008,0042)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedMeterset(OFString &value, const signed long pos = 0) const;

        /** get SpecifiedMeterset (3008,0042)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedMeterset(Float64 &value, const unsigned long pos = 0) const;

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

        /** get TableTopPitchRotationDirection (300a,0142)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopPitchRotationDirection(OFString &value, const signed long pos = 0) const;

        /** get TableTopRollAngle (300a,0144)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopRollAngle(Float32 &value, const unsigned long pos = 0) const;

        /** get TableTopRollRotationDirection (300a,0146)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopRollRotationDirection(OFString &value, const signed long pos = 0) const;

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

        /** get TreatmentControlPointDate (3008,0024)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentControlPointDate(OFString &value, const signed long pos = 0) const;

        /** get TreatmentControlPointTime (3008,0025)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentControlPointTime(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get BeamLimitingDevicePositionSequence (300a,011a)
         *  @return reference to sequence element
         */
        DRTBeamLimitingDevicePositionSequence &getBeamLimitingDevicePositionSequence()
            { return BeamLimitingDevicePositionSequence; }

        /** get BeamLimitingDevicePositionSequence (300a,011a)
         *  @return const reference to sequence element
         */
        const DRTBeamLimitingDevicePositionSequence &getBeamLimitingDevicePositionSequence() const
            { return BeamLimitingDevicePositionSequence; }

        /** get CorrectedParameterSequence (3008,0068)
         *  @return reference to sequence element
         */
        DRTCorrectedParameterSequence &getCorrectedParameterSequence()
            { return CorrectedParameterSequence; }

        /** get CorrectedParameterSequence (3008,0068)
         *  @return const reference to sequence element
         */
        const DRTCorrectedParameterSequence &getCorrectedParameterSequence() const
            { return CorrectedParameterSequence; }

        /** get IonWedgePositionSequence (300a,03ac)
         *  @return reference to sequence element
         */
        DRTIonWedgePositionSequence &getIonWedgePositionSequence()
            { return IonWedgePositionSequence; }

        /** get IonWedgePositionSequence (300a,03ac)
         *  @return const reference to sequence element
         */
        const DRTIonWedgePositionSequence &getIonWedgePositionSequence() const
            { return IonWedgePositionSequence; }

        /** get LateralSpreadingDeviceSettingsSequence (300a,0370)
         *  @return reference to sequence element
         */
        DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsSessionRecordModule &getLateralSpreadingDeviceSettingsSequence()
            { return LateralSpreadingDeviceSettingsSequence; }

        /** get LateralSpreadingDeviceSettingsSequence (300a,0370)
         *  @return const reference to sequence element
         */
        const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsSessionRecordModule &getLateralSpreadingDeviceSettingsSequence() const
            { return LateralSpreadingDeviceSettingsSequence; }

        /** get OverrideSequence (3008,0060)
         *  @return reference to sequence element
         */
        DRTOverrideSequence &getOverrideSequence()
            { return OverrideSequence; }

        /** get OverrideSequence (3008,0060)
         *  @return const reference to sequence element
         */
        const DRTOverrideSequence &getOverrideSequence() const
            { return OverrideSequence; }

        /** get RangeModulatorSettingsSequence (300a,0380)
         *  @return reference to sequence element
         */
        DRTRangeModulatorSettingsSequenceInRTIonBeamsSessionRecordModule &getRangeModulatorSettingsSequence()
            { return RangeModulatorSettingsSequence; }

        /** get RangeModulatorSettingsSequence (300a,0380)
         *  @return const reference to sequence element
         */
        const DRTRangeModulatorSettingsSequenceInRTIonBeamsSessionRecordModule &getRangeModulatorSettingsSequence() const
            { return RangeModulatorSettingsSequence; }

        /** get RangeShifterSettingsSequence (300a,0360)
         *  @return reference to sequence element
         */
        DRTRangeShifterSettingsSequenceInRTIonBeamsSessionRecordModule &getRangeShifterSettingsSequence()
            { return RangeShifterSettingsSequence; }

        /** get RangeShifterSettingsSequence (300a,0360)
         *  @return const reference to sequence element
         */
        const DRTRangeShifterSettingsSequenceInRTIonBeamsSessionRecordModule &getRangeShifterSettingsSequence() const
            { return RangeShifterSettingsSequence; }

      // --- set DICOM attribute values ---

        /** set BeamLimitingDeviceAngle (300a,0120)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamLimitingDeviceAngle(const OFString &value, const OFBool check = OFTrue);

        /** set BeamLimitingDeviceRotationDirection (300a,0121)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamLimitingDeviceRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveredMeterset (3008,0044)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveredMeterset(const OFString &value, const OFBool check = OFTrue);

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

        /** set GantryPitchRotationDirection (300a,014c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGantryPitchRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set GantryRotationDirection (300a,011f)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setGantryRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set HeadFixationAngle (300a,0148)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setHeadFixationAngle(const Float32 value, const unsigned long pos = 0);

        /** set KVP (0018,0060)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setKVP(const OFString &value, const OFBool check = OFTrue);

        /** set MetersetRateDelivered (3008,0046)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMetersetRateDelivered(const Float32 value, const unsigned long pos = 0);

        /** set MetersetRateSet (3008,0045)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMetersetRateSet(const Float32 value, const unsigned long pos = 0);

        /** set NominalBeamEnergy (300a,0114)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNominalBeamEnergy(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfPaintings (300a,039a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfPaintings(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfScanSpotPositions (300a,0392)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfScanSpotPositions(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportAngle (300a,0122)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportAngle(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportRotationDirection (300a,0123)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedControlPointIndex (300c,00f0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedControlPointIndex(const OFString &value, const OFBool check = OFTrue);

        /** set ScanSpotMetersetsDelivered (3008,0047)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanSpotMetersetsDelivered(const Float32 value, const unsigned long pos = 0);

        /** set ScanSpotPositionMap (300a,0394)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanSpotPositionMap(const Float32 value, const unsigned long pos = 0);

        /** set ScanSpotTuneID (300a,0390)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanSpotTuneID(const OFString &value, const OFBool check = OFTrue);

        /** set ScanningSpotSize (300a,0398)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=2
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanningSpotSize(const Float32 value, const unsigned long pos = 0);

        /** set SnoutPosition (300a,030d)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSnoutPosition(const Float32 value, const unsigned long pos = 0);

        /** set SpecifiedMeterset (3008,0042)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSpecifiedMeterset(const OFString &value, const OFBool check = OFTrue);

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

        /** set TableTopPitchRotationDirection (300a,0142)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopPitchRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopRollAngle (300a,0144)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopRollAngle(const Float32 value, const unsigned long pos = 0);

        /** set TableTopRollRotationDirection (300a,0146)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopRollRotationDirection(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopVerticalPosition (300a,0128)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopVerticalPosition(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentControlPointDate (3008,0024)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentControlPointDate(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentControlPointTime (3008,0025)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentControlPointTime(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BeamLimitingDeviceAngle (300a,0120) vr=DS, vm=1, type=1C
        DcmDecimalString BeamLimitingDeviceAngle;
        /// BeamLimitingDevicePositionSequence (300a,011a) vr=SQ, vm=1, type=1C
        DRTBeamLimitingDevicePositionSequence BeamLimitingDevicePositionSequence;
        /// BeamLimitingDeviceRotationDirection (300a,0121) vr=CS, vm=1, type=1C
        DcmCodeString BeamLimitingDeviceRotationDirection;
        /// CorrectedParameterSequence (3008,0068) vr=SQ, vm=1, type=3
        DRTCorrectedParameterSequence CorrectedParameterSequence;
        /// DeliveredMeterset (3008,0044) vr=DS, vm=1, type=1
        DcmDecimalString DeliveredMeterset;
        /// GantryAngle (300a,011e) vr=DS, vm=1, type=1C
        DcmDecimalString GantryAngle;
        /// GantryPitchAngle (300a,014a) vr=FL, vm=1, type=2C
        DcmFloatingPointSingle GantryPitchAngle;
        /// GantryPitchRotationDirection (300a,014c) vr=CS, vm=1, type=2C
        DcmCodeString GantryPitchRotationDirection;
        /// GantryRotationDirection (300a,011f) vr=CS, vm=1, type=1C
        DcmCodeString GantryRotationDirection;
        /// HeadFixationAngle (300a,0148) vr=FL, vm=1, type=3
        DcmFloatingPointSingle HeadFixationAngle;
        /// IonWedgePositionSequence (300a,03ac) vr=SQ, vm=1, type=1C
        DRTIonWedgePositionSequence IonWedgePositionSequence;
        /// KVP (0018,0060) vr=DS, vm=1, type=1C
        DcmDecimalString KVP;
        /// LateralSpreadingDeviceSettingsSequence (300a,0370) vr=SQ, vm=1, type=1C
        DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsSessionRecordModule LateralSpreadingDeviceSettingsSequence;
        /// MetersetRateDelivered (3008,0046) vr=FL, vm=1, type=3
        DcmFloatingPointSingle MetersetRateDelivered;
        /// MetersetRateSet (3008,0045) vr=FL, vm=1, type=3
        DcmFloatingPointSingle MetersetRateSet;
        /// NominalBeamEnergy (300a,0114) vr=DS, vm=1, type=1C
        DcmDecimalString NominalBeamEnergy;
        /// NumberOfPaintings (300a,039a) vr=IS, vm=1, type=1C
        DcmIntegerString NumberOfPaintings;
        /// NumberOfScanSpotPositions (300a,0392) vr=IS, vm=1, type=1C
        DcmIntegerString NumberOfScanSpotPositions;
        /// OverrideSequence (3008,0060) vr=SQ, vm=1, type=3
        DRTOverrideSequence OverrideSequence;
        /// PatientSupportAngle (300a,0122) vr=DS, vm=1, type=1C
        DcmDecimalString PatientSupportAngle;
        /// PatientSupportRotationDirection (300a,0123) vr=CS, vm=1, type=1C
        DcmCodeString PatientSupportRotationDirection;
        /// RangeModulatorSettingsSequence (300a,0380) vr=SQ, vm=1, type=1C
        DRTRangeModulatorSettingsSequenceInRTIonBeamsSessionRecordModule RangeModulatorSettingsSequence;
        /// RangeShifterSettingsSequence (300a,0360) vr=SQ, vm=1, type=1C
        DRTRangeShifterSettingsSequenceInRTIonBeamsSessionRecordModule RangeShifterSettingsSequence;
        /// ReferencedControlPointIndex (300c,00f0) vr=IS, vm=1, type=1
        DcmIntegerString ReferencedControlPointIndex;
        /// ScanSpotMetersetsDelivered (3008,0047) vr=FL, vm=1-n, type=1C
        DcmFloatingPointSingle ScanSpotMetersetsDelivered;
        /// ScanSpotPositionMap (300a,0394) vr=FL, vm=1-n, type=1C
        DcmFloatingPointSingle ScanSpotPositionMap;
        /// ScanSpotTuneID (300a,0390) vr=SH, vm=1, type=1C
        DcmShortString ScanSpotTuneID;
        /// ScanningSpotSize (300a,0398) vr=FL, vm=2, type=3
        DcmFloatingPointSingle ScanningSpotSize;
        /// SnoutPosition (300a,030d) vr=FL, vm=1, type=2C
        DcmFloatingPointSingle SnoutPosition;
        /// SpecifiedMeterset (3008,0042) vr=DS, vm=1, type=2
        DcmDecimalString SpecifiedMeterset;
        /// TableTopLateralPosition (300a,012a) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopLateralPosition;
        /// TableTopLongitudinalPosition (300a,0129) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopLongitudinalPosition;
        /// TableTopPitchAngle (300a,0140) vr=FL, vm=1, type=2C
        DcmFloatingPointSingle TableTopPitchAngle;
        /// TableTopPitchRotationDirection (300a,0142) vr=CS, vm=1, type=2C
        DcmCodeString TableTopPitchRotationDirection;
        /// TableTopRollAngle (300a,0144) vr=FL, vm=1, type=2C
        DcmFloatingPointSingle TableTopRollAngle;
        /// TableTopRollRotationDirection (300a,0146) vr=CS, vm=1, type=2C
        DcmCodeString TableTopRollRotationDirection;
        /// TableTopVerticalPosition (300a,0128) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopVerticalPosition;
        /// TreatmentControlPointDate (3008,0024) vr=DA, vm=1, type=1
        DcmDate TreatmentControlPointDate;
        /// TreatmentControlPointTime (3008,0025) vr=TM, vm=1, type=1
        DcmTime TreatmentControlPointTime;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIonControlPointDeliverySequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIonControlPointDeliverySequence(const DRTIonControlPointDeliverySequence &copy);

    /** destructor
     */
    virtual ~DRTIonControlPointDeliverySequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTIonControlPointDeliverySequence &operator=(const DRTIonControlPointDeliverySequence &copy);

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
