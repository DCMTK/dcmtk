/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTIBS_H
#define DRTIBS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtas6.h"    // for ApplicatorSequence
#include "dcmtk/dcmrt/seq/drtgas.h"    // for GeneralAccessorySequence
#include "dcmtk/dcmrt/seq/drtiblds.h"  // for IonBeamLimitingDeviceSequence
#include "dcmtk/dcmrt/seq/drtibls.h"   // for IonBlockSequence
#include "dcmtk/dcmrt/seq/drticps.h"   // for IonControlPointSequence
#include "dcmtk/dcmrt/seq/drtircs.h"   // for IonRangeCompensatorSequence
#include "dcmtk/dcmrt/seq/drtiws.h"    // for IonWedgeSequence
#include "dcmtk/dcmrt/seq/drtlsds.h"   // for LateralSpreadingDeviceSequence
#include "dcmtk/dcmrt/seq/drtrms.h"    // for RangeModulatorSequence
#include "dcmtk/dcmrt/seq/drtrshs.h"   // for RangeShifterSequence
#include "dcmtk/dcmrt/seq/drtrbos6.h"  // for ReferencedBolusSequence
#include "dcmtk/dcmrt/seq/drtrds.h"    // for ReferencedDoseSequence
#include "dcmtk/dcmrt/seq/drtrris6.h"  // for ReferencedReferenceImageSequence
#include "dcmtk/dcmrt/seq/drtsns.h"    // for SnoutSequence


/** Interface class for IonBeamSequence (300a,03a2) in RTIonBeamsModule
 */
