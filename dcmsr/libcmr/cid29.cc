/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID29_AcquisitionModality
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:14 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid29.h"


// general information on CID 29 (Acquisition Modality)
#define CONTEXT_GROUP_NUMBER  "29"
#define CONTEXT_GROUP_VERSION "20180605"
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
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
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
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
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
        Codes->insert(OFMake_pair(Autorefraction, DSRBasicCodedEntry("AR", "DCM", "Autorefraction")));
        Codes->insert(OFMake_pair(BoneMineralDensitometry, DSRBasicCodedEntry("BMD", "DCM", "Bone Mineral Densitometry")));
        Codes->insert(OFMake_pair(UltrasoundBoneDensitometry, DSRBasicCodedEntry("BDUS", "DCM", "Ultrasound Bone Densitometry")));
        Codes->insert(OFMake_pair(CardiacElectrophysiology, DSRBasicCodedEntry("EPS", "DCM", "Cardiac Electrophysiology")));
        Codes->insert(OFMake_pair(ComputedRadiography, DSRBasicCodedEntry("CR", "DCM", "Computed Radiography")));
        Codes->insert(OFMake_pair(ComputedTomography, DSRBasicCodedEntry("CT", "DCM", "Computed Tomography")));
        Codes->insert(OFMake_pair(DigitalRadiography, DSRBasicCodedEntry("DX", "DCM", "Digital Radiography")));
        Codes->insert(OFMake_pair(Electrocardiography, DSRBasicCodedEntry("ECG", "DCM", "Electrocardiography")));
        Codes->insert(OFMake_pair(Endoscopy, DSRBasicCodedEntry("ES", "DCM", "Endoscopy")));
        Codes->insert(OFMake_pair(ExternalCameraPhotography, DSRBasicCodedEntry("XC", "DCM", "External-camera Photography")));
        Codes->insert(OFMake_pair(GeneralMicroscopy, DSRBasicCodedEntry("GM", "DCM", "General Microscopy")));
        Codes->insert(OFMake_pair(HemodynamicWaveform, DSRBasicCodedEntry("HD", "DCM", "Hemodynamic Waveform")));
        Codes->insert(OFMake_pair(IntraOralRadiography, DSRBasicCodedEntry("IO", "DCM", "Intra-oral Radiography")));
        Codes->insert(OFMake_pair(IntravascularOpticalCoherenceTomography, DSRBasicCodedEntry("IVOCT", "DCM", "Intravascular Optical Coherence Tomography")));
        Codes->insert(OFMake_pair(IntravascularUltrasound, DSRBasicCodedEntry("IVUS", "DCM", "Intravascular Ultrasound")));
        Codes->insert(OFMake_pair(Keratometry, DSRBasicCodedEntry("KER", "DCM", "Keratometry")));
        Codes->insert(OFMake_pair(Lensometry, DSRBasicCodedEntry("LEN", "DCM", "Lensometry")));
        Codes->insert(OFMake_pair(MagneticResonance, DSRBasicCodedEntry("MR", "DCM", "Magnetic Resonance")));
        Codes->insert(OFMake_pair(Mammography, DSRBasicCodedEntry("MG", "DCM", "Mammography")));
        Codes->insert(OFMake_pair(NuclearMedicine, DSRBasicCodedEntry("NM", "DCM", "Nuclear Medicine")));
        Codes->insert(OFMake_pair(OphthalmicAxialMeasurements, DSRBasicCodedEntry("OAM", "DCM", "Ophthalmic Axial Measurements")));
        Codes->insert(OFMake_pair(OpticalCoherenceTomography, DSRBasicCodedEntry("OCT", "DCM", "Optical Coherence Tomography")));
        Codes->insert(OFMake_pair(OphthalmicMapping, DSRBasicCodedEntry("OPM", "DCM", "Ophthalmic Mapping")));
        Codes->insert(OFMake_pair(OphthalmicPhotography, DSRBasicCodedEntry("OP", "DCM", "Ophthalmic Photography")));
        Codes->insert(OFMake_pair(OphthalmicRefraction, DSRBasicCodedEntry("OPR", "DCM", "Ophthalmic Refraction")));
        Codes->insert(OFMake_pair(OphthalmicTomography, DSRBasicCodedEntry("OPT", "DCM", "Ophthalmic Tomography")));
        Codes->insert(OFMake_pair(OphthalmicTomographyBScanVolumeAnalysis, DSRBasicCodedEntry("OPTBSV", "DCM", "Ophthalmic Tomography B-scan Volume Analysis")));
        Codes->insert(OFMake_pair(OphthalmicTomographyEnFace, DSRBasicCodedEntry("OPTENF", "DCM", "Ophthalmic Tomography En Face")));
        Codes->insert(OFMake_pair(OphthalmicVisualField, DSRBasicCodedEntry("OPV", "DCM", "Ophthalmic Visual Field")));
        Codes->insert(OFMake_pair(OpticalSurfaceScanner, DSRBasicCodedEntry("OSS", "DCM", "Optical Surface Scanner")));
        Codes->insert(OFMake_pair(PanoramicXRay, DSRBasicCodedEntry("PX", "DCM", "Panoramic X-Ray")));
        Codes->insert(OFMake_pair(PositronEmissionTomography, DSRBasicCodedEntry("PT", "DCM", "Positron emission tomography")));
        Codes->insert(OFMake_pair(Radiofluoroscopy, DSRBasicCodedEntry("RF", "DCM", "Radiofluoroscopy")));
        Codes->insert(OFMake_pair(RadiographicImaging, DSRBasicCodedEntry("RG", "DCM", "Radiographic imaging")));
        Codes->insert(OFMake_pair(SlideMicroscopy, DSRBasicCodedEntry("SM", "DCM", "Slide Microscopy")));
        Codes->insert(OFMake_pair(SubjectiveRefraction, DSRBasicCodedEntry("SRF", "DCM", "Subjective Refraction")));
        Codes->insert(OFMake_pair(Ultrasound, DSRBasicCodedEntry("US", "DCM", "Ultrasound")));
        Codes->insert(OFMake_pair(VisualAcuity, DSRBasicCodedEntry("VA", "DCM", "Visual Acuity")));
        Codes->insert(OFMake_pair(XRayAngiography, DSRBasicCodedEntry("XA", "DCM", "X-Ray Angiography")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID29_AcquisitionModality::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
