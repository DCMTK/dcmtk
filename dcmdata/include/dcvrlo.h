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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRLO_H
#define DCVRLO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmLongString : public DcmCharString {
public:
    DcmLongString( DcmTag &tag );
    DcmLongString( DcmTag &tag,
                   T_VR_UL len,
                   iDicomStream *iDStream );
    DcmLongString( const DcmObject &oldObj );
    DcmLongString( const DcmLongString &newLO );
    virtual ~DcmLongString();
    virtual EVR ident() const;
};


#endif // DCVRLO_H

