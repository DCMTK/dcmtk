/*
 *
 * Author: Gerd Ehlers		Created:  11-06-94
 *				Modified: 11-17-94
 *
 * Module: dcmutils.h
 *
 * Purpose:
 * Interface of some routines for accessing DICOM datasets
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctk.h"


BOOL getSingleValueUS( DcmObject *obj, DcmTagKey searchtag, 
		       T_VR_US &returnVal );
BOOL getSingleValueSS( DcmObject *obj, DcmTagKey searchtag, 
		       T_VR_SS &returnVal );
BOOL getSingleValueUL( DcmObject *obj, DcmTagKey searchtag, 
		       T_VR_UL &returnVal );
BOOL getSingleValueSL( DcmObject *obj, DcmTagKey searchtag, 
		       T_VR_SL &returnVal );
BOOL getSingleValueByteString( DcmObject *obj, DcmTagKey searchtag, 
			       const char *&returnVal );

BOOL putSingleValue( DcmItem *item, DcmTagKey tag, T_VR_US value );
BOOL putSingleValue( DcmItem *item, DcmTagKey tag, T_VR_SS value );
BOOL putSingleValue( DcmItem *item, DcmTagKey tag, T_VR_UL value );
BOOL putSingleValue( DcmItem *item, DcmTagKey tag, T_VR_SL value );
BOOL putSingleValue( DcmItem *item, DcmTagKey tag, const char *value );

BOOL deleteAttribute( DcmItem *item, DcmTagKey searchtag );
BOOL deleteAttribute( DcmItem *item, DcmObject *attribute );

