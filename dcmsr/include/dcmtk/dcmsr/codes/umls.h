/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with UMLS Code Definitions (Coding Scheme "UMLS")
 *
 *  Generated automatically from DICOM PS 3.16-2016b
 *  File created on 2016-04-06 16:08:00 by J. Riesmeier
 *
 */


#ifndef CODES_UMLS_H
#define CODES_UMLS_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_UMLS_CodingSchemeDesignator  "UMLS"
#define CODE_UMLS_CodingSchemeName        "UMLS"
#define CODE_UMLS_CodingSchemeDescription "UMLS codes as CUIs making up the values in a coding system"
#define CODE_UMLS_CodingSchemeUID         "2.16.840.1.113883.6.86"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// total number of codes: 39
// - retired: 0
// - no name: 0
// - not unique: 0

// The basic scheme for creating the names is as follows:
//   'CODE_' + <coding-scheme-designator> + ['_RETIRED'] + '_' + <code-name>
// where <coding-scheme-designator> is "UMLS" and <code-name> is either
// - a cleaned camel-case version of the code meaning (if unique within this coding scheme) or
// - a cleaned camel-case version of the code meaning with an underscore and the code value appended or
// - simply the code value with an underscore character appended (if code meaning contains an equation).
// The "cleaned camel-case version of the code meaning" is an attempt to map the free text
// description of the code meaning to a compiler-friendly but still human-readable representation.

#define CODE_UMLS_AnesthesiaRecoveryPeriod                                 DSRBasicCodedEntry("C0002908", "UMLS", "Anesthesia recovery period")
#define CODE_UMLS_TransgenicAnimal                                         DSRBasicCodedEntry("C0003069", "UMLS", "Transgenic animal")
#define CODE_UMLS_FeedingMethod                                            DSRBasicCodedEntry("C0015746", "UMLS", "Feeding method")
#define CODE_UMLS_Heating                                                  DSRBasicCodedEntry("C0018851", "UMLS", "Heating")
#define CODE_UMLS_H2Antagonist                                             DSRBasicCodedEntry("C0019593", "UMLS", "H2 antagonist")
#define CODE_UMLS_EthidiumBromide                                          DSRBasicCodedEntry("C0019873", "UMLS", "Ethidium Bromide")
#define CODE_UMLS_Mastectomy                                               DSRBasicCodedEntry("C0024881", "UMLS", "Mastectomy")
#define CODE_UMLS_Metomidate                                               DSRBasicCodedEntry("C0025856", "UMLS", "Metomidate")
#define CODE_UMLS_NarcoticAnalgesic                                        DSRBasicCodedEntry("C0027409", "UMLS", "Narcotic analgesic")
#define CODE_UMLS_QualitativeEvaluations                                   DSRBasicCodedEntry("C0034375", "UMLS", "Qualitative Evaluations")
#define CODE_UMLS_Thermography                                             DSRBasicCodedEntry("C0039810", "UMLS", "Thermography")
#define CODE_UMLS_Alphaxalone                                              DSRBasicCodedEntry("C0051482", "UMLS", "Alphaxalone")
#define CODE_UMLS_Fluanisone                                               DSRBasicCodedEntry("C0060473", "UMLS", "Fluanisone")
#define CODE_UMLS_HeatLamp                                                 DSRBasicCodedEntry("C0181514", "UMLS", "Heat lamp")
#define CODE_UMLS_UnknownPrimaryNeoplasiaSite                              DSRBasicCodedEntry("C0221297", "UMLS", "unknown primary neoplasia site")
#define CODE_UMLS_CoefficientOfVariance                                    DSRBasicCodedEntry("C0681921", "UMLS", "Coefficient of Variance")
#define CODE_UMLS_Intern                                                   DSRBasicCodedEntry("C1144859", "UMLS", "Intern")
#define CODE_UMLS_ConsultingPhysician                                      DSRBasicCodedEntry("C1441532", "UMLS", "Consulting Physician")
#define CODE_UMLS_Baseline                                                 DSRBasicCodedEntry("C1442488", "UMLS", "Baseline")
#define CODE_UMLS_Fluoroetanidazole_F18                                    DSRBasicCodedEntry("C1541539", "UMLS", "Fluoroetanidazole F^18^")
#define CODE_UMLS_CommercialProduct                                        DSRBasicCodedEntry("C1547887", "UMLS", "Commercial product")
#define CODE_UMLS_Antihypoglycemic                                         DSRBasicCodedEntry("C1579431", "UMLS", "Antihypoglycemic")
#define CODE_UMLS_Unscheduled                                              DSRBasicCodedEntry("C1699701", "UMLS", "Unscheduled")
#define CODE_UMLS_Nadir                                                    DSRBasicCodedEntry("C1708760", "UMLS", "Nadir")
#define CODE_UMLS_MedicalPhysicist                                         DSRBasicCodedEntry("C1708969", "UMLS", "Medical Physicist")
#define CODE_UMLS_Referring                                                DSRBasicCodedEntry("C1709880", "UMLS", "Referring")
#define CODE_UMLS_Variance                                                 DSRBasicCodedEntry("C1711260", "UMLS", "Variance")
#define CODE_UMLS_Tyrosine3Octreotate_Ga68                                 DSRBasicCodedEntry("C1742831", "UMLS", "tyrosine-3-octreotate Ga^68^")
#define CODE_UMLS_Fluoroestradiol_FES_F18                                  DSRBasicCodedEntry("C1831937", "UMLS", "Fluoroestradiol (FES) F^18^")
#define CODE_UMLS_Sonographer                                              DSRBasicCodedEntry("C1954848", "UMLS", "Sonographer")
#define CODE_UMLS_InverseRatioVentilation                                  DSRBasicCodedEntry("C2223982", "UMLS", "Inverse ratio ventilation")
#define CODE_UMLS_RootMeanSquare                                           DSRBasicCodedEntry("C2347976", "UMLS", "Root Mean Square")
#define CODE_UMLS_TimePoint                                                DSRBasicCodedEntry("C2348792", "UMLS", "Time Point")
#define CODE_UMLS_Edotreotide_Ga68                                         DSRBasicCodedEntry("C2713594", "UMLS", "Edotreotide Ga^68^")
#define CODE_UMLS_FluoropropylDihydrotetrabenazine_DTBZ_F18                DSRBasicCodedEntry("C2934038", "UMLS", "Fluoropropyl-dihydrotetrabenazine (DTBZ) F^18^")
#define CODE_UMLS_ISO1_F18                                                 DSRBasicCodedEntry("C2981788", "UMLS", "ISO-1 F^18^")
#define CODE_UMLS_RadiationPhysicist                                       DSRBasicCodedEntry("C2985483", "UMLS", "Radiation Physicist")
#define CODE_UMLS_Pretreatment                                             DSRBasicCodedEntry("C3539075", "UMLS", "Pretreatment")
#define CODE_UMLS_RoomAir                                                  DSRBasicCodedEntry("C3846005", "UMLS", "Room air")

#endif
