/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
**          dicom.h
**
**  This header defines public typedefs for the DICOM software produced
**  by the CEN Dicom Development Team.  These definitions are
**  meant to be used in a number of packages and need to be in a central
**  location.
**
**  The header file has evolved from a version written for the
**  RSNA'92 InfoRAD DICOM Demonstration by:
**
**      Stephen Moore (smm@wuerl.wustl.edu)
**      Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**      Washington University School of Medicine
**      23-May-1992
**
**
**  Modifications:
**      Andrew Hewett
**      OFFIS/University of Oldenburg, Germany.
**
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 2010-12-01 08:26:10 $
** CVS/RCS Revision: $Revision: 1.14 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DICOM_H
#define DICOM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/cond.h"		/* condition typedefs */
#include "dcmtk/dcmnet/dcompat.h"	/* compatability routines */
#include "dcmtk/dcmdata/dctypes.h"	/* dcmdata toolkit basic types */
#include "dcmtk/dcmdata/dcuid.h"	/* dcmdata UID definitions */

/*
 * Useful Types
 */

/* these types are for backwards compatibility */
typedef Uint16	U_SHORT;	/* normal unsigned short*/
typedef Uint32	U_LONG;		/* normal unsigned long */
typedef Uint32	MASK_32;	/* for bit masks */


#define DICOM_STDAPPLICATIONCONTEXT UID_StandardApplicationContext
#define DICOM_NET_IMPLEMENTATIONCLASSUID OFFIS_IMPLEMENTATION_CLASS_UID
#define DICOM_NET_IMPLEMENTATIONVERSIONNAME OFFIS_DTK_IMPLEMENTATION_VERSION_NAME

/* network type constants as used by the DUL layer */
#define	DICOM_APPLICATION_ACCEPTOR	0x01
#define	DICOM_APPLICATION_REQUESTOR	0x02

/*
** The following defines are used in some levels of the network code.
*/

#define DIC_US_LEN		sizeof(DIC_US)
#define DIC_UL_LEN		sizeof(DIC_UL)
#define DIC_UI_LEN		64
#define DIC_AE_LEN		16
#define DIC_SH_LEN		16
#define DIC_PN_LEN		64
#define DIC_LO_LEN		64
#define DIC_CS_LEN		16
#define DIC_DS_LEN		16
#define DIC_IS_LEN		12
#define DIC_AT_LEN		(2*sizeof(DIC_US))
#define DIC_NODENAME_LEN	128

#define DICOM_UI_LENGTH DIC_UI_LEN

typedef Uint16	DIC_US;
typedef Uint32	DIC_UL;
typedef char 	DIC_UI[DIC_UI_LEN + 1];
typedef char	DIC_AE[DIC_AE_LEN + 1];
typedef char	DIC_SH[DIC_SH_LEN + 1];
typedef char	DIC_PN[DIC_PN_LEN + 1];
typedef char	DIC_LO[DIC_LO_LEN + 1];
typedef char	DIC_CS[DIC_CS_LEN + 1];
typedef char	DIC_DS[DIC_DS_LEN + 1];
typedef char	DIC_IS[DIC_IS_LEN + 1];
typedef char	DIC_NODENAME[DIC_NODENAME_LEN + 1];


/*
 * Useful Macros
 */

#ifndef MAX
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef IS_EVEN
#define IS_EVEN(i) (~(i) & 0x01)
#endif
#ifndef DIM_OF
#define DIM_OF(a) (sizeof(a) / sizeof(a[0]))
#endif
#ifndef IN_RANGE
#define IN_RANGE(n, lo, hi) ((lo) <= n && (n) <= (hi))
#endif

#endif

/*
** CVS Log
** $Log: dicom.h,v $
** Revision 1.14  2010-12-01 08:26:10  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.13  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2005/12/08 16:02:20  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.11  2004/08/03 11:42:43  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.10  2002/11/27 13:04:35  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.9  2001/10/12 10:18:26  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.8  2001/09/26 12:28:59  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.7  2000/08/10 14:50:53  meichel
** Added initial OpenSSL support.
**
** Revision 1.6  2000/02/23 15:12:25  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.5  2000/02/01 10:24:06  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.4  1997/07/21 08:40:09  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1997/02/06 12:15:41  hewett
** Updated preliminary Apple Macintosh support for the Metrowerks CodeWarrior
** version 11 compiler and environment.
**
** Revision 1.2  1996/09/27 08:29:42  hewett
** Support for Win32.  Now does not define a type BOOLEAN if <WINSOCK.H>
** is included.  Renames the ERROR(A) macro to be CONDERROR(A) macro due
** to a clash with Windows include files.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
