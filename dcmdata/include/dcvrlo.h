/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrlo.h
 *
 * Purpose:
 * Interface of class DcmLongString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRLO_H
#define DCVRLO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmLongString : public DcmCharString {
public:
    DcmLongString( const DcmTag &tag,
                   T_VR_UL len = 0,
                   iDicomStream *iDStream = NULL);
    DcmLongString( const DcmLongString& old );
    virtual ~DcmLongString();
    virtual DcmEVR ident() const;
};


#endif // DCVRLO_H