class DCMTK_DCMRT_EXPORT DRTIonBeamSequence
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

        /** get BeamNumber (300a,00c0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamNumber(OFString &value, const signed long pos = 0) const;

        /** get BeamNumber (300a,00c0)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get BeamType (300a,00c4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBeamType(OFString &value, const signed long pos = 0) const;

        /** get DeviceSerialNumber (0018,1000)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDeviceSerialNumber(OFString &value, const signed long pos = 0) const;

        /** get FinalCumulativeMetersetWeight (300a,010e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFinalCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

        /** get FinalCumulativeMetersetWeight (300a,010e)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getFinalCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

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

        /** get InstitutionAddress (0008,0081)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionAddress(OFString &value, const signed long pos = 0) const;

        /** get InstitutionName (0008,0080)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionName(OFString &value, const signed long pos = 0) const;

        /** get InstitutionalDepartmentName (0008,1040)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getInstitutionalDepartmentName(OFString &value, const signed long pos = 0) const;

        /** get Manufacturer (0008,0070)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturer(OFString &value, const signed long pos = 0) const;

        /** get ManufacturerModelName (0008,1090)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getManufacturerModelName(OFString &value, const signed long pos = 0) const;

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

        /** get PrimaryDosimeterUnit (300a,00b3)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPrimaryDosimeterUnit(OFString &value, const signed long pos = 0) const;

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

        /** get TotalBlockTrayWaterEquivalentThickness (300a,00f3)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalBlockTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos = 0) const;

        /** get TotalCompensatorTrayWaterEquivalentThickness (300a,02e3)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalCompensatorTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos = 0) const;

        /** get TotalWedgeTrayWaterEquivalentThickness (300a,00d7)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalWedgeTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos = 0) const;

        /** get TreatmentDeliveryType (300a,00ce)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentDeliveryType(OFString &value, const signed long pos = 0) const;

        /** get TreatmentMachineName (300a,00b2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTreatmentMachineName(OFString &value, const signed long pos = 0) const;

        /** get VirtualSourceAxisDistances (300a,030a)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getVirtualSourceAxisDistances(Float32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get ApplicatorSequence (300a,0107)
         *  @return reference to sequence element
         */
        DRTApplicatorSequenceInRTIonBeamsModule &getApplicatorSequence()
            { return ApplicatorSequence; }

        /** get ApplicatorSequence (300a,0107)
         *  @return const reference to sequence element
         */
        const DRTApplicatorSequenceInRTIonBeamsModule &getApplicatorSequence() const
            { return ApplicatorSequence; }

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

        /** get IonBeamLimitingDeviceSequence (300a,03a4)
         *  @return reference to sequence element
         */
        DRTIonBeamLimitingDeviceSequence &getIonBeamLimitingDeviceSequence()
            { return IonBeamLimitingDeviceSequence; }

        /** get IonBeamLimitingDeviceSequence (300a,03a4)
         *  @return const reference to sequence element
         */
        const DRTIonBeamLimitingDeviceSequence &getIonBeamLimitingDeviceSequence() const
            { return IonBeamLimitingDeviceSequence; }

        /** get IonBlockSequence (300a,03a6)
         *  @return reference to sequence element
         */
        DRTIonBlockSequence &getIonBlockSequence()
            { return IonBlockSequence; }

        /** get IonBlockSequence (300a,03a6)
         *  @return const reference to sequence element
         */
        const DRTIonBlockSequence &getIonBlockSequence() const
            { return IonBlockSequence; }

        /** get IonControlPointSequence (300a,03a8)
         *  @return reference to sequence element
         */
        DRTIonControlPointSequence &getIonControlPointSequence()
            { return IonControlPointSequence; }

        /** get IonControlPointSequence (300a,03a8)
         *  @return const reference to sequence element
         */
        const DRTIonControlPointSequence &getIonControlPointSequence() const
            { return IonControlPointSequence; }

        /** get IonRangeCompensatorSequence (300a,02ea)
         *  @return reference to sequence element
         */
        DRTIonRangeCompensatorSequence &getIonRangeCompensatorSequence()
            { return IonRangeCompensatorSequence; }

        /** get IonRangeCompensatorSequence (300a,02ea)
         *  @return const reference to sequence element
         */
        const DRTIonRangeCompensatorSequence &getIonRangeCompensatorSequence() const
            { return IonRangeCompensatorSequence; }

        /** get IonWedgeSequence (300a,03aa)
         *  @return reference to sequence element
         */
        DRTIonWedgeSequence &getIonWedgeSequence()
            { return IonWedgeSequence; }

        /** get IonWedgeSequence (300a,03aa)
         *  @return const reference to sequence element
         */
        const DRTIonWedgeSequence &getIonWedgeSequence() const
            { return IonWedgeSequence; }

        /** get LateralSpreadingDeviceSequence (300a,0332)
         *  @return reference to sequence element
         */
        DRTLateralSpreadingDeviceSequence &getLateralSpreadingDeviceSequence()
            { return LateralSpreadingDeviceSequence; }

        /** get LateralSpreadingDeviceSequence (300a,0332)
         *  @return const reference to sequence element
         */
        const DRTLateralSpreadingDeviceSequence &getLateralSpreadingDeviceSequence() const
            { return LateralSpreadingDeviceSequence; }

        /** get RangeModulatorSequence (300a,0342)
         *  @return reference to sequence element
         */
        DRTRangeModulatorSequence &getRangeModulatorSequence()
            { return RangeModulatorSequence; }

        /** get RangeModulatorSequence (300a,0342)
         *  @return const reference to sequence element
         */
        const DRTRangeModulatorSequence &getRangeModulatorSequence() const
            { return RangeModulatorSequence; }

        /** get RangeShifterSequence (300a,0314)
         *  @return reference to sequence element
         */
        DRTRangeShifterSequence &getRangeShifterSequence()
            { return RangeShifterSequence; }

        /** get RangeShifterSequence (300a,0314)
         *  @return const reference to sequence element
         */
        const DRTRangeShifterSequence &getRangeShifterSequence() const
            { return RangeShifterSequence; }

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return reference to sequence element
         */
        DRTReferencedBolusSequenceInRTIonBeamsModule &getReferencedBolusSequence()
            { return ReferencedBolusSequence; }

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return const reference to sequence element
         */
        const DRTReferencedBolusSequenceInRTIonBeamsModule &getReferencedBolusSequence() const
            { return ReferencedBolusSequence; }

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

        /** get ReferencedReferenceImageSequence (300c,0042)
         *  @return reference to sequence element
         */
        DRTReferencedReferenceImageSequenceInRTIonBeamsModule &getReferencedReferenceImageSequence()
            { return ReferencedReferenceImageSequence; }

        /** get ReferencedReferenceImageSequence (300c,0042)
         *  @return const reference to sequence element
         */
        const DRTReferencedReferenceImageSequenceInRTIonBeamsModule &getReferencedReferenceImageSequence() const
            { return ReferencedReferenceImageSequence; }

        /** get SnoutSequence (300a,030c)
         *  @return reference to sequence element
         */
        DRTSnoutSequence &getSnoutSequence()
            { return SnoutSequence; }

        /** get SnoutSequence (300a,030c)
         *  @return const reference to sequence element
         */
        const DRTSnoutSequence &getSnoutSequence() const
            { return SnoutSequence; }

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

        /** set BeamNumber (300a,00c0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamNumber(const OFString &value, const OFBool check = OFTrue);

        /** set BeamType (300a,00c4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBeamType(const OFString &value, const OFBool check = OFTrue);

        /** set DeviceSerialNumber (0018,1000)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDeviceSerialNumber(const OFString &value, const OFBool check = OFTrue);

        /** set FinalCumulativeMetersetWeight (300a,010e)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setFinalCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

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

        /** set InstitutionAddress (0008,0081)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionAddress(const OFString &value, const OFBool check = OFTrue);

        /** set InstitutionName (0008,0080)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionName(const OFString &value, const OFBool check = OFTrue);

        /** set InstitutionalDepartmentName (0008,1040)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setInstitutionalDepartmentName(const OFString &value, const OFBool check = OFTrue);

        /** set Manufacturer (0008,0070)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturer(const OFString &value, const OFBool check = OFTrue);

        /** set ManufacturerModelName (0008,1090)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setManufacturerModelName(const OFString &value, const OFBool check = OFTrue);

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

        /** set PrimaryDosimeterUnit (300a,00b3)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPrimaryDosimeterUnit(const OFString &value, const OFBool check = OFTrue);

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

        /** set TotalBlockTrayWaterEquivalentThickness (300a,00f3)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalBlockTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos = 0);

        /** set TotalCompensatorTrayWaterEquivalentThickness (300a,02e3)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalCompensatorTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos = 0);

        /** set TotalWedgeTrayWaterEquivalentThickness (300a,00d7)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalWedgeTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos = 0);

        /** set TreatmentDeliveryType (300a,00ce)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentDeliveryType(const OFString &value, const OFBool check = OFTrue);

        /** set TreatmentMachineName (300a,00b2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTreatmentMachineName(const OFString &value, const OFBool check = OFTrue);

        /** set VirtualSourceAxisDistances (300a,030a)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=2
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setVirtualSourceAxisDistances(const Float32 value, const unsigned long pos = 0);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ApplicatorSequence (300a,0107) vr=SQ, vm=1, type=3
        DRTApplicatorSequenceInRTIonBeamsModule ApplicatorSequence;
        /// BeamDescription (300a,00c3) vr=ST, vm=1, type=3
        DcmShortText BeamDescription;
        /// BeamName (300a,00c2) vr=LO, vm=1, type=1
        DcmLongString BeamName;
        /// BeamNumber (300a,00c0) vr=IS, vm=1, type=1
        DcmIntegerString BeamNumber;
        /// BeamType (300a,00c4) vr=CS, vm=1, type=1
        DcmCodeString BeamType;
        /// DeviceSerialNumber (0018,1000) vr=LO, vm=1, type=3
        DcmLongString DeviceSerialNumber;
        /// FinalCumulativeMetersetWeight (300a,010e) vr=DS, vm=1, type=1C
        DcmDecimalString FinalCumulativeMetersetWeight;
        /// FixationLightAzimuthalAngle (300a,0356) vr=FL, vm=1, type=3
        DcmFloatingPointSingle FixationLightAzimuthalAngle;
        /// FixationLightPolarAngle (300a,0358) vr=FL, vm=1, type=3
        DcmFloatingPointSingle FixationLightPolarAngle;
        /// GeneralAccessorySequence (300a,0420) vr=SQ, vm=1, type=3
        DRTGeneralAccessorySequence GeneralAccessorySequence;
        /// InstitutionAddress (0008,0081) vr=ST, vm=1, type=3
        DcmShortText InstitutionAddress;
        /// InstitutionName (0008,0080) vr=LO, vm=1, type=3
        DcmLongString InstitutionName;
        /// InstitutionalDepartmentName (0008,1040) vr=LO, vm=1, type=3
        DcmLongString InstitutionalDepartmentName;
        /// IonBeamLimitingDeviceSequence (300a,03a4) vr=SQ, vm=1, type=3
        DRTIonBeamLimitingDeviceSequence IonBeamLimitingDeviceSequence;
        /// IonBlockSequence (300a,03a6) vr=SQ, vm=1, type=1C
        DRTIonBlockSequence IonBlockSequence;
        /// IonControlPointSequence (300a,03a8) vr=SQ, vm=1, type=1
        DRTIonControlPointSequence IonControlPointSequence;
        /// IonRangeCompensatorSequence (300a,02ea) vr=SQ, vm=1, type=1C
        DRTIonRangeCompensatorSequence IonRangeCompensatorSequence;
        /// IonWedgeSequence (300a,03aa) vr=SQ, vm=1, type=1C
        DRTIonWedgeSequence IonWedgeSequence;
        /// LateralSpreadingDeviceSequence (300a,0332) vr=SQ, vm=1, type=1C
        DRTLateralSpreadingDeviceSequence LateralSpreadingDeviceSequence;
        /// Manufacturer (0008,0070) vr=LO, vm=1, type=3
        DcmLongString Manufacturer;
        /// ManufacturerModelName (0008,1090) vr=LO, vm=1, type=3
        DcmLongString ManufacturerModelName;
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
        /// PrimaryDosimeterUnit (300a,00b3) vr=CS, vm=1, type=1
        DcmCodeString PrimaryDosimeterUnit;
        /// RadiationAtomicNumber (300a,0304) vr=IS, vm=1, type=1C
        DcmIntegerString RadiationAtomicNumber;
        /// RadiationChargeState (300a,0306) vr=SS, vm=1, type=1C
        DcmSignedShort RadiationChargeState;
        /// RadiationMassNumber (300a,0302) vr=IS, vm=1, type=1C
        DcmIntegerString RadiationMassNumber;
        /// RadiationType (300a,00c6) vr=CS, vm=1, type=1
        DcmCodeString RadiationType;
        /// RangeModulatorSequence (300a,0342) vr=SQ, vm=1, type=1C
        DRTRangeModulatorSequence RangeModulatorSequence;
        /// RangeShifterSequence (300a,0314) vr=SQ, vm=1, type=1C
        DRTRangeShifterSequence RangeShifterSequence;
        /// ReferencedBolusSequence (300c,00b0) vr=SQ, vm=1, type=1C
        DRTReferencedBolusSequenceInRTIonBeamsModule ReferencedBolusSequence;
        /// ReferencedDoseSequence (300c,0080) vr=SQ, vm=1, type=3
        DRTReferencedDoseSequence ReferencedDoseSequence;
        /// ReferencedPatientSetupNumber (300c,006a) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedPatientSetupNumber;
        /// ReferencedReferenceImageSequence (300c,0042) vr=SQ, vm=1, type=3
        DRTReferencedReferenceImageSequenceInRTIonBeamsModule ReferencedReferenceImageSequence;
        /// ReferencedToleranceTableNumber (300c,00a0) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedToleranceTableNumber;
        /// ScanMode (300a,0308) vr=CS, vm=1, type=1
        DcmCodeString ScanMode;
        /// SnoutSequence (300a,030c) vr=SQ, vm=1, type=3
        DRTSnoutSequence SnoutSequence;
        /// TotalBlockTrayWaterEquivalentThickness (300a,00f3) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TotalBlockTrayWaterEquivalentThickness;
        /// TotalCompensatorTrayWaterEquivalentThickness (300a,02e3) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TotalCompensatorTrayWaterEquivalentThickness;
        /// TotalWedgeTrayWaterEquivalentThickness (300a,00d7) vr=FL, vm=1, type=3
        DcmFloatingPointSingle TotalWedgeTrayWaterEquivalentThickness;
        /// TreatmentDeliveryType (300a,00ce) vr=CS, vm=1, type=1
        DcmCodeString TreatmentDeliveryType;
        /// TreatmentMachineName (300a,00b2) vr=SH, vm=1, type=2
        DcmShortString TreatmentMachineName;
        /// VirtualSourceAxisDistances (300a,030a) vr=FL, vm=2, type=1
        DcmFloatingPointSingle VirtualSourceAxisDistances;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIonBeamSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIonBeamSequence(const DRTIonBeamSequence &copy);

    /** destructor
     */
    virtual ~DRTIonBeamSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTIonBeamSequence &operator=(const DRTIonBeamSequence &copy);

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
