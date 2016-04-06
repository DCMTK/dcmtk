/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with NCIt Code Definitions (Coding Scheme "NCIt")
 *
 *  Generated automatically from DICOM PS 3.16-2016b
 *  File created on 2016-04-06 16:08:01 by J. Riesmeier
 *
 */


#ifndef CODES_NCIT_H
#define CODES_NCIT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_NCIt_CodingSchemeDesignator  "NCIt"
#define CODE_NCIt_CodingSchemeName        "NCIt"
#define CODE_NCIt_CodingSchemeDescription "NCI Thesaurus"
#define CODE_NCIt_CodingSchemeUID         "2.16.840.1.113883.3.26.1.1"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// total number of codes: 16
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

#define CODE_NCIt_RANO                                                     DSRBasicCodedEntry("C114879", "NCIt", "RANO")
#define CODE_NCIt_BronchioloalveolarAdenocarcinoma                         DSRBasicCodedEntry("C2923", "NCIt", "Bronchioloalveolar adenocarcinoma")
#define CODE_NCIt_ExtraluminalRoute                                        DSRBasicCodedEntry("C38213", "NCIt", "Extraluminal route")
#define CODE_NCIt_IntraepithelialRoute                                     DSRBasicCodedEntry("C38244", "NCIt", "Intraepithelial route")
#define CODE_NCIt_TransluminalRoute                                        DSRBasicCodedEntry("C38306", "NCIt", "Transluminal route")
#define CODE_NCIt_TemperatureSensorDeviceComponent                         DSRBasicCodedEntry("C50304", "NCIt", "Temperature sensor device component")
#define CODE_NCIt_AdLibitum                                                DSRBasicCodedEntry("C64636", "NCIt", "ad libitum")
#define CODE_NCIt_ActivitySession                                          DSRBasicCodedEntry("C67447", "NCIt", "Activity Session")
#define CODE_NCIt_Erect                                                    DSRBasicCodedEntry("C86043", "NCIt", "erect")
#define CODE_NCIt_BeddingChange                                            DSRBasicCodedEntry("C90365", "NCIt", "Bedding change")
#define CODE_NCIt_BeddingMaterial                                          DSRBasicCodedEntry("C90366", "NCIt", "Bedding material")
#define CODE_NCIt_EnvironmentalTemperature                                 DSRBasicCodedEntry("C90380", "NCIt", "Environmental temperature")
#define CODE_NCIt_HousingHumidity                                          DSRBasicCodedEntry("C90395", "NCIt", "Housing humidity")
#define CODE_NCIt_LightCycle                                               DSRBasicCodedEntry("C90419", "NCIt", "Light cycle")
#define CODE_NCIt_WaterDelivery                                            DSRBasicCodedEntry("C90486", "NCIt", "Water delivery")
#define CODE_NCIt_ReferenceRegion                                          DSRBasicCodedEntry("C94970", "NCIt", "Reference Region")

#endif
