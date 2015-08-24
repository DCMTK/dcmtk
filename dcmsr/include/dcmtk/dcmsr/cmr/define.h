/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file with preprocessor definitions for submodule "dcmsr/cmr"
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_DEFINE_H
#define CMR_DEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef cmr_EXPORTS
#define DCMTK_CMR_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_CMR_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
