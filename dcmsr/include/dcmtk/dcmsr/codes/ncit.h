/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with NCI Thesaurus Code Definitions (Coding Scheme "NCIt")
 *
 *  Generated automatically from DICOM PS 3.16-2022b
 *  File created on 2022-04-11 09:59:32 by J. Riesmeier
 *
 */


#ifndef CODES_NCIT_H
#define CODES_NCIT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_NCIt_CodingSchemeDesignator              "NCIt"
#define CODE_NCIt_CodingSchemeName                    "NCI Thesaurus"
#define CODE_NCIt_CodingSchemeDescription             "NCI Thesaurus"
#define CODE_NCIt_CodingSchemeResponsibleOrganization "NCI"
#define CODE_NCIt_CodingSchemeUID                     "2.16.840.1.113883.3.26.1.1"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// total number of codes: 49
// - retired: 0
// - no name: 0
// - not unique: 0

// The basic scheme for creating the names is as follows:
//   'CODE_' + <coding-scheme-designator> + ['_RETIRED'] + '_' + <code-name>
// where <coding-scheme-designator> is "NCIt" and <code-name> is either
// - a cleaned camel-case version of the code meaning (if unique within this coding scheme) or
// - a cleaned camel-case version of the code meaning with an underscore and the code value appended or
// - simply the code value with an underscore character appended (if code meaning contains an equation).
// The "cleaned camel-case version of the code meaning" is an attempt to map the free text
// description of the code meaning to a compiler-friendly but still human-readable representation.

