/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with UCUM Code Definitions (Coding Scheme "UCUM")
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


#ifndef CODES_UCUM_H
#define CODES_UCUM_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  general information  *
 *-----------------------*/

#define CODE_UCUM_CodingSchemeDesignator  "UCUM"
#define CODE_UCUM_CodingSchemeName        "UCUM"
#define CODE_UCUM_CodingSchemeDescription "Unified Code for Units of Measure"
#define CODE_UCUM_CodingSchemeUID         "2.16.840.1.113883.6.8"


/*--------------------*
 *  code definitions  *
 *--------------------*/

// The basic scheme for creating the names is as follows:
//   'CODE_' + <coding-scheme-designator> + ['_RETIRED'] + '_' + <code-name>
// where <coding-scheme-designator> is "UCUM" and <code-name> is either
// - a cleaned camel-case version of the code meaning (if unique within this coding scheme) or
// - a cleaned camel-case version of the code meaning with an underscore and the code value appended or
// - simply the code value with an underscore character appended (if code meaning contains an equation).
// The "cleaned camel-case version of the code meaning" is an attempt to map the free text
// description of the code meaning to a compiler-friendly but still human-readable representation.

#define CODE_UCUM_Pixels            DSRBasicCodedEntry("{pixels}", "UCUM", "pixels")
#define CODE_UCUM_Degrees           DSRBasicCodedEntry("deg", "UCUM", "deg")
#define CODE_UCUM_Minus1To1         DSRBasicCodedEntry("{-1:1}", "UCUM", "{-1:1}")
#define CODE_UCUM_Millimeter        DSRBasicCodedEntry("mm", "UCUM", "millimeter")
#define CODE_UCUM_Centimeter        DSRBasicCodedEntry("cm", "UCUM", "centimeter")
#define CODE_UCUM_cm3               DSRBasicCodedEntry("cm3", "UCUM", "cm3")
#define CODE_UCUM_ml                DSRBasicCodedEntry("ml", "UCUM", "ml")
#define CODE_UCUM_s                 DSRBasicCodedEntry("s", "UCUM", "s")
#define CODE_UCUM_min               DSRBasicCodedEntry("min", "UCUM", "min")
#define CODE_UCUM_Bq                DSRBasicCodedEntry("Bq", "UCUM", "Bq")
#define CODE_UCUM_BqPerMol          DSRBasicCodedEntry("Bq/mol", "UCUM", "Bq/mol")
#define CODE_UCUM_mmolPerL          DSRBasicCodedEntry("mmol/l", "UCUM", "mmol/l")
#define CODE_UCUM_CountsPerSecond   DSRBasicCodedEntry("{counts}/s", "UCUM", "Counts per second")

#endif
