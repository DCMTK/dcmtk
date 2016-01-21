/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: include most dcmdata files that are usually required
 *
 */

#ifndef DCTK_H
#define DCTK_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

// various headers
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcostrma.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/ofstd/ofdefine.h"

// tags and dictionary
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dchashdi.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdeftag.h"

// basis classes
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcelem.h"

// classes for management of sequences and other lists
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcpixseq.h"

// element classes for string management (8-bit)
#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrur.h"

// element classes for string management (8-bit and/or multi-byte)
#include "dcmtk/dcmdata/dcchrstr.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrst.h"
#include "dcmtk/dcmdata/dcvruc.h"
#include "dcmtk/dcmdata/dcvrut.h"

// element class for byte and word value representations
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcovlay.h"

// element classes for binary value fields
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrulup.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrod.h"
#include "dcmtk/dcmdata/dcvrol.h"

// misc supporting tools
#include "dcmtk/dcmdata/cmdlnarg.h"

#endif /* DCTK_H */
