/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTCPS_H
#define DRTCPS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbldps.h"  // for BeamLimitingDevicePositionSequence
#include "dcmtk/dcmrt/seq/drtrdrs1.h"  // for ReferencedDoseReferenceSequence
#include "dcmtk/dcmrt/seq/drtrds.h"    // for ReferencedDoseSequence
#include "dcmtk/dcmrt/seq/drtwps.h"    // for WedgePositionSequence


/** Interface class for ControlPointSequence (300a,0111) in RTBeamsModule
 */
class DCMTK_DCMRT_EXPORT DRTControlPointSequence
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

        /** get DoseRateSet (300a,0115)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDoseRateSet(OFString &value, const signed long pos = 0) const;

        /** get DoseRateSet (300a,0115)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDoseRateSet(Float64 &value, const unsigned long pos = 0) const;

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

        /** get SourceToExternalContourDistance (300a,0132)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToExternalContourDistance(Float32 &value, const unsigned long pos = 0) const;

        /** get SourceToSurfaceDistance (300a,0130)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToSurfaceDistance(OFString &value, const signed long pos = 0) const;

        /** get SourceToSurfaceDistance (300a,0130)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceToSurfaceDistance(Float64 &value, const unsigned long pos = 0) const;

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

        /** get TableTopEccentricAngle (300a,0125)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopEccentricAngle(OFString &value, const signed long pos = 0) const;

        /** get TableTopEccentricAngle (300a,0125)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopEccentricAngle(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopEccentricAxisDistance (300a,0124)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopEccentricAxisDistance(OFString &value, const signed long pos = 0) const;

        /** get TableTopEccentricAxisDistance (300a,0124)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopEccentricAxisDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get TableTopEccentricRotationDirection (300a,0126)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTableTopEccentricRotationDirection(OFString &value, const signed long pos = 0) const;

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

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return reference to sequence element
         */
        DRTReferencedDoseReferenceSequenceInRTBeamsModule &getReferencedDoseReferenceSequence()
            { return ReferencedDoseReferenceSequence; }

        /** get ReferencedDoseReferenceSequence (300c,0050)
         *  @return const reference to sequence element
         */
        const DRTReferencedDoseReferenceSequenceInRTBeamsModule &getReferencedDoseReferenceSequence() const
            { return ReferencedDoseReferenceSequence; }

        /** get ReferencedDoseSequence (300c,0080)
         *  @return reference to sequence element
         */
        DRTReferencedDoseSequence &getReferencedDoseSequence()
            { return ReferencedDoseSequence; }

        /** get ReferencedDoseSequence (300c,0080)
         *  @return const reference to sequence element
         */
        const DRTReferencedDoseSequence &getReferencedDoseSequence() const
            { return ReferencedDoseSequence; }

        /** get WedgePositionSequence (300a,0116)
         *  @return reference to sequence element
         */
        DRTWedgePositionSequence &getWedgePositionSequence()
            { return WedgePositionSequence; }

        /** get WedgePositionSequence (300a,0116)
         *  @return const reference to sequence element
         */
        const DRTWedgePositionSequence &getWedgePositionSequence() const
            { return WedgePositionSequence; }

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

        /** set DoseRateSet (300a,0115)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDoseRateSet(const OFString &value, const OFBool check = OFTrue);

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

        /** set IsocenterPosition (300a,012c)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setIsocenterPosition(const OFString &value, const OFBool check = OFTrue);

        /** set NominalBeamEnergy (300a,0114)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNominalBeamEnergy(const OFString &value, const OFBool check = OFTrue);

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

        /** set SourceToExternalContourDistance (300a,0132)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceToExternalContourDistance(const Float32 value, const unsigned long pos = 0);

        /** set SourceToSurfaceDistance (300a,0130)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceToSurfaceDistance(const OFString &value, const OFBool check = OFTrue);

        /** set SurfaceEntryPoint (300a,012e)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSurfaceEntryPoint(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopEccentricAngle (300a,0125)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopEccentricAngle(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopEccentricAxisDistance (300a,0124)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopEccentricAxisDistance(const OFString &value, const OFBool check = OFTrue);

        /** set TableTopEccentricRotationDirection (300a,0126)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTableTopEccentricRotationDirection(const OFString &value, const OFBool check = OFTrue);

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
        /// DoseRateSet (300a,0115) vr=DS, vm=1, type=3
        DcmDecimalString DoseRateSet;
        /// ExternalContourEntryPoint (300a,0133) vr=FL, vm=3, type=3
        DcmFloatingPointSingle ExternalContourEntryPoint;
        /// GantryAngle (300a,011e) vr=DS, vm=1, type=1C
        DcmDecimalString GantryAngle;
        /// GantryPitchAngle (300a,014a) vr=FL, vm=1, type=3
        DcmFloatingPointSingle GantryPitchAngle;
        /// GantryPitchRotationDirection (300a,014c) vr=CS, vm=1, type=3
        DcmCodeString GantryPitchRotationDirection;
        /// GantryRotationDirection (300a,011f) vr=CS, vm=1, type=1C
        DcmCodeString GantryRotationDirection;
        /// IsocenterPosition (300a,012c) vr=DS, vm=3, type=2C
        DcmDecimalString IsocenterPosition;
        /// NominalBeamEnergy (300a,0114) vr=DS, vm=1, type=3
        DcmDecimalString NominalBeamEnergy;
        /// PatientSupportAngle (300a,0122) vr=DS, vm=1, type=1C
        DcmDecimalString PatientSupportAngle;
        /// PatientSupportRotationDirection (300a,0123) vr=CS, vm=1, type=1C
        DcmCodeString PatientSupportRotationDirection;
        /// ReferencedDoseReferenceSequence (300c,0050) vr=SQ, vm=1, type=3
        DRTReferencedDoseReferenceSequenceInRTBeamsModule ReferencedDoseReferenceSequence;
        /// ReferencedDoseSequence (300c,0080) vr=SQ, vm=1, type=1C
        DRTReferencedDoseSequence ReferencedDoseSequence;
        /// SourceToExternalContourDistance (300a,0132) vr=FL, vm=1, type=3
        DcmFloatingPointSingle SourceToExternalContourDistance;
        /// SourceToSurfaceDistance (300a,0130) vr=DS, vm=1, type=3
        DcmDecimalString SourceToSurfaceDistance;
        /// SurfaceEntryPoint (300a,012e) vr=DS, vm=3, type=3
        DcmDecimalString SurfaceEntryPoint;
        /// TableTopEccentricAngle (300a,0125) vr=DS, vm=1, type=1C
        DcmDecimalString TableTopEccentricAngle;
        /// TableTopEccentricAxisDistance (300a,0124) vr=DS, vm=1, type=3
        DcmDecimalString TableTopEccentricAxisDistance;
        /// TableTopEccentricRotationDirection (300a,0126) vr=CS, vm=1, type=1C
        DcmCodeString TableTopEccentricRotationDirection;
        /// TableTopLateralPosition (300a,012a) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopLateralPosition;
        /// TableTopLongitudinalPosition (300a,0129) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopLongitudinalPosition;
        /// TableTopPitchAngle (300a,0140) vr=FL, vm=1, type=1C
        DcmFloatingPointSingle TableTopPitchAngle;
        /// TableTopPitchRotationDirection (300a,0142) vr=CS, vm=1, type=1C
        DcmCodeString TableTopPitchRotationDirection;
        /// TableTopRollAngle (300a,0144) vr=FL, vm=1, type=1C
        DcmFloatingPointSingle TableTopRollAngle;
        /// TableTopRollRotationDirection (300a,0146) vr=CS, vm=1, type=1C
        DcmCodeString TableTopRollRotationDirection;
        /// TableTopVerticalPosition (300a,0128) vr=DS, vm=1, type=2C
        DcmDecimalString TableTopVerticalPosition;
        /// WedgePositionSequence (300a,0116) vr=SQ, vm=1, type=1C
        DRTWedgePositionSequence WedgePositionSequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTControlPointSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTControlPointSequence(const DRTControlPointSequence &copy);

    /** destructor
     */
    virtual ~DRTControlPointSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTControlPointSequence &operator=(const DRTControlPointSequence &copy);

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