#define CODE_NCIt_IndexLesion                                              DSRBasicCodedEntry("C110961", "NCIt", "Index lesion")
#define CODE_NCIt_EnhancingLesion                                          DSRBasicCodedEntry("C113842", "NCIt", "Enhancing Lesion")
#define CODE_NCIt_RANO                                                     DSRBasicCodedEntry("C114879", "NCIt", "RANO")
#define CODE_NCIt_DCFPyL_F18                                               DSRBasicCodedEntry("C116352", "NCIt", "DCFPyL F^18^")
#define CODE_NCIt_Technetium99mTrofolastat                                 DSRBasicCodedEntry("C116887", "NCIt", "Technetium^99m Trofolastat")
#define CODE_NCIt_PSMA11_Ga68                                              DSRBasicCodedEntry("C118961", "NCIt", "PSMA-11 Ga^68^")
#define CODE_NCIt_Sarcosine_C11                                            DSRBasicCodedEntry("C122684", "NCIt", "Sarcosine C^11^")
#define CODE_NCIt_IrreversibleElectroporation                              DSRBasicCodedEntry("C131483", "NCIt", "Irreversible electroporation")
#define CODE_NCIt_4Kscore                                                  DSRBasicCodedEntry("C142184", "NCIt", "4Kscore")
#define CODE_NCIt_SoftTissueSarcoma_excludingRhabdomyosarcoma              DSRBasicCodedEntry("C148457", "NCIt", "Soft tissue sarcoma, excluding rhabdomyosarcoma")
#define CODE_NCIt_Cryoablation                                             DSRBasicCodedEntry("C15215", "NCIt", "Cryoablation")
#define CODE_NCIt_166Holmium                                               DSRBasicCodedEntry("C1943", "NCIt", "^166^Holmium")
#define CODE_NCIt_MouseMammaryFatPad                                       DSRBasicCodedEntry("C22550", "NCIt", "Mouse mammary fat pad")
#define CODE_NCIt_Middle                                                   DSRBasicCodedEntry("C25569", "NCIt", "Middle")
#define CODE_NCIt_Reader                                                   DSRBasicCodedEntry("C28747", "NCIt", "Reader")
#define CODE_NCIt_BronchioloalveolarAdenocarcinoma                         DSRBasicCodedEntry("C2923", "NCIt", "Bronchioloalveolar adenocarcinoma")
#define CODE_NCIt_ClinicalCourseOfDisease                                  DSRBasicCodedEntry("C35461", "NCIt", "Clinical course of disease")
#define CODE_NCIt_Enrollment                                               DSRBasicCodedEntry("C37948", "NCIt", "Enrollment")
#define CODE_NCIt_RecurrentDisease                                         DSRBasicCodedEntry("C38155", "NCIt", "Recurrent Disease")
#define CODE_NCIt_ExtraluminalRoute                                        DSRBasicCodedEntry("C38213", "NCIt", "Extraluminal route")
#define CODE_NCIt_IntraepithelialRoute                                     DSRBasicCodedEntry("C38244", "NCIt", "Intraepithelial route")
#define CODE_NCIt_TransluminalRoute                                        DSRBasicCodedEntry("C38306", "NCIt", "Transluminal route")
#define CODE_NCIt_NoEvidenceOfDisease                                      DSRBasicCodedEntry("C40413", "NCIt", "No Evidence of Disease")
#define CODE_NCIt_NativeHawaiianOrOtherPacificIslander                     DSRBasicCodedEntry("C41219", "NCIt", "Native Hawaiian or other Pacific Islander")
#define CODE_NCIt_AdverseEvent                                             DSRBasicCodedEntry("C41331", "NCIt", "Adverse Event")
#define CODE_NCIt_TemperatureSensorDeviceComponent                         DSRBasicCodedEntry("C50304", "NCIt", "Temperature sensor device component")
#define CODE_NCIt_Experience                                               DSRBasicCodedEntry("C54627", "NCIt", "Experience")
#define CODE_NCIt_Reviewer                                                 DSRBasicCodedEntry("C54634", "NCIt", "Reviewer")
#define CODE_NCIt_Ion                                                      DSRBasicCodedEntry("C597", "NCIt", "Ion")
#define CODE_NCIt_AdLibitum                                                DSRBasicCodedEntry("C64636", "NCIt", "ad libitum")
#define CODE_NCIt_ActivitySession                                          DSRBasicCodedEntry("C67447", "NCIt", "Activity Session")
#define CODE_NCIt_FocusedUltrasoundAblation                                DSRBasicCodedEntry("C68681", "NCIt", "Focused ultrasound ablation")
#define CODE_NCIt_UnitConversionFactor                                     DSRBasicCodedEntry("C70774", "NCIt", "Unit Conversion Factor")
#define CODE_NCIt_MedicalProductExpirationDate                             DSRBasicCodedEntry("C70854", "NCIt", "Medical Product Expiration Date")
#define CODE_NCIt_CardiotonicAgent                                         DSRBasicCodedEntry("C78322", "NCIt", "Cardiotonic agent")
#define CODE_NCIt_NonEnhancingLesion                                       DSRBasicCodedEntry("C81175", "NCIt", "Non-Enhancing Lesion")
#define CODE_NCIt_Erect                                                    DSRBasicCodedEntry("C86043", "NCIt", "erect")
#define CODE_NCIt_BeddingChange                                            DSRBasicCodedEntry("C90365", "NCIt", "Bedding change")
#define CODE_NCIt_BeddingMaterial                                          DSRBasicCodedEntry("C90366", "NCIt", "Bedding material")
#define CODE_NCIt_EnvironmentalTemperature                                 DSRBasicCodedEntry("C90380", "NCIt", "Environmental temperature")
#define CODE_NCIt_HousingHumidity                                          DSRBasicCodedEntry("C90395", "NCIt", "Housing humidity")
#define CODE_NCIt_LightCycle                                               DSRBasicCodedEntry("C90419", "NCIt", "Light cycle")
#define CODE_NCIt_WaterDelivery                                            DSRBasicCodedEntry("C90486", "NCIt", "Water delivery")
#define CODE_NCIt_Dosimetrist                                              DSRBasicCodedEntry("C93176", "NCIt", "Dosimetrist")
#define CODE_NCIt_Abnormality                                              DSRBasicCodedEntry("C9440", "NCIt", "Abnormality")
#define CODE_NCIt_ReferenceRegion                                          DSRBasicCodedEntry("C94970", "NCIt", "Reference Region")
#define CODE_NCIt_DCFBC_F18                                                DSRBasicCodedEntry("C96234", "NCIt", "DCFBC F^18^")
#define CODE_NCIt_Adjudicator                                              DSRBasicCodedEntry("C96561", "NCIt", "Adjudicator")
#define CODE_NCIt_Fibrate                                                  DSRBasicCodedEntry("C98150", "NCIt", "Fibrate")

#endif
