/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTICPS_H
#define DRTICPS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbldps.h"  // for BeamLimitingDevicePositionSequence
#include "dcmtk/dcmrt/seq/drtiwps.h"   // for IonWedgePositionSequence
#include "dcmtk/dcmrt/seq/drtlsds6.h"  // for LateralSpreadingDeviceSettingsSequence
#include "dcmtk/dcmrt/seq/drtrmss6.h"  // for RangeModulatorSettingsSequence
#include "dcmtk/dcmrt/seq/drtrshs6.h"  // for RangeShifterSettingsSequence
#include "dcmtk/dcmrt/seq/drtrdrs6.h"  // for ReferencedDoseReferenceSequence


/** Interface class for IonControlPointSequence (300a,03a8) in RTIonBeamsModule
 */
class DCMTK_DCMRT_EXPORT DRTIonControlPointSequence
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

        /** get ControlPointIndex (300a,0112)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getControlPointIndex(OFString &value, const signed long pos = 0) const;

        /** get ControlPointIndex (300a,0112)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getControlPointIndex(Sint32 &value, const unsigned long pos = 0) const;

        /** get CumulativeMetersetWeight (300a,0134)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

        /** get CumulativeMetersetWeight (300a,0134)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

        /** get ExternalContourEntryPoint (300a,0133)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getExternalContourEntryPoint(Float32 &value, const unsigned long pos = 0) const;

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

        /** get IsocenterPosition (300a,012c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getIsocenterPosition(OFString &value, const signed long pos = 0) const;

        /** get IsocenterPosition (300a,012c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getIsocenterPosition(Float64 &value, const unsigned long pos = 0) const;

        /** get IsocenterPosition (300a,012c)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getIsocenterPosition(OFVector<Float64> &value) const;

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

        /** get MetersetRate (300a,035a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMetersetRate(Float32 &value, const unsigned long pos = 0) const;

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

        /** get ScanSpotMetersetWeights (300a,0396)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanSpotMetersetWeights(Float32 &value, const unsigned long pos = 0) const;

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

        /** get SurfaceEntryPoint (300a,012e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSurfaceEntryPoint(OFString &value, const signed long pos = 0) const;

        /** get SurfaceEntryPoint (300a,012e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSurfaceEntryPoint(Float64 &value, const unsigned long pos = 0) const;

        /** get SurfaceEntryPoint (300a,012e)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSurfaceEntryPoint(OFVector<Float64> &value) const;

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
        DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule &getLateralSpreadingDeviceSettingsSequence()
            { return LateralSpreadingDeviceSettingsSequence; }

        /** get LateralSpreadingDeviceSettingsSequence (300a,0370)
         *  @return const reference to sequence element
         */
        const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule &getLateralSpreadingDeviceSettingsSequence() const
            { return LateralSpreadingDeviceSettingsSequence; }

        /** get RangeModulatorSettingsSequence (300a,0380)
         *  @return reference to sequence element
         */
        DRTRangeModulatorSettingsSequenceInRTIonBeamsModule &getRangeModulatorSettingsSequence()
            { return RangeModulatorSettingsSequence; }

        /** get RangeModulatorSettingsSequence (300a,0380)
         *  @return const reference to sequence element
         */
        const DRTRangeModulatorSettingsSequenceInRTIonBeamsModule &getRangeModulatorSettingsSequence() const
            { return RangeModulatorSettingsSequence; }

        /** get RangeShifterSettingsSequence (300a,0360)
         *  @return reference to sequence element
         */
        DRTRangeShifterSettingsSequenceInRTIonBeamsModule &getRangeShifterSettingsSequence()
            { return RangeShifterSettingsSequence; }

        /** get RangeShifterSettingsSequence (300a,0360)
         *  @return const reference to sequence element
         */
        const DRTRangeShifterSettingsSequenceInRTIonBeamsModule &getRangeShifterSettingsSequence() const
            { return RangeShifterSettingsSequence; }

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return reference to sequence element
         */
        DRTReferencedDoseReferenceSequenceInRTIonBeamsModule &getReferencedDoseReferenceSequence()
            { return ReferencedDoseReferenceSequence; }

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return const reference to sequence element
         */
        const DRTReferencedDoseReferenceSequenceInRTIonBeamsModule &getReferencedDoseReferenceSequence() const
            { return ReferencedDoseReferenceSequence; }

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

        /** set ControlPointIndex (300a,0112)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setControlPointIndex(const OFString &value, const OFBool check = OFTrue);

        /** set CumulativeMetersetWeight (300a,0134)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

        /** set ExternalContourEntryPoint (300a,0133)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=3
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setExternalContourEntryPoint(const Float32 value, const unsigned long pos = 0);

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

        /** set IsocenterPosition (300a,012c)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setIsocenterPosition(const OFString &value, const OFBool check = OFTrue);

        /** set KVP (0018,0060)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setKVP(const OFString &value, const OFBool check = OFTrue);

        /** set MetersetRate (300a,035a)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMetersetRate(const Float32 value, const unsigned long pos = 0);

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

        /** set ScanSpotMetersetWeights (300a,0396)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanSpotMetersetWeights(const Float32 value, const unsigned long pos = 0);

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

        /** set SurfaceEntryPoint (300a,012e)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSurfaceEntryPoint(const OFString &value, const OFBool check = OFTrue);

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

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// BeamLimitingDeviceAngle (300a,0120) vr=DS, vm=1, type=1C
        DcmDecimalString BeamLimitingDeviceAngle;
        /// BeamLimitingDevicePositionSequence (300a,011a) vr=SQ, vm=1, type=1C
        DRTBeamLimitingDevicePositionSequence BeamLimitingDevicePositionSequence;
        /// BeamLimitingDeviceRotationDirection (300a,0121) vr=CS, vm=1, type=1C
        DcmCodeString BeamLimitingDeviceRotationDirection;
        /// ControlPointIndex (300a,0112) vr=IS, vm=1, type=1
        DcmIntegerString ControlPointIndex;
        /// CumulativeMetersetWeight (300a,0134) vr=DS, vm=1, type=2
        DcmDecimalString CumulativeMetersetWeight;
        /// ExternalContourEntryPoint (300a,0133) vr=FL, vm=3, type=3
        DcmFloatingPointSingle ExternalContourEntryPoint;
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
        /// IsocenterPosition (300a,012c) vr=DS, vm=3, type=2C
        DcmDecimalString IsocenterPosition;
        /// KVP (0018,0060) vr=DS, vm=1, type=1C
        DcmDecimalString KVP;
        /// LateralSpreadingDeviceSettingsSequence (300a,0370) vr=SQ, vm=1, type=1C
        DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule LateralSpreadingDeviceSettingsSequence;
        /// MetersetRate (300a,035a) vr=FL, vm=1, type=3
        DcmFloatingPointSingle MetersetRate;
        /// NominalBeamEnergy (300a,0114) vr=DS, vm=1, type=1C
        DcmDecimalString NominalBeamEnergy;
        /// NumberOfPaintings (300a,039a) vr=IS, vm=1, type=1C
        DcmIntegerString NumberOfPaintings;
        /// NumberOfScanSpotPositions (300a,0392) vr=IS, vm=1, type=1C
        DcmIntegerString NumberOfScanSpotPositions;
        /// PatientSupportAngle (300a,0122) vr=DS, vm=1, type=1C
        DcmDecimalString PatientSupportAngle;
        /// PatientSupportRotationDirection (300a,0123) vr=CS, vm=1, type=1C
        DcmCodeString PatientSupportRotationDirection;
        /// RangeModulatorSettingsSequence (300a,0380) vr=SQ, vm=1, type=1C
        DRTRangeModulatorSettingsSequenceInRTIonBeamsModule RangeModulatorSettingsSequence;
        /// RangeShifterSettingsSequence (300a,0360) vr=SQ, vm=1, type=1C
        DRTRangeShifterSettingsSequenceInRTIonBeamsModule RangeShifterSettingsSequence;
        /// ReferencedDoseReferenceSequence (300c,0050) vr=SQ, vm=1, type=3
        DRTReferencedDoseReferenceSequenceInRTIonBeamsModule ReferencedDoseReferenceSequence;
        /// ScanSpotMetersetWeights (300a,0396) vr=FL, vm=1-n, type=1C
        DcmFloatingPointSingle ScanSpotMetersetWeights;
        /// ScanSpotPositionMap (300a,0394) vr=FL, vm=1-n, type=1C
        DcmFloatingPointSingle ScanSpotPositionMap;
        /// ScanSpotTuneID (300a,0390) vr=SH, vm=1, type=1C
        DcmShortString ScanSpotTuneID;
        /// ScanningSpotSize (300a,0398) vr=FL, vm=2, type=3
        DcmFloatingPointSingle ScanningSpotSize;
        /// SnoutPosition (300a,030d) vr=FL, vm=1, type=2C
        DcmFloatingPointSingle SnoutPosition;
        /// SurfaceEntryPoint (300a,012e) vr=DS, vm=3, type=3
        DcmDecimalString SurfaceEntryPoint;
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

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIonControlPointSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIonControlPointSequence(const DRTIonControlPointSequence &copy);

    /** destructor
     */
    virtual ~DRTIonControlPointSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTIonControlPointSequence &operator=(const DRTIonControlPointSequence &copy);

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
