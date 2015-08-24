/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID29_AcquisitionModality
 *
 *  Generated automatically from DICOM PS 3.16-2015c
 *  File created on 2015-08-23 15:24:58 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid29.h"


// general information on CID 29 (Acquisition Modality)
#define CONTEXT_GROUP_NUMBER  "29"
#define CONTEXT_GROUP_VERSION "20121129"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.19"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID29_AcquisitionModality::CodeList *CID29_AcquisitionModality::Codes = NULL;


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
                    setEnhancedEncodingMode(*foundCodedEntry);
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


void CID29_AcquisitionModality::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
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
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID29_AcquisitionModality::CodeList &CID29_AcquisitionModality::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Autorefraction, DSRBasicCodedEntry("AR", "DCM", "Autorefraction")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(BoneMineralDensitometry, DSRBasicCodedEntry("BMD", "DCM", "Bone Mineral Densitometry")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UltrasoundBoneDensitometry, DSRBasicCodedEntry("BDUS", "DCM", "Ultrasound Bone Densitometry")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CardiacElectrophysiology, DSRBasicCodedEntry("EPS", "DCM", "Cardiac Electrophysiology")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ComputedRadiography, DSRBasicCodedEntry("CR", "DCM", "Computed Radiography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ComputedTomography, DSRBasicCodedEntry("CT", "DCM", "Computed Tomography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(DigitalRadiography, DSRBasicCodedEntry("DX", "DCM", "Digital Radiography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Electrocardiography, DSRBasicCodedEntry("ECG", "DCM", "Electrocardiography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Endoscopy, DSRBasicCodedEntry("ES", "DCM", "Endoscopy")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ExternalCameraPhotography, DSRBasicCodedEntry("XC", "DCM", "External-camera Photography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(GeneralMicroscopy, DSRBasicCodedEntry("GM", "DCM", "General Microscopy")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(HemodynamicWaveform, DSRBasicCodedEntry("HD", "DCM", "Hemodynamic Waveform")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntraOralRadiography, DSRBasicCodedEntry("IO", "DCM", "Intra-oral Radiography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntravascularOpticalCoherenceTomography, DSRBasicCodedEntry("IVOCT", "DCM", "Intravascular Optical Coherence Tomography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IntravascularUltrasound, DSRBasicCodedEntry("IVUS", "DCM", "Intravascular Ultrasound")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Keratometry, DSRBasicCodedEntry("KER", "DCM", "Keratometry")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Lensometry, DSRBasicCodedEntry("LEN", "DCM", "Lensometry")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MagneticResonance, DSRBasicCodedEntry("MR", "DCM", "Magnetic Resonance")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Mammography, DSRBasicCodedEntry("MG", "DCM", "Mammography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NuclearMedicine, DSRBasicCodedEntry("NM", "DCM", "Nuclear Medicine")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicAxialMeasurements, DSRBasicCodedEntry("OAM", "DCM", "Ophthalmic Axial Measurements")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OpticalCoherenceTomography, DSRBasicCodedEntry("OCT", "DCM", "Optical Coherence Tomography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicMapping, DSRBasicCodedEntry("OPM", "DCM", "Ophthalmic Mapping")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicPhotography, DSRBasicCodedEntry("OP", "DCM", "Ophthalmic Photography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicRefraction, DSRBasicCodedEntry("OPR", "DCM", "Ophthalmic Refraction")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicTomography, DSRBasicCodedEntry("OPT", "DCM", "Ophthalmic Tomography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OphthalmicVisualField, DSRBasicCodedEntry("OPV", "DCM", "Ophthalmic Visual Field")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OpticalSurfaceScanner, DSRBasicCodedEntry("OSS", "DCM", "Optical Surface Scanner")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PanoramicXRay, DSRBasicCodedEntry("PX", "DCM", "Panoramic X-Ray")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PositronEmissionTomography, DSRBasicCodedEntry("PT", "DCM", "Positron emission tomography")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Radiofluoroscopy, DSRBasicCodedEntry("RF", "DCM", "Radiofluoroscopy")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(RadiographicImaging, DSRBasicCodedEntry("RG", "DCM", "Radiographic imaging")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SlideMicroscopy, DSRBasicCodedEntry("SM", "DCM", "Slide Microscopy")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SubjectiveRefraction, DSRBasicCodedEntry("SRF", "DCM", "Subjective Refraction")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Ultrasound, DSRBasicCodedEntry("US", "DCM", "Ultrasound")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(VisualAcuity, DSRBasicCodedEntry("VA", "DCM", "Visual Acuity")));
        Codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(XRayAngiography, DSRBasicCodedEntry("XA", "DCM", "X-Ray Angiography")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID29_AcquisitionModality::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
