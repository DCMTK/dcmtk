/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrdt.h
 *
 * Purpose:
 * Interface of class DcmDateTime
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDT_H
#define DCVRDT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDateTime : public DcmByteString {
public:
    DcmDateTime( const DcmTag &tag,
                 T_VR_UL len = 0,
                 iDicomStream *iDStream = NULL);
    DcmDateTime( const DcmDateTime &newDT );
    virtual ~DcmDateTime();
    virtual DcmEVR ident() const;
};


#endif // DCVRDT_H

