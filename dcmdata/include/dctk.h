/*
 * 
 * Author: Gerd Ehlers      Created:  03-06-94
 *                          Modified: 07-17-95
 * 
 * Module: dctk.h
 * 
 * Purpose:
 * Collection of necessary Header-Files for users
 *
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.3 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCTK_H
#define DCTK_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

// various headers
#include "dctypes.h"
#include "dcswap.h"
#include "dcbuf.h"
#include "dcstream.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcuid.h"
#include "dcvm.h"
#include "dcdefine.h"
#include "dcdebug.h"

// tags and dictionary
#include "pix.h"
#include "dctagkey.h"
#include "dctag.h"
#include "dcdicent.h"
#include "dcentdef.h"
#include "dcentlst.h"
#include "dcentbst.h"
#include "dcentset.h"
#include "dcdict.h"
#include "dcdeftag.h"

// basis classes
#include "dcobject.h"
#include "dcelem.h"

// classes for management of sequences and other lists
#include "dcitem.h"
#include "dcmetinf.h"
#include "dcdatset.h"
#include "dcsequen.h"
#include "dcfilefo.h"
#include "dcdicdir.h"
#include "dcpixseq.h"

// element classes for string management (8-bit)
#include "dcbytstr.h"
#include "dcvrae.h"
#include "dcvras.h"
#include "dcvrcs.h"
#include "dcvrda.h"
#include "dcvrds.h"
#include "dcvrdt.h"
#include "dcvris.h"
#include "dcvrtm.h"
#include "dcvrui.h"

// element classes for string management (8-bit and/or 16-bit in later extensions)
#include "dcchrstr.h"
#include "dcvrlo.h"
#include "dcvrlt.h"
#include "dcvrpn.h"
#include "dcvrsh.h"
#include "dcvrst.h"

// element class for byte and word streams
#include "dcvrobow.h"
#include "dcpxitem.h"

// element classes for binary value fields
#include "dcvrat.h"
#include "dcvrss.h"
#include "dcvrus.h"
#include "dcvrsl.h"
#include "dcvrul.h"
#include "dcvrulup.h"
#include "dcvrfl.h"
#include "dcvrfd.h"

// misc supporting tools
#include "cmdlnarg.h"

#endif // DCTK_H

