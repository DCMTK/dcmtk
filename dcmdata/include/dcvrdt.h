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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDT_H
#define DCVRDT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDateTime : public DcmByteString {
public:
    DcmDateTime( DcmTag &tag );
    DcmDateTime( DcmTag &tag,
                 T_VR_UL len,
                 iDicomStream *iDStream );
    DcmDateTime( const DcmObject &oldObj );
    DcmDateTime( const DcmDateTime &newDT );
    virtual ~DcmDateTime();
    virtual EVR ident() const;
};


#endif // DCVRDT_H

