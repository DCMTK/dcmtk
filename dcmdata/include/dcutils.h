/*
 *
 *  Copyright (C) 1994-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Helper functions for accessing DICOM datasets
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcutils.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCUTILS_H
#define DCUTILS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"
#include "dctagkey.h"

class OFString;
class DcmItem;
class DcmObject;

OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Uint16 &returnVal );
OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Sint16 &returnVal );
OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Uint32 &returnVal );
OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    Sint32 &returnVal );
OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    char * & returnVal );
OFBool getSingleValue(DcmObject *obj, DcmTagKey searchtag, 
		    OFString & returnVal );

OFBool putSingleValue(DcmItem *item, DcmTagKey tag, Uint16 value);
OFBool putSingleValue(DcmItem *item, DcmTagKey tag, Sint16 value);
OFBool putSingleValue(DcmItem *item, DcmTagKey tag, Uint32 value);
OFBool putSingleValue(DcmItem *item, DcmTagKey tag, Sint32 value);
OFBool putSingleValue(DcmItem *item, DcmTagKey tag, const char *value);
OFBool putSingleValue(DcmItem *item, DcmTagKey tag, OFString & value);

OFBool deleteAttribute( DcmItem *item, DcmTagKey searchtag );
OFBool deleteAttribute( DcmItem *item, DcmObject *attribute );

#endif

/*
 * CVS/RCS Log:
 * $Log: dcutils.h,v $
 * Revision 1.5  2000-03-08 16:26:20  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/03/31 09:24:53  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
