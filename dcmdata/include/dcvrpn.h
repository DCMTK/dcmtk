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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRPN_H
#define DCVRPN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmPersonName : public DcmCharString {
public:
    DcmPersonName( const DcmTag &tag,
                   T_VR_UL len = 0,
                   iDicomStream *iDStream = NULL);
    DcmPersonName( const DcmPersonName& old );
    virtual ~DcmPersonName();
    virtual DcmEVR ident() const;
};


#endif // DCVRPN_H

