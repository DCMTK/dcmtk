/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with SNOMED-RT Code Definitions (Coding Scheme "SRT")
 *
 *  Generated semi-automatically from DICOM PS 3.16-2015c
 *  File created on 2015-08-24 by J. Riesmeier
 *  Last modified on 2016-02-22 by Riesmeier
 *
 *  NB: This file does not yet contain all codes defined in PS 3.16 since the
 *      mapping of the code meaning to a compiler-friendly representation is
 *      much more complex than for other coding schemes.
 *
 */


#ifndef CODES_SRT_H
#define CODES_SRT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_SRT_CodingSchemeDesignator  "SRT"
#define CODE_SRT_CodingSchemeName        "SNOMED-RT"
#define CODE_SRT_CodingSchemeDescription "SNOMED-CT, using the 'SNOMED-RT style' code values"
#define CODE_SRT_CodingSchemeUID         "2.16.840.1.113883.6.96"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// The basic scheme for creating the names is as follows:
//   'CODE_' + <coding-scheme-designator> + ['_RETIRED'] + '_' + <code-name>
// where <coding-scheme-designator> is "SRT" and <code-name> is either
// - a cleaned camel-case version of the code meaning (if unique within this coding scheme) or
// - a cleaned camel-case version of the code meaning with an underscore and the code value appended or
// - simply the code value with an underscore character appended (if code meaning contains an equation).
// The "cleaned camel-case version of the code meaning" is an attempt to map the free text
// description of the code meaning to a compiler-friendly but still human-readable representation.

#define CODE_SRT_Radionuclide                   DSRBasicCodedEntry("C-10072", "SRT", "Radionuclide")
#define CODE_SRT_18_Fluorine                    DSRBasicCodedEntry("C-111A1", "SRT", "^18^Fluorine")
#define CODE_SRT_Fluorodeoxyglucose_F18         DSRBasicCodedEntry("C-B1031", "SRT", "Fluorodeoxyglucose F^18^")
#define CODE_SRT_RadiopharmaceuticalAgent       DSRBasicCodedEntry("F-61FDB", "SRT", "Radiopharmaceutical agent")
#define CODE_SRT_MeasurementMethod              DSRBasicCodedEntry("G-C036", "SRT", "Measurement Method")
#define CODE_SRT_FindingSite                    DSRBasicCodedEntry("G-C0E3", "SRT", "Finding Site")
#define CODE_SRT_RouteOfAdministration          DSRBasicCodedEntry("G-C340", "SRT", "Route of Administration")
#define CODE_SRT_Neoplasm_Primary               DSRBasicCodedEntry("M-80003", "SRT", "Neoplasm, Primary")
#define CODE_SRT_Neoplasm_Secondary             DSRBasicCodedEntry("M-80006", "SRT", "Neoplasm, Secondary")
#define CODE_SRT_HalfLifeOfRadiopharmaceutical  DSRBasicCodedEntry("R-42806", "SRT", "Half-life of radiopharmaceutical")
#define CODE_SRT_AorticArch                     DSRBasicCodedEntry("T-42300", "SRT", "Aortic arch")
#define CODE_SRT_Liver_T62000                   DSRBasicCodedEntry("T-62000", "SRT", "Liver")
#define CODE_SRT_Liver_T62002                   DSRBasicCodedEntry("T-62002", "SRT", "Liver")
#define CODE_SRT_Cerebellum                     DSRBasicCodedEntry("T-A6000", "SRT", "Cerebellum")
#define CODE_SRT_LymphNode                      DSRBasicCodedEntry("T-C4000", "SRT", "Lymph node")

#endif
