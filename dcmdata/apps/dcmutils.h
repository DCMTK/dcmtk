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
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctk.h"


BOOL getSingleValueUS(	       DcmObject *obj, ETag searchtag, T_VR_US &returnVal );
BOOL getSingleValueSS(	       DcmObject *obj, ETag searchtag, T_VR_SS &returnVal );
BOOL getSingleValueUL(	       DcmObject *obj, ETag searchtag, T_VR_UL &returnVal );
BOOL getSingleValueSL(	       DcmObject *obj, ETag searchtag, T_VR_SL &returnVal );
BOOL getSingleValueByteString( DcmObject *obj, ETag searchtag, char   *&returnVal );

BOOL putSingleValue( DcmItem *item, ETag tag, T_VR_US value );
BOOL putSingleValue( DcmItem *item, ETag tag, T_VR_SS value );
BOOL putSingleValue( DcmItem *item, ETag tag, T_VR_UL value );
BOOL putSingleValue( DcmItem *item, ETag tag, T_VR_SL value );
BOOL putSingleValue( DcmItem *item, ETag tag, char   *value );

BOOL deleteAttribute( DcmItem *item, ETag searchtag );
BOOL deleteAttribute( DcmItem *item, DcmObject *attribute );

