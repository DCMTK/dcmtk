/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrds.h
 *
 * Purpose:
 * Interface of class DcmDecimalString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDS_H
#define DCVRDS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDecimalString : public DcmByteString {
public:
    DcmDecimalString( DcmTag &tag );
    DcmDecimalString( DcmTag &tag,
                      T_VR_UL len,
                      iDicomStream *iDStream );
    DcmDecimalString( const DcmObject &oldObj );
    DcmDecimalString( const DcmDecimalString &newDS );
    virtual ~DcmDecimalString();
    virtual EVR ident() const;
};


#endif // DCVRDS_H

