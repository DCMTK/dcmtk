/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrpn.h
 *
 * Purpose:
 * Interface of class DcmPersonName
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRPN_H
#define DCVRPN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmPersonName : public DcmCharString {
public:
    DcmPersonName( DcmTag &tag );
    DcmPersonName( DcmTag &tag,
                   T_VR_UL len,
                   iDicomStream *iDStream );
    DcmPersonName( const DcmObject &oldObj );
    DcmPersonName( const DcmPersonName &newPN );
    virtual ~DcmPersonName();
    virtual EVR ident() const;
};


#endif // DCVRPN_H

