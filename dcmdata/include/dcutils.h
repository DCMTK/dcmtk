/*
 *
 * Author: Gerd Ehlers		Created:  11-06-94
 *				Modified: 11-17-94
 *
 * Module: dcutils.h
 *
 * Purpose:
 * Interface of some routines for accessing DICOM datasets
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifndef DCUTILS_H
#define DCUTILS_H

#include "dcelem.h"
#include "dcitem.h"


BOOL getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Uint16 &returnVal );
BOOL getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Sint16 &returnVal );
BOOL getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Uint32 &returnVal );
BOOL getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Sint32 &returnVal );
BOOL getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    char * & returnVal );

BOOL putSingleValue(DcmItem *item, DcmTagKey tag, Uint16 value);
BOOL putSingleValue(DcmItem *item, DcmTagKey tag, Sint16 value);
BOOL putSingleValue(DcmItem *item, DcmTagKey tag, Uint32 value);
BOOL putSingleValue(DcmItem *item, DcmTagKey tag, Sint32 value);
BOOL putSingleValue(DcmItem *item, DcmTagKey tag, const char *value);

BOOL deleteAttribute( DcmItem *item, DcmTagKey searchtag );
BOOL deleteAttribute( DcmItem *item, DcmObject *attribute );

#endif
