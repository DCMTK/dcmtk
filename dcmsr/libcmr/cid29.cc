/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: CID29_AcquisitionModality
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-08-18 17:27:27 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid29.h"


// general information on CID 29 (Acquisition Modality)
#define CONTEXT_GROUP_NUMBER  "29"
#define CONTEXT_GROUP_VERSION "20121129"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.19"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


CID29_AcquisitionModality::CID29_AcquisitionModality(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID29_AcquisitionModality::CID29_AcquisitionModality(const EnumType selectedValue,
                                                     const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID29_AcquisitionModality::selectValue(const EnumType selectedValue,
                                                   const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID29_AcquisitionModality::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                                      DSRCodedEntryValue *foundCodedEntry,
                                                      const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (searchForCodedEntry == iter->second)
        {
            /* return coded entry (if requested) */
            if (foundCodedEntry != NULL)
            {
                *foundCodedEntry = iter->second;
                /* also set enhanced encoding mode (if enabled) */
                if (!foundCodedEntry->isEmpty() && enhancedEncodingMode)
                    foundCodedEntry->setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
            }
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
    {
        result = DSRContextGroup::findCodedEntry(searchForCodedEntry, foundCodedEntry);
        /* tbd: set "enhanced encoding mode" to mark a local/extended version? */
    }
    return result;
}


void CID29_AcquisitionModality::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        stream << "  ";
        /* print coded entry */
        DSRCodedEntryValue(iter->second).print(stream);
        stream << OFendl;
        ++iter;
    }
    /* print extended codes */
    DSRContextGroup::printCodes(stream);
}


// static functions

void CID29_AcquisitionModality::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID29_AcquisitionModality::getCodedEntry(const EnumType value,
                                                            const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* search for given enumerated value */
    CodeList::iterator iter = getCodes().find(value);
    /* if found, set the coded entry */
    if (iter != getCodes().end())
    {
        codedEntry = iter->second;
        /* also set enhanced encoding mode (if enabled) */
        if (!codedEntry.isEmpty() && enhancedEncodingMode)
            codedEntry.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
    }
    return codedEntry;
}


CID29_AcquisitionModality::CodeList &CID29_AcquisitionModality::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Autorefraction, DSRBasicCodedEntry("AR", "DCM", "Autorefraction")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(BoneMineralDensitometry, DSRBasicCodedEntry("BMD", "DCM", "Bone Mineral Densitometry")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UltrasoundBoneDensitometry, DSRBasicCodedEntry("BDUS", "DCM", "Ultrasound Bone Densitometry")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CardiacElectrophysiology, DSRBasicCodedEntry("EPS", "DCM", "Cardiac Electrophysiology")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ComputedRadiography, DSRBasicCodedEntry("CR", "DCM", "Computed Radiography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ComputedTomography, DSRBasicCodedEntry("CT", "DCM", "Computed Tomography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(DigitalRadiography, DSRBasicCodedEntry("DX", "DCM", "Digital Radiography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Electrocardiography, DSRBasicCodedEntry("ECG", "DCM", "Electrocardiography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Endoscopy, DSRBasicCodedEntry("ES", "DCM", "Endoscopy")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ExternalCameraPhotography, DSRBasicCodedEntry("XC", "DCM", "External-camera Photography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(GeneralMicroscopy, DSRBasicCodedEntry("GM", "DCM", "General Microscopy")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(HemodynamicWaveform, DSRBasicCodedEntry("HD", "DCM", "Hemodynamic Waveform")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntraOralRadiography, DSRBasicCodedEntry("IO", "DCM", "Intra-oral Radiography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntravascularOpticalCoherenceTomography, DSRBasicCodedEntry("IVOCT", "DCM", "Intravascular Optical Coherence Tomography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntravascularUltrasound, DSRBasicCodedEntry("IVUS", "DCM", "Intravascular Ultrasound")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Keratometry, DSRBasicCodedEntry("KER", "DCM", "Keratometry")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Lensometry, DSRBasicCodedEntry("LEN", "DCM", "Lensometry")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MagneticResonance, DSRBasicCodedEntry("MR", "DCM", "Magnetic Resonance")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Mammography, DSRBasicCodedEntry("MG", "DCM", "Mammography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NuclearMedicine, DSRBasicCodedEntry("NM", "DCM", "Nuclear Medicine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicAxialMeasurements, DSRBasicCodedEntry("OAM", "DCM", "Ophthalmic Axial Measurements")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OpticalCoherenceTomography, DSRBasicCodedEntry("OCT", "DCM", "Optical Coherence Tomography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicMapping, DSRBasicCodedEntry("OPM", "DCM", "Ophthalmic Mapping")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicPhotography, DSRBasicCodedEntry("OP", "DCM", "Ophthalmic Photography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicRefraction, DSRBasicCodedEntry("OPR", "DCM", "Ophthalmic Refraction")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicTomography, DSRBasicCodedEntry("OPT", "DCM", "Ophthalmic Tomography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicVisualField, DSRBasicCodedEntry("OPV", "DCM", "Ophthalmic Visual Field")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OpticalSurfaceScanner, DSRBasicCodedEntry("OSS", "DCM", "Optical Surface Scanner")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PanoramicXRay, DSRBasicCodedEntry("PX", "DCM", "Panoramic X-Ray")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PositronEmissionTomography, DSRBasicCodedEntry("PT", "DCM", "Positron emission tomography")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Radiofluoroscopy, DSRBasicCodedEntry("RF", "DCM", "Radiofluoroscopy")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(RadiographicImaging, DSRBasicCodedEntry("RG", "DCM", "Radiographic imaging")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SlideMicroscopy, DSRBasicCodedEntry("SM", "DCM", "Slide Microscopy")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SubjectiveRefraction, DSRBasicCodedEntry("SRF", "DCM", "Subjective Refraction")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Ultrasound, DSRBasicCodedEntry("US", "DCM", "Ultrasound")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(VisualAcuity, DSRBasicCodedEntry("VA", "DCM", "Visual Acuity")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(XRayAngiography, DSRBasicCodedEntry("XA", "DCM", "X-Ray Angiography")));
    }
    /* should never be NULL */
    return *codes;
}
