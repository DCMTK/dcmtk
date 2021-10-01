/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmnet
 *
 *  Author: Michael Onken
 *
 *  Purpose: Collection of helper functions
 *
 */

#ifndef DCMHET_HELPERS_H
#define DCMHET_HELPERS_H

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmnet/dulstruc.h"

struct T_ASC_Parameters;
class LST_HEAD;


void
destroyPresentationContextList(LST_HEAD ** l);

void
destroyUserInformationLists(DUL_USERINFO * userInfo);

#endif
