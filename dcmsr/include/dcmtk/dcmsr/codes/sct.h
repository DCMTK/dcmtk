/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with SNOMED-CT Code Definitions (Coding Scheme "SCT")
 *
 *  Generated semi-automatically from DICOM PS 3.16-2015c (for "SRT")
 *  File created on 2019-04-11 by J. Riesmeier
 *  Last modified on 2019-04-11 by J. Riesmeier
 *
 *  NB: This file does not yet contain all codes defined in PS 3.16 since the
 *      mapping of the code meaning to a compiler-friendly representation is
 *      much more complex than for other coding schemes.
 *
 */


#ifndef CODES_SCT_H
#define CODES_SCT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_SCT_CodingSchemeDesignator  "SCT"
#define CODE_SCT_CodingSchemeName        "SNOMED CT"
#define CODE_SCT_CodingSchemeDescription "SNOMED, using the CT code values"
#define CODE_SCT_CodingSchemeUID         "2.16.840.1.113883.6.96"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// The basic scheme for creating the names is as follows:
//   'CODE_' + <coding-scheme-designator> + ['_RETIRED'] + '_' + <code-name>
// where <coding-scheme-designator> is "SCT" and <code-name> is either
// - a cleaned camel-case version of the code meaning (if unique within this coding scheme) or
// - a cleaned camel-case version of the code meaning with an underscore and the code value appended or
// - simply the code value with an underscore character appended (if code meaning contains an equation).
// The "cleaned camel-case version of the code meaning" is an attempt to map the free text
// description of the code meaning to a compiler-friendly but still human-readable representation.

#define CODE_SCT_Radionuclide                   DSRBasicCodedEntry("89457008", "SCT", "Radionuclide")
#define CODE_SCT_18_Fluorine                    DSRBasicCodedEntry("77004003", "SCT", "^18^Fluorine")
#define CODE_SCT_Fluorodeoxyglucose_F18         DSRBasicCodedEntry("35321007", "SCT", "Fluorodeoxyglucose F^18^")
#define CODE_SCT_RadiopharmaceuticalAgent       DSRBasicCodedEntry("417881006", "SCT", "Radiopharmaceutical agent")
#define CODE_SCT_TopographicalModifier          DSRBasicCodedEntry("106233006", "SCT", "Topographical modifier")
#define CODE_SCT_MeasurementMethod              DSRBasicCodedEntry("370129005", "SCT", "Measurement Method")
#define CODE_SCT_FindingSite                    DSRBasicCodedEntry("363698007", "SCT", "Finding Site")
#define CODE_SCT_Laterality                     DSRBasicCodedEntry("272741003", "SCT", "Laterality")
#define CODE_SCT_RouteOfAdministration          DSRBasicCodedEntry("410675002", "SCT", "Route of Administration")
#define CODE_SCT_Neoplasm_Primary               DSRBasicCodedEntry("86049000", "SCT", "Neoplasm, Primary")
#define CODE_SCT_Neoplasm_Secondary             DSRBasicCodedEntry("14799000", "SCT", "Neoplasm, Secondary")
#define CODE_SCT_ImagingProcedure               DSRBasicCodedEntry("363679005", "SCT", "Imaging procedure")
#define CODE_SCT_HalfLifeOfRadiopharmaceutical  DSRBasicCodedEntry("304283002", "SCT", "Half-life of radiopharmaceutical")
#define CODE_SCT_AorticArch                     DSRBasicCodedEntry("57034009", "SCT", "Aortic arch")
#define CODE_SCT_Liver                          DSRBasicCodedEntry("10200004", "SCT", "Liver")
#define CODE_SCT_Cerebellum                     DSRBasicCodedEntry("113305005", "SCT", "Cerebellum")
#define CODE_SCT_LymphNode                      DSRBasicCodedEntry("59441001", "SCT", "Lymph node")

#endif
