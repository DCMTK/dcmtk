/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTTreatmentSessionIonBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTTSIBS_H
#define DRTTSIBS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtas7.h"    // for ApplicatorSequence
#include "dcmtk/dcmrt/seq/drtbldls.h"  // for BeamLimitingDeviceLeafPairsSequence
#include "dcmtk/dcmrt/seq/drtgas.h"    // for GeneralAccessorySequence
#include "dcmtk/dcmrt/seq/drticpds.h"  // for IonControlPointDeliverySequence
#include "dcmtk/dcmrt/seq/drtrbls.h"   // for RecordedBlockSequence
#include "dcmtk/dcmrt/seq/drtrcos.h"   // for RecordedCompensatorSequence
#include "dcmtk/dcmrt/seq/drtrlsds.h"  // for RecordedLateralSpreadingDeviceSequence
#include "dcmtk/dcmrt/seq/drtrrms.h"   // for RecordedRangeModulatorSequence
#include "dcmtk/dcmrt/seq/drtrrshs.h"  // for RecordedRangeShifterSequence
#include "dcmtk/dcmrt/seq/drtrsns.h"   // for RecordedSnoutSequence
#include "dcmtk/dcmrt/seq/drtrws.h"    // for RecordedWedgeSequence
#include "dcmtk/dcmrt/seq/drtrbos7.h"  // for ReferencedBolusSequence
#include "dcmtk/dcmrt/seq/drtrcdrs.h"  // for ReferencedCalculatedDoseReferenceSequence
#include "dcmtk/dcmrt/seq/drtrmdrs.h"  // for ReferencedMeasuredDoseReferenceSequence
#include "dcmtk/dcmrt/seq/drtrvis.h"   // for ReferencedVerificationImageSequence


/** Interface class for TreatmentSessionIonBeamSequence (3008,0021) in RTIonBeamsSessionRecordModule
 */
