/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrcs.h
 *
 * Purpose:
 * Interface of class DcmCodeString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRCS_H
#define DCVRCS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmCodeString : public DcmByteString {
public:
    DcmCodeString( DcmTag &tag );
    DcmCodeString( DcmTag &tag,
                   T_VR_UL len,
                   iDicomStream *iDStream );
    DcmCodeString( const DcmObject &oldObj );
    DcmCodeString( const DcmCodeString &newCS );
    virtual ~DcmCodeString();
    virtual EVR ident() const;
};


#endif // DCVRCS_H

