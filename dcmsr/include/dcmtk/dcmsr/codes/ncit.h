/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with NCIt Code Definitions (Coding Scheme "NCIt")
 *
 *  Generated semi-automatically from DICOM PS 3.16-2015c
 *  File created on 2016-01-12 18:02:09 by J. Riesmeier
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

// total number of codes: 2
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

#define CODE_NCIt_RANO             DSRBasicCodedEntry("C114879", "NCIt", "RANO")
#define CODE_NCIt_ActivitySession  DSRBasicCodedEntry("C67447", "NCIt", "Activity Session")   // added from CP-1466

#endif