class DCMTK_DCMRT_EXPORT DRTTreatmentSessionIonBeamSequence
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

        /** get BeamDescription (300a,00c3)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamDescription(OFString &value, const signed long pos = 0) const;

        /** get BeamName (300a,00c2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamName(OFString &value, const signed long pos = 0) const;

        /** get BeamType (300a,00c4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamType(OFString &value, const signed long pos = 0) const;

        /** get CurrentFractionNumber (3008,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCurrentFractionNumber(OFString &value, const signed long pos = 0) const;

        /** get CurrentFractionNumber (3008,0022)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCurrentFractionNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get DeliveredPrimaryMeterset (3008,0036)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredPrimaryMeterset(OFString &value, const signed long pos = 0) const;

        /** get DeliveredPrimaryMeterset (3008,0036)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredPrimaryMeterset(Float64 &value, const unsigned long pos = 0) const;

        /** get DeliveredSecondaryMeterset (3008,0037)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredSecondaryMeterset(OFString &value, const signed long pos = 0) const;

        /** get DeliveredSecondaryMeterset (3008,0037)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredSecondaryMeterset(Float64 &value, const unsigned long pos = 0) const;

        /** get DeliveredTreatmentTime (3008,003b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredTreatmentTime(OFString &value, const signed long pos = 0) const;

        /** get DeliveredTreatmentTime (3008,003b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeliveredTreatmentTime(Float64 &value, const unsigned long pos = 0) const;

        /** get FixationLightAzimuthalAngle (300a,0356)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFixationLightAzimuthalAngle(Float32 &value, const unsigned long pos = 0) const;

        /** get FixationLightPolarAngle (300a,0358)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFixationLightPolarAngle(Float32 &value, const unsigned long pos = 0) const;

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

        /** get NumberOfBoli (300a,00ed)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBoli(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBoli (300a,00ed)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBoli(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfCompensators (300a,00e0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfCompensators(OFString &value, const signed long pos = 0) const;

        /** get NumberOfCompensators (300a,00e0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfCompensators(Sint32 &value, const unsigned long pos = 0) const;

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

        /** get NumberOfLateralSpreadingDevices (300a,0330)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfLateralSpreadingDevices(OFString &value, const signed long pos = 0) const;

        /** get NumberOfLateralSpreadingDevices (300a,0330)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfLateralSpreadingDevices(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfRangeModulators (300a,0340)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfRangeModulators(OFString &value, const signed long pos = 0) const;

        /** get NumberOfRangeModulators (300a,0340)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfRangeModulators(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfRangeShifters (300a,0312)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfRangeShifters(OFString &value, const signed long pos = 0) const;

        /** get NumberOfRangeShifters (300a,0312)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfRangeShifters(Sint32 &value, const unsigned long pos = 0) const;

        /** get NumberOfWedges (300a,00d0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfWedges(OFString &value, const signed long pos = 0) const;

        /** get NumberOfWedges (300a,00d0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfWedges(Sint32 &value, const unsigned long pos = 0) const;

        /** get PatientSupportAccessoryCode (300a,0354)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportAccessoryCode(OFString &value, const signed long pos = 0) const;

        /** get PatientSupportID (300a,0352)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportID(OFString &value, const signed long pos = 0) const;

        /** get PatientSupportType (300a,0350)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPatientSupportType(OFString &value, const signed long pos = 0) const;

        /** get RadiationAtomicNumber (300a,0304)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationAtomicNumber(OFString &value, const signed long pos = 0) const;

        /** get RadiationAtomicNumber (300a,0304)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationAtomicNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get RadiationChargeState (300a,0306)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationChargeState(Sint16 &value, const unsigned long pos = 0) const;

        /** get RadiationMassNumber (300a,0302)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationMassNumber(OFString &value, const signed long pos = 0) const;

        /** get RadiationMassNumber (300a,0302)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationMassNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get RadiationType (300a,00c6)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRadiationType(OFString &value, const signed long pos = 0) const;

        /** get ReferencedBeamNumber (300c,0006)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedBeamNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedBeamNumber (300c,0006)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedBeamNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ReferencedPatientSetupNumber (300c,006a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedPatientSetupNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedPatientSetupNumber (300c,006a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedPatientSetupNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ReferencedToleranceTableNumber (300c,00a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedToleranceTableNumber(OFString &value, const signed long pos = 0) const;

        /** get ReferencedToleranceTableNumber (300c,00a0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getReferencedToleranceTableNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get ScanMode (300a,0308)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getScanMode(OFString &value, const signed long pos = 0) const;

        /** get SpecifiedPrimaryMeterset (3008,0032)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedPrimaryMeterset(OFString &value, const signed long pos = 0) const;

        /** get SpecifiedPrimaryMeterset (3008,0032)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedPrimaryMeterset(Float64 &value, const unsigned long pos = 0) const;

        /** get SpecifiedSecondaryMeterset (3008,0033)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedSecondaryMeterset(OFString &value, const signed long pos = 0) const;

        /** get SpecifiedSecondaryMeterset (3008,0033)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedSecondaryMeterset(Float64 &value, const unsigned long pos = 0) const;

        /** get SpecifiedTreatmentTime (3008,003a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedTreatmentTime(OFString &value, const signed long pos = 0) const;

        /** get SpecifiedTreatmentTime (3008,003a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSpecifiedTreatmentTime(Float64 &value, const unsigned long pos = 0) const;

        /** get TreatmentDeliveryType (300a,00ce)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentDeliveryType(OFString &value, const signed long pos = 0) const;

        /** get TreatmentTerminationCode (3008,002b)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentTerminationCode(OFString &value, const signed long pos = 0) const;

        /** get TreatmentTerminationStatus (3008,002a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentTerminationStatus(OFString &value, const signed long pos = 0) const;

        /** get TreatmentVerificationStatus (3008,002c)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentVerificationStatus(OFString &value, const signed long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ApplicatorSequence (300a,0107)
         *  @return reference to sequence element
         */
        DRTApplicatorSequenceInRTIonBeamsSessionRecordModule &getApplicatorSequence()
            { return ApplicatorSequence; }

        /** get ApplicatorSequence (300a,0107)
         *  @return const reference to sequence element
         */
        const DRTApplicatorSequenceInRTIonBeamsSessionRecordModule &getApplicatorSequence() const
            { return ApplicatorSequence; }

        /** get BeamLimitingDeviceLeafPairsSequence (3008,00a0)
         *  @return reference to sequence element
         */
        DRTBeamLimitingDeviceLeafPairsSequence &getBeamLimitingDeviceLeafPairsSequence()
            { return BeamLimitingDeviceLeafPairsSequence; }

        /** get BeamLimitingDeviceLeafPairsSequence (3008,00a0)
         *  @return const reference to sequence element
         */
        const DRTBeamLimitingDeviceLeafPairsSequence &getBeamLimitingDeviceLeafPairsSequence() const
            { return BeamLimitingDeviceLeafPairsSequence; }

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

        /** get IonControlPointDeliverySequence (3008,0041)
         *  @return reference to sequence element
         */
        DRTIonControlPointDeliverySequence &getIonControlPointDeliverySequence()
            { return IonControlPointDeliverySequence; }

        /** get IonControlPointDeliverySequence (3008,0041)
         *  @return const reference to sequence element
         */
        const DRTIonControlPointDeliverySequence &getIonControlPointDeliverySequence() const
            { return IonControlPointDeliverySequence; }

        /** get RecordedBlockSequence (3008,00d0)
         *  @return reference to sequence element
         */
        DRTRecordedBlockSequence &getRecordedBlockSequence()
            { return RecordedBlockSequence; }

        /** get RecordedBlockSequence (3008,00d0)
         *  @return const reference to sequence element
         */
        const DRTRecordedBlockSequence &getRecordedBlockSequence() const
            { return RecordedBlockSequence; }

        /** get RecordedCompensatorSequence (3008,00c0)
         *  @return reference to sequence element
         */
        DRTRecordedCompensatorSequence &getRecordedCompensatorSequence()
            { return RecordedCompensatorSequence; }

        /** get RecordedCompensatorSequence (3008,00c0)
         *  @return const reference to sequence element
         */
        const DRTRecordedCompensatorSequence &getRecordedCompensatorSequence() const
            { return RecordedCompensatorSequence; }

        /** get RecordedLateralSpreadingDeviceSequence (3008,00f4)
         *  @return reference to sequence element
         */
        DRTRecordedLateralSpreadingDeviceSequence &getRecordedLateralSpreadingDeviceSequence()
            { return RecordedLateralSpreadingDeviceSequence; }

        /** get RecordedLateralSpreadingDeviceSequence (3008,00f4)
         *  @return const reference to sequence element
         */
        const DRTRecordedLateralSpreadingDeviceSequence &getRecordedLateralSpreadingDeviceSequence() const
            { return RecordedLateralSpreadingDeviceSequence; }

        /** get RecordedRangeModulatorSequence (3008,00f6)
         *  @return reference to sequence element
         */
        DRTRecordedRangeModulatorSequence &getRecordedRangeModulatorSequence()
            { return RecordedRangeModulatorSequence; }

        /** get RecordedRangeModulatorSequence (3008,00f6)
         *  @return const reference to sequence element
         */
        const DRTRecordedRangeModulatorSequence &getRecordedRangeModulatorSequence() const
            { return RecordedRangeModulatorSequence; }

        /** get RecordedRangeShifterSequence (3008,00f2)
         *  @return reference to sequence element
         */
        DRTRecordedRangeShifterSequence &getRecordedRangeShifterSequence()
            { return RecordedRangeShifterSequence; }

        /** get RecordedRangeShifterSequence (3008,00f2)
         *  @return const reference to sequence element
         */
        const DRTRecordedRangeShifterSequence &getRecordedRangeShifterSequence() const
            { return RecordedRangeShifterSequence; }

        /** get RecordedSnoutSequence (3008,00f0)
         *  @return reference to sequence element
         */
        DRTRecordedSnoutSequence &getRecordedSnoutSequence()
            { return RecordedSnoutSequence; }

        /** get RecordedSnoutSequence (3008,00f0)
         *  @return const reference to sequence element
         */
        const DRTRecordedSnoutSequence &getRecordedSnoutSequence() const
            { return RecordedSnoutSequence; }

        /** get RecordedWedgeSequence (3008,00b0)
         *  @return reference to sequence element
         */
        DRTRecordedWedgeSequence &getRecordedWedgeSequence()
            { return RecordedWedgeSequence; }

        /** get RecordedWedgeSequence (3008,00b0)
         *  @return const reference to sequence element
         */
        const DRTRecordedWedgeSequence &getRecordedWedgeSequence() const
            { return RecordedWedgeSequence; }

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return reference to sequence element
         */
        DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule &getReferencedBolusSequence()
            { return ReferencedBolusSequence; }

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return const reference to sequence element
         */
        const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule &getReferencedBolusSequence() const
            { return ReferencedBolusSequence; }

        /** get ReferencedCalculatedDoseReferenceSequence (3008,0090)
         *  @return reference to sequence element
         */
        DRTReferencedCalculatedDoseReferenceSequence &getReferencedCalculatedDoseReferenceSequence()
            { return ReferencedCalculatedDoseReferenceSequence; }

        /** get ReferencedCalculatedDoseReferenceSequence (3008,0090)
         *  @return const reference to sequence element
         */
        const DRTReferencedCalculatedDoseReferenceSequence &getReferencedCalculatedDoseReferenceSequence() const
            { return ReferencedCalculatedDoseReferenceSequence; }

        /** get ReferencedMeasuredDoseReferenceSequence (3008,0080)
         *  @return reference to sequence element
         */
        DRTReferencedMeasuredDoseReferenceSequence &getReferencedMeasuredDoseReferenceSequence()
            { return ReferencedMeasuredDoseReferenceSequence; }

        /** get ReferencedMeasuredDoseReferenceSequence (3008,0080)
         *  @return const reference to sequence element
         */
        const DRTReferencedMeasuredDoseReferenceSequence &getReferencedMeasuredDoseReferenceSequence() const
            { return ReferencedMeasuredDoseReferenceSequence; }

        /** get ReferencedVerificationImageSequence (300c,0040)
         *  @return reference to sequence element
         */
        DRTReferencedVerificationImageSequence &getReferencedVerificationImageSequence()
            { return ReferencedVerificationImageSequence; }

        /** get ReferencedVerificationImageSequence (300c,0040)
         *  @return const reference to sequence element
         */
        const DRTReferencedVerificationImageSequence &getReferencedVerificationImageSequence() const
            { return ReferencedVerificationImageSequence; }

      // --- set DICOM attribute values ---

        /** set BeamDescription (300a,00c3)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamDescription(const OFString &value, const OFBool check = OFTrue);

        /** set BeamName (300a,00c2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamName(const OFString &value, const OFBool check = OFTrue);

        /** set BeamType (300a,00c4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamType(const OFString &value, const OFBool check = OFTrue);

        /** set CurrentFractionNumber (3008,0022)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCurrentFractionNumber(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveredPrimaryMeterset (3008,0036)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveredPrimaryMeterset(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveredSecondaryMeterset (3008,0037)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveredSecondaryMeterset(const OFString &value, const OFBool check = OFTrue);

        /** set DeliveredTreatmentTime (3008,003b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeliveredTreatmentTime(const OFString &value, const OFBool check = OFTrue);

        /** set FixationLightAzimuthalAngle (300a,0356)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFixationLightAzimuthalAngle(const Float32 value, const unsigned long pos = 0);

        /** set FixationLightPolarAngle (300a,0358)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFixationLightPolarAngle(const Float32 value, const unsigned long pos = 0);

        /** set NumberOfBlocks (300a,00f0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBlocks(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfBoli (300a,00ed)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBoli(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfCompensators (300a,00e0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfCompensators(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfControlPoints (300a,0110)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfControlPoints(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfLateralSpreadingDevices (300a,0330)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfLateralSpreadingDevices(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfRangeModulators (300a,0340)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfRangeModulators(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfRangeShifters (300a,0312)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfRangeShifters(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfWedges (300a,00d0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfWedges(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportAccessoryCode (300a,0354)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportAccessoryCode(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportID (300a,0352)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportID(const OFString &value, const OFBool check = OFTrue);

        /** set PatientSupportType (300a,0350)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPatientSupportType(const OFString &value, const OFBool check = OFTrue);

        /** set RadiationAtomicNumber (300a,0304)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRadiationAtomicNumber(const OFString &value, const OFBool check = OFTrue);

        /** set RadiationChargeState (300a,0306)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRadiationChargeState(const Sint16 value, const unsigned long pos = 0);

        /** set RadiationMassNumber (300a,0302)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRadiationMassNumber(const OFString &value, const OFBool check = OFTrue);

        /** set RadiationType (300a,00c6)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRadiationType(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedBeamNumber (300c,0006)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedBeamNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedPatientSetupNumber (300c,006a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedPatientSetupNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ReferencedToleranceTableNumber (300c,00a0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setReferencedToleranceTableNumber(const OFString &value, const OFBool check = OFTrue);

        /** set ScanMode (300a,0308)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setScanMode(const OFString &value, const OFBool check = OFTrue);

        /** set SpecifiedPrimaryMeterset (3008,0032)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSpecifiedPrimaryMeterset(const OFString &value, const OFBool check = OFTrue);

        /** set SpecifiedSecondaryMeterset (3008,0033)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSpecifiedSecondaryMeterset(const OFString &value, const OFBool check = OFTrue);

        /** set SpecifiedTreatmentTime (3008,003a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSpecifiedTreatmentTime(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentDeliveryType (300a,00ce)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentDeliveryType(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentTerminationCode (3008,002b)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentTerminationCode(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentTerminationStatus (3008,002a)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentTerminationStatus(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentVerificationStatus (3008,002c)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentVerificationStatus(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ApplicatorSequence (300a,0107) vr=SQ, vm=1, type=1C
        DRTApplicatorSequenceInRTIonBeamsSessionRecordModule ApplicatorSequence;
        /// BeamDescription (300a,00c3) vr=ST, vm=1, type=3
        DcmShortText BeamDescription;
        /// BeamLimitingDeviceLeafPairsSequence (3008,00a0) vr=SQ, vm=1, type=3
        DRTBeamLimitingDeviceLeafPairsSequence BeamLimitingDeviceLeafPairsSequence;
        /// BeamName (300a,00c2) vr=LO, vm=1, type=1
        DcmLongString BeamName;
        /// BeamType (300a,00c4) vr=CS, vm=1, type=1
        DcmCodeString BeamType;
        /// CurrentFractionNumber (3008,0022) vr=IS, vm=1, type=2
        DcmIntegerString CurrentFractionNumber;
        /// DeliveredPrimaryMeterset (3008,0036) vr=DS, vm=1, type=3
        DcmDecimalString DeliveredPrimaryMeterset;
        /// DeliveredSecondaryMeterset (3008,0037) vr=DS, vm=1, type=3
        DcmDecimalString DeliveredSecondaryMeterset;
        /// DeliveredTreatmentTime (3008,003b) vr=DS, vm=1, type=3
        DcmDecimalString DeliveredTreatmentTime;
        /// FixationLightAzimuthalAngle (300a,0356) vr=FL, vm=1, type=3
        DcmFloatingPointSingle FixationLightAzimuthalAngle;
        /// FixationLightPolarAngle (300a,0358) vr=FL, vm=1, type=3
        DcmFloatingPointSingle FixationLightPolarAngle;
        /// GeneralAccessorySequence (300a,0420) vr=SQ, vm=1, type=3
        DRTGeneralAccessorySequence GeneralAccessorySequence;
        /// IonControlPointDeliverySequence (3008,0041) vr=SQ, vm=1, type=1
        DRTIonControlPointDeliverySequence IonControlPointDeliverySequence;
        /// NumberOfBlocks (300a,00f0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfBlocks;
        /// NumberOfBoli (300a,00ed) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfBoli;
        /// NumberOfCompensators (300a,00e0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfCompensators;
        /// NumberOfControlPoints (300a,0110) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfControlPoints;
        /// NumberOfLateralSpreadingDevices (300a,0330) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfLateralSpreadingDevices;
        /// NumberOfRangeModulators (300a,0340) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfRangeModulators;
        /// NumberOfRangeShifters (300a,0312) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfRangeShifters;
        /// NumberOfWedges (300a,00d0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfWedges;
        /// PatientSupportAccessoryCode (300a,0354) vr=LO, vm=1, type=3
        DcmLongString PatientSupportAccessoryCode;
        /// PatientSupportID (300a,0352) vr=SH, vm=1, type=3
        DcmShortString PatientSupportID;
        /// PatientSupportType (300a,0350) vr=CS, vm=1, type=1
        DcmCodeString PatientSupportType;
        /// RadiationAtomicNumber (300a,0304) vr=IS, vm=1, type=1C
        DcmIntegerString RadiationAtomicNumber;
        /// RadiationChargeState (300a,0306) vr=SS, vm=1, type=1C
        DcmSignedShort RadiationChargeState;
        /// RadiationMassNumber (300a,0302) vr=IS, vm=1, type=1C
        DcmIntegerString RadiationMassNumber;
        /// RadiationType (300a,00c6) vr=CS, vm=1, type=1
        DcmCodeString RadiationType;
        /// RecordedBlockSequence (3008,00d0) vr=SQ, vm=1, type=1C
        DRTRecordedBlockSequence RecordedBlockSequence;
        /// RecordedCompensatorSequence (3008,00c0) vr=SQ, vm=1, type=1C
        DRTRecordedCompensatorSequence RecordedCompensatorSequence;
        /// RecordedLateralSpreadingDeviceSequence (3008,00f4) vr=SQ, vm=1, type=1C
        DRTRecordedLateralSpreadingDeviceSequence RecordedLateralSpreadingDeviceSequence;
        /// RecordedRangeModulatorSequence (3008,00f6) vr=SQ, vm=1, type=1C
        DRTRecordedRangeModulatorSequence RecordedRangeModulatorSequence;
        /// RecordedRangeShifterSequence (3008,00f2) vr=SQ, vm=1, type=1C
        DRTRecordedRangeShifterSequence RecordedRangeShifterSequence;
        /// RecordedSnoutSequence (3008,00f0) vr=SQ, vm=1, type=1C
        DRTRecordedSnoutSequence RecordedSnoutSequence;
        /// RecordedWedgeSequence (3008,00b0) vr=SQ, vm=1, type=1C
        DRTRecordedWedgeSequence RecordedWedgeSequence;
        /// ReferencedBeamNumber (300c,0006) vr=IS, vm=1, type=1
        DcmIntegerString ReferencedBeamNumber;
        /// ReferencedBolusSequence (300c,00b0) vr=SQ, vm=1, type=1C
        DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule ReferencedBolusSequence;
        /// ReferencedCalculatedDoseReferenceSequence (3008,0090) vr=SQ, vm=1, type=3
        DRTReferencedCalculatedDoseReferenceSequence ReferencedCalculatedDoseReferenceSequence;
        /// ReferencedMeasuredDoseReferenceSequence (3008,0080) vr=SQ, vm=1, type=3
        DRTReferencedMeasuredDoseReferenceSequence ReferencedMeasuredDoseReferenceSequence;
        /// ReferencedPatientSetupNumber (300c,006a) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedPatientSetupNumber;
        /// ReferencedToleranceTableNumber (300c,00a0) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedToleranceTableNumber;
        /// ReferencedVerificationImageSequence (300c,0040) vr=SQ, vm=1, type=3
        DRTReferencedVerificationImageSequence ReferencedVerificationImageSequence;
        /// ScanMode (300a,0308) vr=CS, vm=1, type=1
        DcmCodeString ScanMode;
        /// SpecifiedPrimaryMeterset (3008,0032) vr=DS, vm=1, type=3
        DcmDecimalString SpecifiedPrimaryMeterset;
        /// SpecifiedSecondaryMeterset (3008,0033) vr=DS, vm=1, type=3
        DcmDecimalString SpecifiedSecondaryMeterset;
        /// SpecifiedTreatmentTime (3008,003a) vr=DS, vm=1, type=3
        DcmDecimalString SpecifiedTreatmentTime;
        /// TreatmentDeliveryType (300a,00ce) vr=CS, vm=1, type=2
        DcmCodeString TreatmentDeliveryType;
        /// TreatmentTerminationCode (3008,002b) vr=SH, vm=1, type=3
        DcmShortString TreatmentTerminationCode;
        /// TreatmentTerminationStatus (3008,002a) vr=CS, vm=1, type=1
        DcmCodeString TreatmentTerminationStatus;
        /// TreatmentVerificationStatus (3008,002c) vr=CS, vm=1, type=2
        DcmCodeString TreatmentVerificationStatus;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTTreatmentSessionIonBeamSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTTreatmentSessionIonBeamSequence(const DRTTreatmentSessionIonBeamSequence &copy);

    /** destructor
     */
    virtual ~DRTTreatmentSessionIonBeamSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTTreatmentSessionIonBeamSequence &operator=(const DRTTreatmentSessionIonBeamSequence &copy);

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
