/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
#include "dcmtk/dcmdata/dcvm.h"
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

// element classes for string management (8-bit and/or 16-bit in later extensions)
#include "dcmtk/dcmdata/dcchrstr.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrst.h"
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

// misc supporting tools
#include "dcmtk/dcmdata/cmdlnarg.h"

#endif /* DCTK_H */

/*
 * CVS/RCS Log:
 * $Log: dctk.h,v $
 * Revision 1.18  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.17  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.16  2009-09-28 13:29:38  joergr
 * Moved general purpose definition file from module dcmdata to ofstd, and
 * added new defines in order to make the usage easier.
 *
 * Revision 1.15  2005/12/08 16:28:46  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.14  2005/11/28 15:28:56  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.13  2002/12/06 12:19:30  joergr
 * Added support for new value representation Other Float String (OF).
 *
 * Revision 1.12  2002/08/27 16:55:40  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.11  2001/06/01 15:48:45  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:19  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/02/29 11:48:38  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.8  1999/03/31 09:24:50  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
