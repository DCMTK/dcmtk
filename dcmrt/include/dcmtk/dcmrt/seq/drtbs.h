/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTBS_H
#define DRTBS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtas1.h"    // for ApplicatorSequence
#include "dcmtk/dcmrt/seq/drtblds1.h"  // for BeamLimitingDeviceSequence
#include "dcmtk/dcmrt/seq/drtbl2.h"    // for BlockSequence
#include "dcmtk/dcmrt/seq/drtcos.h"    // for CompensatorSequence
#include "dcmtk/dcmrt/seq/drtcps.h"    // for ControlPointSequence
#include "dcmtk/dcmrt/seq/drtgas.h"    // for GeneralAccessorySequence
#include "dcmtk/dcmrt/seq/drtpvis.h"   // for PlannedVerificationImageSequence
#include "dcmtk/dcmrt/seq/drtpfms.h"   // for PrimaryFluenceModeSequence
#include "dcmtk/dcmrt/seq/drtrbos1.h"  // for ReferencedBolusSequence
#include "dcmtk/dcmrt/seq/drtrds.h"    // for ReferencedDoseSequence
#include "dcmtk/dcmrt/seq/drtrris1.h"  // for ReferencedReferenceImageSequence
#include "dcmtk/dcmrt/seq/drtws.h"     // for WedgeSequence


/** Interface class for BeamSequence (300a,00b0) in RTBeamsModule
 */
