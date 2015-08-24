/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file with logger definition for submodule "dcmsr/cmr"
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/logger.h"


/*--------------------*
 *  global variables  *
 *--------------------*/

OFLogger DCM_dcmsrCmrLogger = OFLog::getLogger("dcmtk.dcmsr.cmr");
