/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**
**  This software and supporting documentation were
**  developed by
**
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**
**  for CEN/TC251/WG4 as a contribution to the Radiological
**  Society of North America (RSNA) 1993 Digital Imaging and
**  Communications in Medicine (DICOM) Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE
**  IS WITH THE USER.
**
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice.
**
*/

/*
**
** Author: Andrew Hewett                Created: 11-08-93
**
** Module: diutil
**
** Purpose:
**     This file contains the interface to
**     some general useful dicom utility routines
**
** Module Prefix: DU_
**
*/

#ifndef DIUTIL_H
#define DIUTIL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/oflog/oflog.h"


extern DCMTK_DCMNET_EXPORT OFLogger DCM_dcmnetLogger;

#define DCMNET_LOGGER_NAME "dcmtk.dcmnet"

#define DCMNET_TRACE(msg) OFLOG_TRACE(DCM_dcmnetLogger, msg)
#define DCMNET_DEBUG(msg) OFLOG_DEBUG(DCM_dcmnetLogger, msg)
#define DCMNET_INFO(msg)  OFLOG_INFO(DCM_dcmnetLogger, msg)
#define DCMNET_WARN(msg)  OFLOG_WARN(DCM_dcmnetLogger, msg)
#define DCMNET_ERROR(msg) OFLOG_ERROR(DCM_dcmnetLogger, msg)
#define DCMNET_FATAL(msg) OFLOG_FATAL(DCM_dcmnetLogger, msg)


DCMTK_DCMNET_EXPORT void DU_stripTrailingSpaces(char *s);
DCMTK_DCMNET_EXPORT void DU_stripLeadingSpaces(char *s);
DCMTK_DCMNET_EXPORT void DU_stripLeadingAndTrailingSpaces(char *s);

DCMTK_DCMNET_EXPORT OFBool DU_getStringDOElement(DcmItem *obj, DcmTagKey t, char *s);
DCMTK_DCMNET_EXPORT OFBool DU_putStringDOElement(DcmItem *obj, DcmTagKey t, const char *s);
DCMTK_DCMNET_EXPORT OFBool DU_getShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 *us);
DCMTK_DCMNET_EXPORT OFBool DU_putShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 us);

DCMTK_DCMNET_EXPORT OFBool DU_findSOPClassAndInstanceInDataSet(
  DcmItem *obj,
  char* sopClass,
  char* sopInstance,
  OFBool tolerateSpacePaddedUIDs = OFFalse);

DCMTK_DCMNET_EXPORT OFBool DU_findSOPClassAndInstanceInFile(
  const char *fname,
  char* sopClass,
  char* sopInstance,
  OFBool tolerateSpacePaddedUIDs = OFFalse);

DCMTK_DCMNET_EXPORT const char *DU_cechoStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_cstoreStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_cfindStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_cmoveStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_cgetStatusString(Uint16 statusCode);

DCMTK_DCMNET_EXPORT const char *DU_ncreateStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_ngetStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_nsetStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_nactionStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_ndeleteStatusString(Uint16 statusCode);
DCMTK_DCMNET_EXPORT const char *DU_neventReportStatusString(Uint16 statusCode);

/** Logs result of a select() call. According to the select() documentation,
 *  a value of > 1 means that data has arrived. This is logged on TRACE level
 *  since it should be a very regular case. If select() returns 0, a timeout
 *  has occured, and in case of -1 another error happened (e.g. interrupt).
 *  These two events are logged to DEBUG log level.
 *  Note that this function makes use of errno (Unix) or WSAGetLastError()
 *  (Windows aka Winsock), so it must be called right after the select() call
 *  since otherwise the error might be already cleared or modified.
 *  @param  selectReturnValue The return value returned by select()
 */
DCMTK_DCMNET_EXPORT void DU_logSelectResult(int selectReturnValue);

#endif