class DCMTK_DCMRT_EXPORT DRTBeamSequence
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

        /** get HighDoseTechniqueType (300a,00c7)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getHighDoseTechniqueType(OFString &value, const signed long pos = 0) const;

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

        /** get PrimaryDosimeterUnit (300a,00b3)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getPrimaryDosimeterUnit(OFString &value, const signed long pos = 0) const;

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

        /** get SourceAxisDistance (300a,00b4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceAxisDistance(OFString &value, const signed long pos = 0) const;

        /** get SourceAxisDistance (300a,00b4)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSourceAxisDistance(Float64 &value, const unsigned long pos = 0) const;

        /** get TotalBlockTrayFactor (300a,00f2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalBlockTrayFactor(OFString &value, const signed long pos = 0) const;

        /** get TotalBlockTrayFactor (300a,00f2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalBlockTrayFactor(Float64 &value, const unsigned long pos = 0) const;

        /** get TotalCompensatorTrayFactor (300a,00e2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalCompensatorTrayFactor(OFString &value, const signed long pos = 0) const;

        /** get TotalCompensatorTrayFactor (300a,00e2)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getTotalCompensatorTrayFactor(Float64 &value, const unsigned long pos = 0) const;

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

      // --- get DICOM sequence attributes ---

        /** get ApplicatorSequence (300a,0107)
         *  @return reference to sequence element
         */
        DRTApplicatorSequenceInRTBeamsModule &getApplicatorSequence()
            { return ApplicatorSequence; }

        /** get ApplicatorSequence (300a,0107)
         *  @return const reference to sequence element
         */
        const DRTApplicatorSequenceInRTBeamsModule &getApplicatorSequence() const
            { return ApplicatorSequence; }

        /** get BeamLimitingDeviceSequence (300a,00b6)
         *  @return reference to sequence element
         */
        DRTBeamLimitingDeviceSequenceInRTBeamsModule &getBeamLimitingDeviceSequence()
            { return BeamLimitingDeviceSequence; }

        /** get BeamLimitingDeviceSequence (300a,00b6)
         *  @return const reference to sequence element
         */
        const DRTBeamLimitingDeviceSequenceInRTBeamsModule &getBeamLimitingDeviceSequence() const
            { return BeamLimitingDeviceSequence; }

        /** get BlockSequence (300a,00f4)
         *  @return reference to sequence element
         */
        DRTBlockSequenceInRTBeamsModule &getBlockSequence()
            { return BlockSequence; }

        /** get BlockSequence (300a,00f4)
         *  @return const reference to sequence element
         */
        const DRTBlockSequenceInRTBeamsModule &getBlockSequence() const
            { return BlockSequence; }

        /** get CompensatorSequence (300a,00e3)
         *  @return reference to sequence element
         */
        DRTCompensatorSequence &getCompensatorSequence()
            { return CompensatorSequence; }

        /** get CompensatorSequence (300a,00e3)
         *  @return const reference to sequence element
         */
        const DRTCompensatorSequence &getCompensatorSequence() const
            { return CompensatorSequence; }

        /** get ControlPointSequence (300a,0111)
         *  @return reference to sequence element
         */
        DRTControlPointSequence &getControlPointSequence()
            { return ControlPointSequence; }

        /** get ControlPointSequence (300a,0111)
         *  @return const reference to sequence element
         */
        const DRTControlPointSequence &getControlPointSequence() const
            { return ControlPointSequence; }

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

        /** get PlannedVerificationImageSequence (300a,00ca)
         *  @return reference to sequence element
         */
        DRTPlannedVerificationImageSequence &getPlannedVerificationImageSequence()
            { return PlannedVerificationImageSequence; }

        /** get PlannedVerificationImageSequence (300a,00ca)
         *  @return const reference to sequence element
         */
        const DRTPlannedVerificationImageSequence &getPlannedVerificationImageSequence() const
            { return PlannedVerificationImageSequence; }

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

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return reference to sequence element
         */
        DRTReferencedBolusSequenceInRTBeamsModule &getReferencedBolusSequence()
            { return ReferencedBolusSequence; }

        /** get ReferencedBolusSequence (300c,00b0)
         *  @return const reference to sequence element
         */
        const DRTReferencedBolusSequenceInRTBeamsModule &getReferencedBolusSequence() const
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
        DRTReferencedReferenceImageSequenceInRTBeamsModule &getReferencedReferenceImageSequence()
            { return ReferencedReferenceImageSequence; }

        /** get ReferencedReferenceImageSequence (300c,0042)
         *  @return const reference to sequence element
         */
        const DRTReferencedReferenceImageSequenceInRTBeamsModule &getReferencedReferenceImageSequence() const
            { return ReferencedReferenceImageSequence; }

        /** get WedgeSequence (300a,00d1)
         *  @return reference to sequence element
         */
        DRTWedgeSequence &getWedgeSequence()
            { return WedgeSequence; }

        /** get WedgeSequence (300a,00d1)
         *  @return const reference to sequence element
         */
        const DRTWedgeSequence &getWedgeSequence() const
            { return WedgeSequence; }

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

        /** set HighDoseTechniqueType (300a,00c7)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setHighDoseTechniqueType(const OFString &value, const OFBool check = OFTrue);

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

        /** set NumberOfWedges (300a,00d0)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfWedges(const OFString &value, const OFBool check = OFTrue);

        /** set PrimaryDosimeterUnit (300a,00b3)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setPrimaryDosimeterUnit(const OFString &value, const OFBool check = OFTrue);

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

        /** set SourceAxisDistance (300a,00b4)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSourceAxisDistance(const OFString &value, const OFBool check = OFTrue);

        /** set TotalBlockTrayFactor (300a,00f2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalBlockTrayFactor(const OFString &value, const OFBool check = OFTrue);

        /** set TotalCompensatorTrayFactor (300a,00e2)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setTotalCompensatorTrayFactor(const OFString &value, const OFBool check = OFTrue);

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

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// ApplicatorSequence (300a,0107) vr=SQ, vm=1, type=3
        DRTApplicatorSequenceInRTBeamsModule ApplicatorSequence;
        /// BeamDescription (300a,00c3) vr=ST, vm=1, type=3
        DcmShortText BeamDescription;
        /// BeamLimitingDeviceSequence (300a,00b6) vr=SQ, vm=1, type=1
        DRTBeamLimitingDeviceSequenceInRTBeamsModule BeamLimitingDeviceSequence;
        /// BeamName (300a,00c2) vr=LO, vm=1, type=3
        DcmLongString BeamName;
        /// BeamNumber (300a,00c0) vr=IS, vm=1, type=1
        DcmIntegerString BeamNumber;
        /// BeamType (300a,00c4) vr=CS, vm=1, type=1
        DcmCodeString BeamType;
        /// BlockSequence (300a,00f4) vr=SQ, vm=1, type=1C
        DRTBlockSequenceInRTBeamsModule BlockSequence;
        /// CompensatorSequence (300a,00e3) vr=SQ, vm=1, type=1C
        DRTCompensatorSequence CompensatorSequence;
        /// ControlPointSequence (300a,0111) vr=SQ, vm=1, type=1
        DRTControlPointSequence ControlPointSequence;
        /// DeviceSerialNumber (0018,1000) vr=LO, vm=1, type=3
        DcmLongString DeviceSerialNumber;
        /// FinalCumulativeMetersetWeight (300a,010e) vr=DS, vm=1, type=1C
        DcmDecimalString FinalCumulativeMetersetWeight;
        /// GeneralAccessorySequence (300a,0420) vr=SQ, vm=1, type=3
        DRTGeneralAccessorySequence GeneralAccessorySequence;
        /// HighDoseTechniqueType (300a,00c7) vr=CS, vm=1, type=1C
        DcmCodeString HighDoseTechniqueType;
        /// InstitutionAddress (0008,0081) vr=ST, vm=1, type=3
        DcmShortText InstitutionAddress;
        /// InstitutionName (0008,0080) vr=LO, vm=1, type=3
        DcmLongString InstitutionName;
        /// InstitutionalDepartmentName (0008,1040) vr=LO, vm=1, type=3
        DcmLongString InstitutionalDepartmentName;
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
        /// NumberOfWedges (300a,00d0) vr=IS, vm=1, type=1
        DcmIntegerString NumberOfWedges;
        /// PlannedVerificationImageSequence (300a,00ca) vr=SQ, vm=1, type=3
        DRTPlannedVerificationImageSequence PlannedVerificationImageSequence;
        /// PrimaryDosimeterUnit (300a,00b3) vr=CS, vm=1, type=3
        DcmCodeString PrimaryDosimeterUnit;
        /// PrimaryFluenceModeSequence (3002,0050) vr=SQ, vm=1, type=3
        DRTPrimaryFluenceModeSequence PrimaryFluenceModeSequence;
        /// RadiationType (300a,00c6) vr=CS, vm=1, type=2
        DcmCodeString RadiationType;
        /// ReferencedBolusSequence (300c,00b0) vr=SQ, vm=1, type=1C
        DRTReferencedBolusSequenceInRTBeamsModule ReferencedBolusSequence;
        /// ReferencedDoseSequence (300c,0080) vr=SQ, vm=1, type=3
        DRTReferencedDoseSequence ReferencedDoseSequence;
        /// ReferencedPatientSetupNumber (300c,006a) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedPatientSetupNumber;
        /// ReferencedReferenceImageSequence (300c,0042) vr=SQ, vm=1, type=3
        DRTReferencedReferenceImageSequenceInRTBeamsModule ReferencedReferenceImageSequence;
        /// ReferencedToleranceTableNumber (300c,00a0) vr=IS, vm=1, type=3
        DcmIntegerString ReferencedToleranceTableNumber;
        /// SourceAxisDistance (300a,00b4) vr=DS, vm=1, type=3
        DcmDecimalString SourceAxisDistance;
        /// TotalBlockTrayFactor (300a,00f2) vr=DS, vm=1, type=3
        DcmDecimalString TotalBlockTrayFactor;
        /// TotalCompensatorTrayFactor (300a,00e2) vr=DS, vm=1, type=3
        DcmDecimalString TotalCompensatorTrayFactor;
        /// TreatmentDeliveryType (300a,00ce) vr=CS, vm=1, type=3
        DcmCodeString TreatmentDeliveryType;
        /// TreatmentMachineName (300a,00b2) vr=SH, vm=1, type=2
        DcmShortString TreatmentMachineName;
        /// WedgeSequence (300a,00d1) vr=SQ, vm=1, type=1C
        DRTWedgeSequence WedgeSequence;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTBeamSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTBeamSequence(const DRTBeamSequence &copy);

    /** destructor
     */
    virtual ~DRTBeamSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTBeamSequence &operator=(const DRTBeamSequence &copy);

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
